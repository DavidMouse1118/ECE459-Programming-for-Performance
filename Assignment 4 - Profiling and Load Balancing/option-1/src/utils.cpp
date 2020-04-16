#include "utils.h"
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>

Container<std::string> readFile(std::string fileName) {
    std::ifstream file(fileName);
    std::string line;
    Container<std::string> result;

    while (std::getline(file, line)) {
        if (!line.empty()) {
            result.push(line);
        }
    }

    return result;
}

std::string readFileLine(std::string fileName, int targetLineNum) {
    int counter = -1;
    std::string line;

    while (counter < targetLineNum) {
        std::ifstream file(fileName);

        while (counter < targetLineNum && std::getline(file, line)) {
            if (!line.empty()) {
                counter++;
            }
        }
    }

    return line;
}

std::string bytesToString(uint8_t* bytes, int len) {
    std::stringstream ss;

    for(int i = 0; i < len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)bytes[i];
    }

    delete [] bytes;
    return ss.str();
}

uint8_t* sha256(const std::string str) {
    uint8_t* hash = new uint8_t[SHA256_DIGEST_LENGTH];

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);

    return hash;
}

uint8_t* initChecksum() {
    uint8_t* checksum = new uint8_t[SHA256_DIGEST_LENGTH];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        checksum[i] = 0;
    }
    return checksum;
}

uint8_t hexStrToByte(std::string s) {
    assert(s.size() == 2);

    unsigned int x;
    std::stringstream ss;
    ss << std::hex << s;
    ss >> x;

    assert(x >= 0x00 && x <= 0xFF);

    return x;
}

void xorChecksum(uint8_t* baseLayer, uint8_t* newLayer) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i ++) {
        baseLayer[i] = baseLayer[i] ^ newLayer[i];
    }
}