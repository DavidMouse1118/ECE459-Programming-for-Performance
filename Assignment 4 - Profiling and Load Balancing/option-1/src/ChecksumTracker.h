#pragma once

#include "utils.h"
#include <string>
#include <mutex>
#include <openssl/sha.h>

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
    static uint8_t* globalChecksum[NUM_CHECKSUMS];

protected:
    static void updateGlobalChecksum(uint8_t* checksum) {
        std::unique_lock<std::mutex> lock(checksumMutex[CT]);

        if (globalChecksum[CT] == NULL) {
            globalChecksum[CT] = initChecksum();
        }

        xorChecksum(globalChecksum[CT], checksum);
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
        std::string result = bytesToString(globalChecksum[CT], SHA256_DIGEST_LENGTH);
        return result;
    }
};

template <typename T, ChecksumType CT> std::mutex ChecksumTracker<T, CT>::checksumMutex[NUM_CHECKSUMS];
template <typename T, ChecksumType CT> uint8_t* ChecksumTracker<T, CT>::globalChecksum[NUM_CHECKSUMS];
