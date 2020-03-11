#pragma once

#include "ChecksumTracker.h"
#include "EventQueue.h"
#include <string>

class IdeaGenerator : public ChecksumTracker<IdeaGenerator, ChecksumType::IDEA>
{
    EventQueue* eq;
    const int numIdeas;
    const int ideaStartIdx;
    const int ideaEndIdx;
    const int numPackages;
    const int numStudents;

    std::string getNextIdea(int i);

public:
    IdeaGenerator(EventQueue* eq, int ideaStartIdx, int ideaEndIdx, int numPackages, int numStudents);
    ~IdeaGenerator();
    void run();
};
