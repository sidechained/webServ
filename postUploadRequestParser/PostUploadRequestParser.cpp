#include "PostUploadRequestParser.hpp"

// TODO:
// - correctly parse content disposition
// - handle unexpected amount of semicolons or elements when splitting Content-Type and Content-Disposition

PostUploadRequestParser::PostUploadRequestParser(std::string inputFilename, std::string outputFilename) : _outputFilename(outputFilename)
{
	initAcceptedFields();
	std::fstream requestFile(inputFilename.c_str());
	if(!requestFile.is_open()) {
		std::cout << "Could not open file" << std::endl;
		exit(EXIT_FAILURE);
	}	
	std::string line;
	getline(requestFile, line);	
	parsePostUploadRequest(line, postUploadRequest);
	printPostUploadRequest();
	parseHeaderBlock(requestFile, line, postUploadRequest.headers, headerAcceptedFields);
	parseContentTypeValue();
	parseBody(requestFile);
	requestFile.close();
	printPartHeaders();
	//
	if(postUploadRequest.method != "POST") {
		std::cout << "Request is not a post request" << std::endl;
		exit(EXIT_FAILURE);
	}
	if(postUploadRequest.resource != "/cgi/upload.php") { // hardcoded for now
		std::cout << "Resource requested does not match server-side resource" << std::endl;
		exit(EXIT_FAILURE);
	}		
	if(postUploadRequest.httpVersion != "HTTP/1.1") {
		std::cout << "HTTP version is not HTTP/1.1" << std::endl;
		exit(EXIT_FAILURE);
	}
	checkContentTypes();
	makeOutputFile();
}

PostUploadRequestParser::~PostUploadRequestParser()
{
	
}

void PostUploadRequestParser::initAcceptedFields()
{
	headerAcceptedFields.push_back("host");
	headerAcceptedFields.push_back("content-type");
	headerAcceptedFields.push_back("content-length");
	partsAcceptedFields.push_back("content-disposition");
	partsAcceptedFields.push_back("content-type");
}

void PostUploadRequestParser::parsePostUploadRequest(const std::string& line, PostUploadRequest& PostUploadRequest) {
	size_t firstSpace = line.find(' ');
	size_t lastSpace = line.rfind(' ');

	if (firstSpace != std::string::npos && lastSpace != std::string::npos && firstSpace < lastSpace) {
		PostUploadRequest.method = line.substr(0, firstSpace);
		PostUploadRequest.resource = line.substr(firstSpace + 1, lastSpace - firstSpace - 1);
		PostUploadRequest.httpVersion = line.substr(lastSpace + 1);

		if (!isValidMethod(PostUploadRequest.method) || !isValidResource(PostUploadRequest.resource) || !isValidHttpVersion(PostUploadRequest.httpVersion)) {
			std::cerr << "Error: Invalid HTTP request line." << std::endl;
			PostUploadRequest.method.clear();
			PostUploadRequest.resource.clear();
			PostUploadRequest.httpVersion.clear();
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

void PostUploadRequestParser::parseHeaderLine(const std::string& line, std::map<std::string, std::string> &headers, std::vector<std::string> acceptedFields) {
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
			headers[field] = value;
		}
	}
}

void PostUploadRequestParser::parseHeaderBlock(std::fstream &requestFile, std::string &line, std::map<std::string, std::string> &headers, std::vector<std::string> acceptedFields)
{
	while(!line.empty())
	{
		getline(requestFile, line);
		parseHeaderLine(line, headers, acceptedFields);
	}
}

void PostUploadRequestParser::parseHeaderBlockFromString(std::string& inputString, std::string& line, std::map<std::string, std::string> &headers, std::vector<std::string> acceptedFields)
{
	std::istringstream iss(inputString);  // Treat the input string as a stream
	// Read lines from the stringstream
	while (std::getline(iss, line) && !line.empty()) {
		parseHeaderLine(line, headers, acceptedFields);
		removeFromStringStart(inputString, line);
	}
}

// what if there are more than two elements given here?
void PostUploadRequestParser::parseContentTypeValue()
{
	std::string contentType;
	std::vector<std::string> tokens;

	// 1. Split the "Content-Type" header value by the semicolon (;)
	splitString(postUploadRequest.headers["content-type"], ";", tokens);
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
		postUploadRequest.headers["content-type"] = tokens[1];
	} 
	else if(startsWith(tokens[1], "boundary=")) {
		boundary = tokens[1];
		postUploadRequest.headers["content-type"] = tokens[0];
	}
	else {
		std::cout << "syntax error2!" << std::endl;
		exit(EXIT_FAILURE);
	}
	// 4. Extract the boundary string by splitting the part that starts with "boundary=" and retrieves the value after the equal sign (=).	
	removeFromStringStart(boundary, "boundary=");
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
	for(size_t i = 0; i < parts.size(); i++)
	{
		std::string line;
		Part part;
		postUploadRequest.parts.push_back(part);
		parseHeaderBlockFromString(parts[i], line, postUploadRequest.parts[i].headers, partsAcceptedFields);
		parseContentDisposition(postUploadRequest.parts[i].headers);
		postUploadRequest.parts[i].data = parts[i];
	}
}

