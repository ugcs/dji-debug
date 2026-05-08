#pragma once
#include <string>
#include <vector>

//Alias for single-byte sequences
typedef std::vector<uint8_t> byte_array_t;

std::string
str_toLower(const std::string& s);

std::string
str_toUpper(const std::string& s);

uint64_t htonll(uint64_t value);

uint64_t ntohll(uint64_t value);
