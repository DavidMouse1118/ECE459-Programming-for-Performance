#include "IdeaGenerator.h"
#include "Container.h"
#include "utils.h"
#include <cassert>

IdeaGenerator::IdeaGenerator(EventQueue* eq, int ideaStartIdx, int ideaEndIdx, int numPackages, int numStudents)
    : ChecksumTracker()
    , eq(eq)
    , numIdeas(ideaEndIdx - ideaStartIdx + 1)
    , ideaStartIdx(ideaStartIdx)
    , ideaEndIdx(ideaEndIdx)
    , numPackages(numPackages)
    , numStudents(numStudents)
{
    #ifdef DEBUG
    std::unique_lock<std::mutex> stdoutLock(eq->stdoutMutex);
    printf("IdeaGenerator n:%d s:%d e:%d p:%d s:%d\n", numIdeas, ideaStartIdx, ideaEndIdx, numPackages, numStudents);
    #endif
}

IdeaGenerator::~IdeaGenerator() {
    // nop
}

struct StrPair
{
    std::string a;
    std::string b;

    StrPair() { }

    StrPair(std::string a, std::string b) : a(a), b(b) { }

    StrPair(const StrPair &other) {
        a = other.a;
        b = other.b;
    }

    StrPair& operator=(const StrPair &other) {
        if (this != &other) {
            a = other.a;
            b = other.b;
        }

        return *this;
    }
};

Container<StrPair> crossProduct(Container<std::string> a, Container<std::string> b) {
    Container<StrPair> result;

    for (int i = 0; i < a.size(); i++) {
        for (int j = 0; j < b.size(); j++) {
            result.push(StrPair(a[i], b[j]));
        }
    }

    return result;
}

std::string IdeaGenerator::getNextIdea(int i) {
    Container<std::string> products = readFile("data/ideas-products.txt");
    Container<std::string> customers = readFile("data/ideas-customers.txt");
    Container<StrPair> ideas = crossProduct(products, customers);

    assert(ideas.size() > 0);
    auto ideaPair = ideas[i % ideas.size()];

    return ideaPair.a + " for " + ideaPair.b;
}

void IdeaGenerator::run() {
    int packagesPerIdea = numPackages / numIdeas;
    int ideasWithExtraPackage = numPackages % numIdeas;

    for (int i = ideaStartIdx; i <= ideaEndIdx; i++) {
        std::string ideaName = getNextIdea(i);
        int packagesReq = packagesPerIdea + (((i - ideaStartIdx) < ideasWithExtraPackage) ? 1 : 0);

        eq->enqueueEvent(Event(Event::NEW_IDEA, new Idea(ideaName, packagesReq)));
        updateGlobalChecksum(sha256(ideaName));
    }

    for (int i = 0; i < numStudents; i++) {
        eq->enqueueEvent(Event(Event::OUT_OF_IDEAS, nullptr));
    }
}
