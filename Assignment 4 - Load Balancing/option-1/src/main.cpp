#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <map>

#include "EventQueue.h"
#include "PackageDownloader.h"
#include "IdeaGenerator.h"
#include "Student.h"

using std::cout;
using std::cerr;
using std::endl;

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

enum Args {
    Ideas,
    IdeaGenerators,
    Packages,
    PackageDownloaders,
    Students,
    // Add new args above
    NUM_ARGS,
};

std::map<int, std::string> argNames = {
    {Ideas, "numIdeas"},
    {IdeaGenerators, "numIdeaGenerators"},
    {Packages, "numPackages"},
    {PackageDownloaders, "numPackageDownloaders"},
    {Students, "numStudents"},
};

//-----------------------------------------------------------------------------
// Hackathon
//-----------------------------------------------------------------------------

int getPerThreadAmount(int i, int totalPool, int numThreads) {
    int perThread = totalPool / numThreads;
    int extras = totalPool % numThreads;
    return perThread + (i < extras ? 1 : 0);
}

void runHackathon(const int args[]) {
    EventQueue eq;
    std::vector<std::thread> threads;

    // Spawn Student threads
    for (int i = 0; i < args[Students]; i++) {
        threads.push_back(std::thread([&, i] {
            Student student(&eq, i);
            student.run();
        }));
    }

    // Spawn PackageDownloader threads
    int lastPackageIdx = -1;
    int packagesPerThread =  args[Packages] / args[PackageDownloaders];
    int threadsWithExtraPackages = args[Packages] % args[PackageDownloaders];

    for (int i = 0; i < args[PackageDownloaders]; i++) {
        int startIdx = lastPackageIdx + 1;
        int endIdx = startIdx + (packagesPerThread - 1) + (i < threadsWithExtraPackages ? 1 : 0);
        assert(startIdx >= 0 && startIdx <= endIdx);
        assert(endIdx >= 0 && endIdx < args[Packages]);
        lastPackageIdx = endIdx;

        threads.push_back(std::thread([&, i, startIdx, endIdx]{
            PackageDownloader downloader(&eq, startIdx, endIdx);
            downloader.run();
        }));
    }

    assert(lastPackageIdx == args[Packages] - 1);

    // Spawn IdeaGenerator threads
    int lastIdeaIdx = -1;
    int ideasPerThread =  args[Ideas] / args[IdeaGenerators];
    int threadsWithExtraIdeas = args[Ideas] % args[IdeaGenerators];

    for (int i = 0; i < args[IdeaGenerators]; i++) {
        int startIdx = lastIdeaIdx + 1;
        int endIdx = startIdx + (ideasPerThread - 1) + (i < threadsWithExtraIdeas ? 1 : 0);
        assert(startIdx >= 0 && startIdx <= endIdx);
        assert(endIdx >= 0 && endIdx < args[Ideas]);
        lastIdeaIdx = endIdx;

        threads.push_back(std::thread([&, i, startIdx, endIdx]{
            int numPackages = getPerThreadAmount(i, args[Packages], args[IdeaGenerators]);
            int numStudents = getPerThreadAmount(i, args[Students], args[IdeaGenerators]);

            IdeaGenerator generator(&eq, startIdx, endIdx, numPackages, numStudents);
            generator.run();
        }));
    }

    assert(lastIdeaIdx == args[Ideas] - 1);

    // Wait for threads to finish
    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }

    cout << endl;
    cout << "Global Checksums:" << endl;
    cout << "IdeaGenerator     " << IdeaGenerator::getGlobalChecksum() << endl;
    cout << "Student<IDEA>     " << Student::ChecksumTracker<Student, ChecksumType::IDEA>::getGlobalChecksum() << endl;
    cout << "PackageDownloader " << PackageDownloader::getGlobalChecksum() << endl;
    cout << "Student<PACKAGE>  " << Student::ChecksumTracker<Student, ChecksumType::PACKAGE>::getGlobalChecksum() << endl;
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

void usage(char const *argv[], int defaultArgs[]) {
    cout << "Usage: " << argv[0];

    for (int i = 0; i <= NUM_ARGS; i++) {
        cout << " " << argNames[i];
    }

    cout << endl;
    cout << endl;

    cout << "Defaults:" << endl;
    for (int i = 0; i < NUM_ARGS; i++) {
        cout << argNames[i] << " = " << defaultArgs[i] << endl;
    }
}

int main(int argc, char const *argv[]) {
    int args[NUM_ARGS];
    args[Ideas] = 80;
    args[IdeaGenerators] = 2;
    args[Packages] = 4000;
    args[PackageDownloaders] = 6;
    args[Students] = 6;

    if (argc - 1 > NUM_ARGS) {
        usage(argv, args);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        args[i - 1] = atoi(argv[i]);
    }

    // Force ctype to be initialized in the base thread before forking
    // https://stackoverflow.com/questions/41964067/is-operatorostream-obj-on-two-different-streams-thread-safe
    std::stringstream ss;
    ss << 1;

    runHackathon(args);
    return 0;
}
