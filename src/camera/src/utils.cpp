#include <util.h>
#include <algorithm>
#include <arpa/inet.h>


std::string
str_toLower(const std::string& str) {
	auto s(str);
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); }
                  );
    return s;
}

std::string
str_toUpper(const std::string& str) {
	auto s(str);
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::toupper(c); }
                  );
    return s;
}

uint64_t htonll(uint64_t value) {
    // Если поддерживается прямая функция htonll, используем ее
    #ifdef __APPLE__
        return htonll(value);
    #else
        return (((uint64_t)htonl(value & 0xFFFFFFFF)) << 32) | htonl(value >> 32);
    #endif
}

uint64_t ntohll(uint64_t value) {
    // Если поддерживается прямая функция ntohll, используем ее
    #ifdef __APPLE__
        return ntohll(value);
    #else
        return (((uint64_t)ntohl(value & 0xFFFFFFFF)) << 32) | ntohl(value >> 32);
    #endif
}
