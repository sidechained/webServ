// WEBSERV.hpp
#pragma once
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Parser/ConfigFileParser.hpp"
#include "Servers/ServerManager.hpp"
#include "Servers/Server.hpp"
#include "Responses/SimpleResponse.hpp"
#include "Responses/TextResponse.hpp"

extern bool isRunning;

void loadMapFromFile(const std::string &filename, std::map<std::string, std::string> &map);
std::string findContentType(const std::string &resource);
std::string findStatusCode(std::string const &code);
void removeNonPrintableChars(std::string& input);
