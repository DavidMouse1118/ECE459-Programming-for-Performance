#include "Student.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <cassert>
#include <mutex>
#include <openssl/sha.h>

using std::cout;
using std::cerr;
using std::endl;

Student::Student(EventQueue* eq, int id)
    : ChecksumTracker<Student, ChecksumType::PACKAGE>()
    , ChecksumTracker<Student, ChecksumType::IDEA>()
    , eq(eq)
    , id(id)
{
    // nop
}

Student::~Student() {
    // nop
}

uint8_t* Student::getIdeaChecksum() {
    return sha256(currentIdea->getName());
}

uint8_t* Student::getPackagesChecksum() {
    // A really silly checksum that mainly serves to waste time to
    // make the Student thread seem like it's doing something besides
    // always waiting for a condition var
    //
    // A checksum (in this simulation) is defined to be the xor of all
    // the sha256 hashes of the current project's packages

    uint8_t* checksum = initChecksum();

    for (int i = 0; i < currentIdea->getNumPackagesReq(); i++) {
        std::string packageName = currentPackages[i]->getName();
        uint8_t* hash = sha256(packageName);
        xorChecksum(checksum, hash);
        delete [] hash;
    }

    return checksum;
}

void Student::buildIdea() {
    // No project idea right now
    if (!currentIdea) {
        return;
    }

    // Not enough packages available
    if (currentIdea->getNumPackagesReq() > currentPackages.size()) {
        return;
    }

    // Present the idea
    {
        uint8_t* ideaChecksum = getIdeaChecksum();
        ChecksumTracker<Student, ChecksumType::IDEA>::updateGlobalChecksum(ideaChecksum);

        uint8_t* packagesChecksum = getPackagesChecksum();
        ChecksumTracker<Student, ChecksumType::PACKAGE>::updateGlobalChecksum(packagesChecksum);

        std::unique_lock<std::mutex> stdoutLock(eq->stdoutMutex);

        cout << endl;
        cout << "Student " << id << " built <" << currentIdea->getName() << "> (" << currentIdea->getNumPackagesReq() << " Packages)" << endl;
        cout << "    ideaChecksum: " <<  bytesToString(ideaChecksum, SHA256_DIGEST_LENGTH) << endl;
        cout << "packagesChecksum: " << bytesToString(packagesChecksum, SHA256_DIGEST_LENGTH) << endl;
        for (int i = 0; i < currentIdea->getNumPackagesReq(); i++) {
            cout << "> " << currentPackages[i]->getName() << endl;
        }
    }

    // Garbage collect everything
    for (int i = 0; i < currentIdea->getNumPackagesReq(); i++) {
        Package* p = currentPackages.pop();
        delete p;
    }

    delete currentIdea;
    currentIdea = nullptr;
}

void Student::run() {
    while (true) {
        Event e = eq->dequeueEvent();

        switch (e.getType()) {
            case Event::OUT_OF_IDEAS: {
                // All queue accesses here must be a single atomic transaction to guarantee
                // terminator is always at end of queue.
                std::unique_lock<std::recursive_mutex> transactionLock(eq->queueMutex);

                // Someone else can take care of the rest
                bool canTerminate = (!eq->isEmpty() && eq->peek().getType() == Event::OUT_OF_IDEAS);

                // Give incomplete idea for someone else to finish
                if (currentIdea) {
                    eq->enqueueEvent(Event(Event::NEW_IDEA, currentIdea), true);
                    currentIdea = nullptr;
                }

                // Give unused packages for someone else to use
                for (int i = 0; i < currentPackages.size(); i++) {
                    eq->enqueueEvent(Event(Event::DOWNLOAD_COMPLETE, currentPackages[i]), true);
                }
                currentPackages.clear();

                // We can finish if we didn't add any events to front of queue
                canTerminate |= (eq->isEmpty());

                if (canTerminate) {
                    return;
                } else {
                    eq->enqueueEvent(e);
                    continue;
                }
            }

            case Event::NEW_IDEA: {
                if (currentIdea) {
                    // Already working on something right now
                    // Put idea back into the queue
                    eq->enqueueEvent(e);
                    continue;
                }
             
                currentIdea = (Idea*) e.getData()->clone();
                break;
            }

            case Event::DOWNLOAD_COMPLETE: {
                currentPackages.push((Package*) e.getData()->clone());
                break;
            }

            default: {
                cerr << "Unknown EventType:" << e.getType() << endl;
                exit(1);
            }
        }

        // See if we can build current idea
        buildIdea();
    }
}
