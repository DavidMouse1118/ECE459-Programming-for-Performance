#pragma once

#include "Container.h"
#include <string>

Container<std::string> readFile(std::string fileName);

std::string readFileLine(std::string fileName, int targetLineNum);

std::string bytesToString(uint8_t* bytes, int len);

uint8_t* sha256(const std::string str);

uint8_t* initChecksum();

void xorChecksum(uint8_t* baseLayer, uint8_t* newLayer);