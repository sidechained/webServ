#include "GenericConfig.hpp"

void GenericConfig::extractKey(std::string &key, std::size_t &colonPos) {
	colonPos = _line.find(':');
	if (colonPos == std::string::npos)
		errorExit(ERR_PARSE, ERR_PARSE_NOKEY);
	key = _line.substr(0, colonPos);
}

void GenericConfig::extractValue(std::string &value, std::size_t &colonPos) {
	value = _line.substr(colonPos + 1);
	if (value.empty() || value[0] != ' ')
		errorExit(ERR_PARSE, ERR_PARSE_SPACE);
	value.erase(0, 1); // strip the space
}

void GenericConfig::errorExit(std::string err1, std::string err2) {
	std::cerr << err1 << std::endl;
	if (!err2.empty())
		std::cerr << "[" << err2 << "]" << std::endl;
	exit(EXIT_FAILURE);
}

bool GenericConfig::countTabIndents(int numTabs) {
	if (_line.empty())
		return false;
	for (int i = 0; i < numTabs; i++)
	{
		if (_line[i] != '\t')
			return false;
	}
	return true;
}

void GenericConfig::detectAndStripTabIndents(int numTabs) {
	if (line.empty())
		errorExit(ERR_PARSE, ERR_PARSE_EMPTY);
	for (int i = 0; i < numTabs; i++)
	{
		if (line[i] != '\t')
			errorExit(ERR_PARSE, ERR_PARSE_INDENT);
	}
	line.erase(0, numTabs);
}

// void GenericConfig::detectAndStripTabIndents(int numTabs) {
// 	std::cout << _line << std::endl;
// 	if (_line.empty())
// 		errorExit(ERR_PARSE, ERR_PARSE_EMPTY);
// 	for (int i = 0; i < numTabs; i++)
// 	{
// 		if (_line[i] != '\t')
// 			errorExit(ERR_PARSE, ERR_PARSE_INDENT);
// 	}
// 	_line.erase(0, numTabs);
// }