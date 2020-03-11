#pragma once

#include "utils.h"
#include <string>
#include <mutex>

// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern

enum ChecksumType {
    IDEA,
    PACKAGE,
    NUM_CHECKSUMS
};

template <typename T, ChecksumType CT>
class ChecksumTracker
{
    static std::mutex checksumMutex[NUM_CHECKSUMS];
    static std::string globalChecksum[NUM_CHECKSUMS];

protected:
    static void updateGlobalChecksum(std::string checksum) {
        std::unique_lock<std::mutex> lock(checksumMutex[CT]);

        if (globalChecksum[CT].empty()) {
            globalChecksum[CT] = initChecksum();
        }

        globalChecksum[CT] = xorChecksum(globalChecksum[CT], checksum);
    }

public:
    ChecksumTracker() {
        // nop
    }

    ~ChecksumTracker() {
        // nop
    }

    static std::string getGlobalChecksum() {
        std::unique_lock<std::mutex> lock(checksumMutex[CT]);
        std::string checksum = globalChecksum[CT];
        return checksum;
    }
};

template <typename T, ChecksumType CT> std::mutex ChecksumTracker<T, CT>::checksumMutex[NUM_CHECKSUMS];
template <typename T, ChecksumType CT> std::string ChecksumTracker<T, CT>::globalChecksum[NUM_CHECKSUMS];
