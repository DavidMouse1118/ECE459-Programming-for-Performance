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

    return ss.str();
}

std::string sha256(const std::string str) {
    uint8_t* hash = new uint8_t[SHA256_DIGEST_LENGTH];

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);

    std::string s = bytesToString(hash, SHA256_DIGEST_LENGTH);
    delete [] hash;
    return s;
}

std::string initChecksum() {
    std::string checksum;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        checksum += "00";
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

std::string xorChecksum(std::string baseLayer, std::string newLayer) {
    std::string checksum;

    assert(newLayer.size() == baseLayer.size());

    for (int i = 0; i < newLayer.size(); i += 2) {
        uint8_t* byte = new uint8_t[1];

        std::string a = std::string(1, baseLayer[i]) + std::string(1, baseLayer[i + 1]);
        std::string b = std::string(1, newLayer [i]) + std::string(1, newLayer [i + 1]);

        byte[0] = hexStrToByte(a) ^ hexStrToByte(b);
        checksum += bytesToString(byte, 1);

        delete [] byte;
    }

    return checksum;
}
