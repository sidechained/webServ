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
	redirection.oldURL = "";
	redirection.newURL = "";
	alias = "";
	uploads = "";
	autoindex = false;
	index = "";
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
	alias = o.alias;
	uploads = o.uploads;
	autoindex = o.autoindex;
	index = o.index;
	return *this;
}

LocationConfig::~LocationConfig() {

}

void LocationConfig::extract(std::fstream &configFile) {
	std::size_t locationColonPos;
	detectAndStripTabIndents(2);
	extractKey(key, locationColonPos);
	std::getline(configFile, line);
	while(countTabIndents(3))
	{
		detectAndStripTabIndents(3);
		std::string locEntryKey;
		std::size_t locEntryColonPos;
		extractKey(locEntryKey, locEntryColonPos);
		std::string locEntryValue;
		if (locEntryKey == "methods") {
			extractValue(locEntryValue, locEntryColonPos);
			extractMethods(locEntryValue);
		} else if (locEntryKey == "redir") {
			extractValue(locEntryValue, locEntryColonPos);
			extractRedir(locEntryValue);
		} else if (locEntryKey == "alias") {
			extractValue(locEntryValue, locEntryColonPos);
			alias = locEntryValue;
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

void LocationConfig::extractRedir(std::string redirString) {
	std::istringstream iss(redirString);
	std::string redir;
	int elementCount = 0;
	while (iss >> redir) {
		if (elementCount == 0)
			redirection.oldURL = redir;
		if (elementCount == 1)
			redirection.newURL = redir;
		elementCount++;
		if (elementCount > 2)
			errorExit(ERR_PARSE, ERR_PARSE_REDIR);
	}
}

void LocationConfig::print() const {
	std::cout << "      Methods:";
	for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
		std::cout << " \"" << *it << "\"";
	}
	std::cout << std::endl;
	std::cout << "      Redirection: oldURL = \"" << redirection.oldURL << "\", newURL = \"" << redirection.newURL << "\"" << std::endl;
	std::cout << "      Alias: \"" << alias << "\"" << std::endl;
	std::cout << "      uploads: \"" << uploads << "\"" << std::endl;
	std::cout << "      Autoindex: " << (autoindex ? "true" : "false") << std::endl;
	std::cout << "      Index: \"" << index << "\"" << std::endl;
}