// WEBSERV.hpp
#ifndef WEBSERV_HPP
#define WEBSERV_HPP
#include <string>
#include <map>

extern bool isRunning;

std::string findContentType(const std::string &resource);
void loadContentTypesFromFile(const std::string &filename, std::map<std::string, std::string> &_contentTypes);

#endif // WEBSERV_HPP
