#pragma once

#include "ChecksumTracker.h"
#include "EventQueue.h"
#include "Container.h"
#include <string>

class Student : public ChecksumTracker<Student, ChecksumType::PACKAGE>, public ChecksumTracker<Student, ChecksumType::IDEA>
{

    EventQueue* eq;
    const int id;

    Idea* currentIdea = nullptr;
    Container<Package*> currentPackages;

    uint8_t* getIdeaChecksum();
    uint8_t* getPackagesChecksum();
    void buildIdea();

public:
    Student(EventQueue* eq, int id);
    ~Student();
    void run();
};
