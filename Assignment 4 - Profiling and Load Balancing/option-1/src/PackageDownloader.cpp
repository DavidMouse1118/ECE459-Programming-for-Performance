#include "PackageDownloader.h"
#include "utils.h"
#include <fstream>
#include <cassert>

PackageDownloader::PackageDownloader(EventQueue* eq, int packageStartIdx, int packageEndIdx)
    : ChecksumTracker()
    , eq(eq)
    , numPackages(packageEndIdx - packageStartIdx + 1)
    , packageStartIdx(packageStartIdx)
    , packageEndIdx(packageEndIdx)
{
    #ifdef DEBUG
    std::unique_lock<std::mutex> stdoutLock(eq->stdoutMutex);
    printf("PackageDownloader n:%d s:%d e:%d\n", numPackages, packageStartIdx, packageEndIdx);
    #endif
}

PackageDownloader::~PackageDownloader() {
    // nop
}

void PackageDownloader::run() {
    Container<std::string> packages = readFile("data/packages.txt");

    for (int i = packageStartIdx; i <= packageEndIdx; i++) {
        std::string packageName = packages[i % packages.size()];

        eq->enqueueEvent(Event(Event::DOWNLOAD_COMPLETE, new Package(packageName)));
        uint8_t* checksum = sha256(packageName);
        updateGlobalChecksum(checksum);
        delete [] checksum;
    }
}
