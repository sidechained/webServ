#include "PostUploadRequestParser.hpp"

PostUploadRequestParser::PostUploadRequestParser()
{
	std::fstream requestFile("postRequest.txt");
	if(!requestFile.is_open()) {
		std::cout << "Could not open file" << std::endl;
		exit(EXIT_FAILURE);
	}	
	std::string line;
	getline(requestFile, line);	
	HttpRequest httpRequest;
	parseHttpRequest(line, httpRequest);
	if (!httpRequest.method.empty()) {
		std::cout << "Method: " << httpRequest.method << std::endl;
		std::cout << "Resource: " << httpRequest.resource << std::endl;
		std::cout << "HTTP Version: " << httpRequest.httpVersion << std::endl;
	}
	parseHeaderBlock(requestFile, line);
	requestFile.close();
}


PostUploadRequestParser::~PostUploadRequestParser()
{
	
}

// Check if the method consists of uppercase and lowercase letters only
bool PostUploadRequestParser::isValidMethod(const std::string& method) {
	return std::string::npos == method.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

// Check if the resource contains valid characters (letters, digits, '/', '.', '-', '_', '~')
bool PostUploadRequestParser::isValidResource(const std::string& resource) {
	return std::string::npos == resource.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/._-~");
}

// Check if the HTTP version follows the format HTTP/x.y
bool PostUploadRequestParser::isValidHttpVersion(const std::string& httpVersion) {
	return (httpVersion.length() >= 8 && httpVersion.substr(0, 5) == "HTTP/" && httpVersion[5] == '1' && httpVersion[6] == '.' && httpVersion[7] >= '0' && httpVersion[7] <= '9');
}

void PostUploadRequestParser::parseHttpRequest(const std::string& line, HttpRequest& httpRequest) {
	size_t firstSpace = line.find(' ');
	size_t lastSpace = line.rfind(' ');

	if (firstSpace != std::string::npos && lastSpace != std::string::npos && firstSpace < lastSpace) {
		httpRequest.method = line.substr(0, firstSpace);
		httpRequest.resource = line.substr(firstSpace + 1, lastSpace - firstSpace - 1);
		httpRequest.httpVersion = line.substr(lastSpace + 1);

		if (!isValidMethod(httpRequest.method) || !isValidResource(httpRequest.resource) || !isValidHttpVersion(httpRequest.httpVersion)) {
			std::cerr << "Error: Invalid HTTP request line." << std::endl;
			httpRequest.method.clear();
			httpRequest.resource.clear();
			httpRequest.httpVersion.clear();
		}
	} else {
		std::cerr << "Error: Invalid HTTP request line." << std::endl;
	}
}

bool PostUploadRequestParser::isAcceptedField(std::string field) {
	std::vector<std::string> acceptedFields;
	acceptedFields.push_back("Host");
	acceptedFields.push_back("Content-Type");
	acceptedFields.push_back("Content-Length");
	if (std::find(acceptedFields.begin(), acceptedFields.end(), field) != acceptedFields.end())
		return true;
	return false;
}

void PostUploadRequestParser::parseHeaderLine(const std::string& line, std::map<std::string, std::string>& headerMap) {
	std::string::size_type colonPos = line.find(':');
	if (colonPos != std::string::npos) {
		std::string field = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);
		if(isAcceptedField(field))
		{
			// Remove leading whitespaces from the value
			std::string::size_type valueStart = value.find_first_not_of(" \t");
			if (valueStart != std::string::npos)
				value = value.substr(valueStart);
			
			// Store the field and value in the map
			headerMap[field] = value;
		}
	}
}

void PostUploadRequestParser::parseHeaderBlock(std::fstream &requestFile, std::string &line)
{
	std::map<std::string, std::string> headerMap;
	while(!line.empty())
	{
		getline(requestFile, line);
		parseHeaderLine(line, headerMap);
	}
	printHeaderMap(headerMap);
}

void PostUploadRequestParser::printHeaderMap(std::map<std::string, std::string> headerMap) {
	std::map<std::string, std::string>::iterator it;
	for (it = headerMap.begin(); it != headerMap.end(); ++it) {
		std::cout << "Field: " << it->first << ", Value: " << it->second << std::endl;
	}
}

