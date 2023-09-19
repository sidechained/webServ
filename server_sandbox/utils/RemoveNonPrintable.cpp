#include "../WebServ.hpp"

void removeNonPrintableChars(std::string& input) {
    std::string result;
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        if (c >= 32 && c <= 126) {
            result += c;
        }
    }
    input = result;
}