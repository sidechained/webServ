#pragma once
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#define REQUEST_BUFFER 90000
#define MESSAGE_BUFFER 20000
#define CONNECTION_TIMEOUT 500

extern bool isRunning;

void loadMapFromFile(const std::string &filename, std::map<std::string, std::string> &map);
std::string findContentType(const std::string &resource);
std::string findStatusCode(std::string const &code);
void removeNonPrintableChars(std::string& input);