void PostUploadRequestParser::parseContentDisposition(std::map<std::string, std::string> &partMap)
{
	std::string dispositionType;
	std::string name;
	std::string filename;
	std::vector<std::string> tokens;
	splitString(partMap["content-disposition"], ";", tokens);
	dispositionType = tokens[0];
	if (!(dispositionType == "form-data"))
	{
		std::cout << "syntax error423!" << std::endl;
		exit(EXIT_FAILURE);
	}
	// this will need to be adapted to handle more than 2 elements
	dispositionType = tokens[0];
	for(size_t i = 0; i < tokens.size(); i++)
	{
		trimWhitespace(tokens[i]);
	}
	if(startsWith(tokens[1], "name=")) {
		name = tokens[1];
		filename = tokens[2];
	} 
	else if(startsWith(tokens[2], "boundary=")) {
		name = tokens[2];
		filename = tokens[1];
	}
	else {
		std::cout << "syntax error2!" << std::endl;
		exit(EXIT_FAILURE);
	}
	// std::cout << dispositionType << std::endl;
	// std::cout << name << std::endl;
	// std::cout << filename << std::endl;		
}


bool PostUploadRequestParser::isContentOfType(std::map<std::string, std::string> headers, std::string typeToMatch)
{
	if (headers["content-type"] != typeToMatch) {
		return false;
	}
	return true;
}

// 5. Check if content type of all parts matches give type
bool PostUploadRequestParser::arePartContentsOfType(std::string typeToMatch)
{
	for(size_t i = 0; i < postUploadRequest.parts.size(); i++)
	{
		if (!isContentOfType(postUploadRequest.parts[i].headers, typeToMatch))
			return false;
	}
	return true;
}

// check file type are as expected in header and parts
void PostUploadRequestParser::checkContentTypes()
{
	if (!isContentOfType(postUploadRequest.headers, "multipart/form-data"))
	{
		std::cout << "syntax error3!" << std::endl;
		exit(EXIT_FAILURE);
	}	
	if (!arePartContentsOfType("text/plain"))
	{
		std::cout << "syntax error3!" << std::endl;
		exit(EXIT_FAILURE);
	}
}

// open a file, loop over the parts, write to data payload to the file, close the file
void PostUploadRequestParser::makeOutputFile()
{
	std::ofstream outputFile(_outputFilename.c_str());
	if (!outputFile.is_open()) {
		std::cerr << "Failed to open the file for writing." << std::endl;
	}	
	for(size_t i = 0; i < postUploadRequest.parts.size(); i++)
	{
		outputFile << postUploadRequest.parts[i].data;
	}
	outputFile.close();
	std::cout << "Output file written to: " << _outputFilename << std::endl;
}

// utility functions

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

void PostUploadRequestParser::printPostUploadRequest() {
	if (!postUploadRequest.method.empty()) {
		std::cout << "Method: " << postUploadRequest.method << std::endl;
		std::cout << "Resource: " << postUploadRequest.resource << std::endl;
		std::cout << "HTTP Version: " << postUploadRequest.httpVersion << std::endl;
	}
}	

void PostUploadRequestParser::printPartHeaders() {
	for(size_t i = 0; i < postUploadRequest.parts.size(); i++)
	{
		printHeaders(postUploadRequest.parts[i].headers);
	}
}

void PostUploadRequestParser::printHeaders(std::map<std::string, std::string> map) {
	std::map<std::string, std::string>::iterator it;
	for (it = map.begin(); it != map.end(); ++it) {
		std::cout << "Field: " << it->first << ", Value: " << it->second << std::endl;
	}
}