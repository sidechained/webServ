#include "LocationConfig.hpp"
#include "GenericConfig.hpp"

LocationConfig::LocationConfig()
{

}

LocationConfig::LocationConfig(std::fstream &configFile, std::string &inLine) {
	line = inLine;
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
	redirection = "";
	root = "";
	uploads = "";
	autoindex = false;
	index = "index.html";
	extract(configFile);
}

LocationConfig::LocationConfig(const LocationConfig &o)
{
	*this = o;
}

LocationConfig& LocationConfig::operator=(const LocationConfig &o)
{
	methods = o.methods;
	redirection = o.redirection;
	root = o.root;
	uploads = o.uploads;
	autoindex = o.autoindex;
	index = o.index;
	return *this;
}

LocationConfig::~LocationConfig() {

}

void LocationConfig::extract(std::fstream &configFile) {
	std::size_t locationColonPos;
	stripTabIndents(line);
	extractKey(key, locationColonPos);
	std::getline(configFile, line);
	bool firstLine = true;
	while(1)
	{
		// check for end of location entry
		if (countTabIndents(line) != 3)
		{
			if (firstLine == true)
				errorExit(ERR_PARSE, ERR_NO_VALUES);
			else
			{ // either next entry in locations or carriage return for end of server block
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
			redirection = locEntryValue;
		} else if (locEntryKey == "root") {
			extractValue(locEntryValue, locEntryColonPos);
			root = locEntryValue;
		} else if (locEntryKey == "uploads") {
			extractValue(locEntryValue, locEntryColonPos);
			uploads = locEntryValue;	
		} else if (locEntryKey == "index") {
			extractValue(locEntryValue, locEntryColonPos);
			index = locEntryValue;											
		} else if (locEntryKey == "autoindex") {
			extractValue(locEntryValue, locEntryColonPos);
			if (locEntryValue == "true")
				autoindex = true;
		}
		firstLine = false;
		std::getline(configFile, line);
	}
}

void LocationConfig::extractMethods(std::string methodString) {
	methods.clear(); // wipe out the methods that were put there by initialisation
	std::istringstream iss(methodString);
	std::string method;
	while (iss >> method) {
		methods.push_back(method);
	}
}

void LocationConfig::print() const {
	std::cout << "      Methods:";
	for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
		std::cout << " \"" << *it << "\"";
	}
	std::cout << std::endl;
	std::cout << "      Redirection: \"" << redirection << "\"" << std::endl;
	std::cout << "      Root: \"" << root << "\"" << std::endl;
	std::cout << "      uploads: \"" << uploads << "\"" << std::endl;
	std::cout << "      Autoindex: " << (autoindex ? "true" : "false") << std::endl;
	std::cout << "      Index: \"" << index << "\"" << std::endl;
}

void LocationConfig::cleanUp() {
	//go through all strings and maps and call removeNonPrintableChars
	removeNonPrintableChars(key);
	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); ++it) {
		removeNonPrintableChars(*it);
	}
	removeNonPrintableChars(redirection);
	removeNonPrintableChars(root);
	removeNonPrintableChars(uploads);
	removeNonPrintableChars(index);


}