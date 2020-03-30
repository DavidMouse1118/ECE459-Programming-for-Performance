#pragma once

#include "ChecksumTracker.h"
#include "EventQueue.h"

class PackageDownloader : public ChecksumTracker<PackageDownloader, ChecksumType::PACKAGE>
{
    EventQueue* eq;
    const int numPackages;
    const int packageStartIdx;
    const int packageEndIdx;

public:
    PackageDownloader(EventQueue* eq, int packageStartIdx, int packageEndIdx);
    ~PackageDownloader();
    void run();
};
