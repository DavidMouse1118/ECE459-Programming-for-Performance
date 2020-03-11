#pragma once

#include "Container.h"
#include <string>

Container<std::string> readFile(std::string fileName);

std::string readFileLine(std::string fileName, int targetLineNum);

std::string bytesToString(uint8_t* bytes, int len);

std::string sha256(const std::string str);

std::string initChecksum();

std::string xorChecksum(std::string baseLayer, std::string newLayer);
