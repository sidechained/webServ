#include "LocationParser.hpp"
#include "GenericParser.hpp"

LocationParser::LocationParser()
{

}

LocationParser::LocationParser(std::fstream &configFile, std::string &inLine) {
	line = inLine;
	setDefaults();
	extract(configFile);
}

LocationParser::LocationParser(const LocationParser &o)
{
	*this = o;
}

LocationParser& LocationParser::operator=(const LocationParser &o)
{
	locationConfig = o.locationConfig;
	return *this;
}

LocationParser::~LocationParser() {

}

void LocationParser::setDefaults()
{
	locationConfig.methods.push_back("GET");
	locationConfig.methods.push_back("POST");
	locationConfig.methods.push_back("DELETE");
	locationConfig.root = "";
	locationConfig.uploads = "";
	locationConfig.autoindex = false;
	locationConfig.index = "";	
}

void LocationParser::extract(std::fstream &configFile) {
	std::size_t locationColonPos;
	stripTabIndents(line);
	extractKey(locationConfig.key, locationColonPos);
	std::getline(configFile, line);
	bool firstLine = true;
	while(1)
	{
		// check for end of location entry
		if (countTabIndents(line) != 3)
		{
			if (firstLine == true)
				errorExit(ERR_PARSE, ERR_NO_VALUE);
			else
			{ // either next entry in locationConfigs or carriage return for end of server block
				skipNextLine = true;
				return;
			}
		}
		// matching condition:
		stripTabIndents(line);
		std::string locEntryKey;
		std::size_t locEntryColonPos;
		extractKey(locEntryKey, locEntryColonPos);
		std::string locEntryValue;
		if (locEntryKey == "methods") {
			extractValue(locEntryValue, locEntryColonPos);
			extractMethods(locEntryValue);
		} else if (locEntryKey == "redir") {
			extractValue(locEntryValue, locEntryColonPos);
			locationConfig.redirection = locEntryValue;
		} else if (locEntryKey == "root") {
			extractValue(locEntryValue, locEntryColonPos);
			locationConfig.root = locEntryValue;
		} else if (locEntryKey == "uploads") {
			extractValue(locEntryValue, locEntryColonPos);
			locationConfig.uploads = locEntryValue;	
		} else if (locEntryKey == "index") {
			extractValue(locEntryValue, locEntryColonPos);
			locationConfig.index = locEntryValue;											
		} else if (locEntryKey == "autoindex") {
			extractValue(locEntryValue, locEntryColonPos);
			if (locEntryValue == "true")
				locationConfig.autoindex = true;
		}
		firstLine = false;
		std::getline(configFile, line);
	}
}

void LocationParser::extractMethods(std::string methodString) {
	locationConfig.methods.clear(); // wipe out the methods that were put there by initialisation
	std::istringstream iss(methodString);
	std::string method;
	while (iss >> method) {
		locationConfig.methods.push_back(method);
	}
}

void LocationParser::print(LocationConfig locationConfig) {
	std::cout << "      Methods:";
	for (std::vector<std::string>::const_iterator it = locationConfig.methods.begin(); it != locationConfig.methods.end(); ++it) {
		std::cout << " \"" << *it << "\"";
	}
	std::cout << std::endl;
	std::cout << "      Redirection: \"" << locationConfig.redirection << "\"" << std::endl;
	std::cout << "      root: \"" << locationConfig.root << "\"" << std::endl;
	std::cout << "      uploads: \"" << locationConfig.uploads << "\"" << std::endl;
	std::cout << "      Autoindex: " << (locationConfig.autoindex ? "true" : "false") << std::endl;
	std::cout << "      Index: \"" << locationConfig.index << "\"" << std::endl;
}