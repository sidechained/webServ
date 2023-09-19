// WEBSERV.hpp
#ifndef WEBSERV_HPP
#define WEBSERV_HPP
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

extern bool isRunning;

void loadMapFromFile(const std::string &filename, std::map<std::string, std::string> &map);
std::string findContentType(const std::string &resource);
std::string findStatusCode(std::string const &code);


#endif // WEBSERV_HPP
