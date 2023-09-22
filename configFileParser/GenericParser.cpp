#include "GenericParser.hpp"

void GenericParser::extractKey(std::string &key, std::size_t &colonPos) {
	colonPos = line.find(':');
	if (colonPos == std::string::npos)
		errorExit(ERR_PARSE, ERR_PARSE_NOFIELD);
	key = line.substr(0, colonPos);
}

void GenericParser::extractValue(std::string &value, std::size_t &colonPos) {
	value = line.substr(colonPos + 1);
	if (value[0] != ' ')
		errorExit(ERR_PARSE, ERR_PARSE_SPACE);
	value.erase(0, 1); // strip the space
	if (value.empty())
		errorExit(ERR_PARSE, ERR_NO_VALUE);
}

void GenericParser::errorExit(std::string err1, std::string err2) {
	std::cerr << err1 << std::endl;
	if (!err2.empty())
		std::cerr << "[" << err2 << "]" << std::endl;
	exit(EXIT_FAILURE);
}

int GenericParser::countTabIndents(const std::string line) {
	int numTabs = 0;
	while(line[numTabs] == '\t')
		numTabs++;
	return(numTabs);
}

void GenericParser::stripTabIndents(std::string &line) {
	line.erase(0, countTabIndents(line));
}