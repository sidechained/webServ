#include "GenericParser.hpp"

void GenericParser::extractKey(std::string &key, std::size_t &colonPos) {
	colonPos = line.find(':');
	if (colonPos == std::string::npos)
		errorExit(ERR_PARSE, ERR_PARSE_NOFIELD);
	key = line.substr(0, colonPos);
	trimWhitespace(key);
}

void GenericParser::extractValue(std::string &value, std::size_t &colonPos) {
	value = line.substr(colonPos + 1);
	if (value[0] != ' ')
		errorExit(ERR_PARSE, ERR_PARSE_SPACE);
	value.erase(0, 1); // strip the space
	trimWhitespace(value);
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

void GenericParser::splitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& output) {
	std::string::size_type start = 0;
	std::string::size_type end = input.find(delimiter);

	while (end != std::string::npos) {
		// Only add non-empty substrings to the output
		if (start != end)
			output.push_back(input.substr(start, end - start));

		start = end + delimiter.length();  // Move start to the beginning of the next substring
		end = input.find(delimiter, start);
	}

	// Add the last token if it's non-empty
	if (start < input.length()) {
		std::string lastToken = input.substr(start);
		if (!lastToken.empty())
			output.push_back(lastToken);
	}
}

void GenericParser::trimWhitespace(std::string &str)
{
	// Trim leading whitespace
	std::string::size_type start = str.find_first_not_of(" \t\n\r");
	if (start != std::string::npos)
	{
		str.erase(0, start);
	}
	else
	{
		str.clear(); // String contains only whitespace
		return;
	}
	// Trim trailing whitespace
	std::string::size_type end = str.find_last_not_of(" \t\n\r");
	if (end != std::string::npos)
	{
		str.erase(end + 1);
	}
}
