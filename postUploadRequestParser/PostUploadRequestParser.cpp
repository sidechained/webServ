#include "PostUploadRequestParser.hpp"

PostUploadRequestParser::PostUploadRequestParser()
{
	// init accepted field for header
	headerAcceptedFields.push_back("host");
	headerAcceptedFields.push_back("content-type");
	headerAcceptedFields.push_back("content-length");
	// init accepted fields for parts (multipart)
	partsAcceptedFields.push_back("content-disposition");
	partsAcceptedFields.push_back("content-type");
	// 
	std::fstream requestFile("postRequest.txt");
	if(!requestFile.is_open()) {
		std::cout << "Could not open file" << std::endl;
		exit(EXIT_FAILURE);
	}	
	std::string line;
	getline(requestFile, line);	
	HttpRequest httpRequest;
	parseHttpRequest(line, httpRequest);
	// printHttpRequest(httpRequest);
	parseHeaderBlock(requestFile, line, headerMap, headerAcceptedFields);
	// printMap();
	parseContentTypeValue();
	parseBody(requestFile);
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

bool PostUploadRequestParser::isAcceptedField(std::string field, std::vector<std::string> acceptedFields) {
	if (std::find(acceptedFields.begin(), acceptedFields.end(), field) != acceptedFields.end())
		return true;
	return false;
}

void PostUploadRequestParser::parseHeaderLine(const std::string& line, std::map<std::string, std::string> &headerMap, std::vector<std::string> acceptedFields) {
	std::string::size_type colonPos = line.find(':');
	if (colonPos != std::string::npos) {
		std::string field = line.substr(0, colonPos);
		toLowerCase(field); // headers are case insensitive, so convert to and deal with in lower case only
		std::string value = line.substr(colonPos + 1);
		if(isAcceptedField(field, acceptedFields))
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

void PostUploadRequestParser::parseHeaderBlock(std::fstream &requestFile, std::string &line, std::map<std::string, std::string> &headerMap, std::vector<std::string> acceptedFields)
{
	while(!line.empty())
	{
		getline(requestFile, line);
		parseHeaderLine(line, headerMap, acceptedFields);
	}
}

void PostUploadRequestParser::parseHeaderBlockFromString(std::string& inputString, std::string& line, std::map<std::string, std::string>& headerMap, std::vector<std::string> acceptedFields)
{
    std::istringstream iss(inputString);  // Treat the input string as a stream
    // Read lines from the stringstream
    while (std::getline(iss, line) && !line.empty()) {
        parseHeaderLine(line, headerMap, acceptedFields);
		removeFromStringStart(inputString, line);
    }
}

void PostUploadRequestParser::parseContentTypeValue()
{
	std::string contentType;
	std::vector<std::string> tokens;

	// 1. Split the "Content-Type" header value by the semicolon (;)
	splitString(headerMap["content-type"], ";", tokens);
	if (tokens.size() != 2)
	{
		std::cout << "syntax error1!" << std::endl;
		exit(EXIT_FAILURE);
	}
	// 2. Trim any leading or trailing whitespace from each part.
	for(size_t i = 0; i < tokens.size(); i++)
	{
		trimWhitespace(tokens[i]);
	}
	// 3. Check each part to find the one that starts with "boundary=". This part contains the boundary string.
	if(startsWith(tokens[0], "boundary=")) {
		boundary = tokens[0];
		contentType = tokens[1];
	} 
	else if(startsWith(tokens[1], "boundary=")) {
		boundary = tokens[1];
		contentType = tokens[0];
	}
	else {
		std::cout << "syntax error2!" << std::endl;
		exit(EXIT_FAILURE);
	}
	// 4. Extract the boundary string by splitting the part that starts with "boundary=" and retrieves the value after the equal sign (=).	
	removeFromStringStart(boundary, "boundary=");
	// 5. Check if content type matches "multipart/form-data"
	if (contentType != "multipart/form-data") {
		std::cout << "syntax error3!" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void PostUploadRequestParser::parseBody(std::fstream &requestFile)
{
	std::string endBoundary = "--" + boundary + "--";
	// look for end Boundary and erase it and everything after, treating this as our body
	std::string body;
	body.assign(std::istreambuf_iterator<char>(requestFile), std::istreambuf_iterator<char>());
	size_t foundPos = body.find(endBoundary);
	if (!(foundPos != std::string::npos)) {
		std::cout << "End boundary not found.\n";
		exit(EXIT_FAILURE);
	}
	body.erase(foundPos);
	// split the new body by the boundary string
	std::string splitBoundary = "--" + boundary;
	boundary = "--" + boundary + '\n';
	std::vector<std::string> parts;
	splitString(body, boundary, parts);
	std::map<std::string, std::string> partMap;
	for(size_t i = 0; i < parts.size(); i++)
	{
		std::string line;
		parseHeaderBlockFromString(parts[i], line, partMap, partsAcceptedFields);
		std::cout << parts[i] << std::endl;
		printMap(partMap);
	}
}

// utility functions

void PostUploadRequestParser::toLowerCase(std::string& str) {
	for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
		*it = std::tolower(*it);
	}
}

void PostUploadRequestParser::splitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& output) {
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

void PostUploadRequestParser::trimWhitespace(std::string& str) {
	// Trim leading whitespace
	std::string::size_type start = str.find_first_not_of(" \t\n\r");
	if (start != std::string::npos) {
		str.erase(0, start);
	} else {
		str.clear();  // String contains only whitespace
		return;
	}
	// Trim trailing whitespace
	std::string::size_type end = str.find_last_not_of(" \t\n\r");
	if (end != std::string::npos) {
		str.erase(end + 1);
	}
}

bool PostUploadRequestParser::startsWith(const std::string& fullString, const std::string& start) {
	if (fullString.length() < start.length())
		return false;
	for (std::string::size_type i = 0; i < start.length(); ++i) {
		if (fullString[i] != start[i])
			return false;
	}
	return true;
}

void PostUploadRequestParser::removeFromStringStart(std::string& fullString, const std::string& line) {
    std::string::size_type foundPos = fullString.find(line);

    if (foundPos == 0) {
        // Line found at the start, erase it
        fullString.erase(0, line.length());

        // Additionally, remove any leading whitespace or newline
        while (!fullString.empty() && (fullString[0] == ' ' || fullString[0] == '\t' || fullString[0] == '\n' || fullString[0] == '\r'))
            fullString.erase(0, 1);
    }
}

void PostUploadRequestParser::printHttpRequest(HttpRequest& httpRequest) {
	if (!httpRequest.method.empty()) {
		std::cout << "Method: " << httpRequest.method << std::endl;
		std::cout << "Resource: " << httpRequest.resource << std::endl;
		std::cout << "HTTP Version: " << httpRequest.httpVersion << std::endl;
	}
}	

void PostUploadRequestParser::printMap(std::map<std::string, std::string> map) {
	std::map<std::string, std::string>::iterator it;
	for (it = map.begin(); it != map.end(); ++it) {
		std::cout << "Field: " << it->first << ", Value: " << it->second << std::endl;
	}
}