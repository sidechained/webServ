#include "HttpRequest.hpp"

// TODO
// - exit in Luca-style :)
// - consider adding extra first-line/header validations from G's code
// - separate out extra processing of configs/location to later stage?

HttpRequest::HttpRequest()
{
}

HttpRequest::~HttpRequest()
{
}

HttpRequest::HttpRequest(std::string const &request, ServerConfig *config) : _config(config)
{
    this->clearErrors();
    _errorPages = _config->error_pages;
    fillIncomingRequestMap(request);
    parseLocationConfig();
    parseIndex();
    parseMethod();
    parseContentType();
    parseContentTypeValue();
    parseVectorParts();
	printPartHeaders();
    cleanUpMap(_incomingRequest);
}

void HttpRequest::fillIncomingRequestMap(std::string const &request)
{
    std::istringstream requestStream(request);
    std::string line;
    std::getline(requestStream, line);
    std::string::size_type pos = line.find(" ");
    if (pos != std::string::npos)
    {
        std::string method = line.substr(0, pos);
        _incomingRequest["Method"] = method;
        std::string::size_type pos2 = line.find(" ", pos + 1);
        if (pos2 != std::string::npos)
        {
            std::string resource = line.substr(pos + 1, pos2 - pos - 1);
            _incomingRequest["Resource"] = resource;
            std::string protocol = line.substr(pos2 + 1);
            _incomingRequest["Protocol"] = protocol;
        }
    }
    while (std::getline(requestStream, line))
    {
        if (line == "\r")
            break;
        std::string::size_type pos = line.find(": ");
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            _incomingRequest[key] = value;
        }
    }
    _body.assign(std::istreambuf_iterator<char>(requestStream), std::istreambuf_iterator<char>());
    _incomingRequest["Redirection"] = "";
}

void HttpRequest::cleanUpMap(std::map<std::string, std::string> _map)
{
    for (std::map<std::string, std::string>::iterator it = _map.begin(); it != _map.end(); ++it)
        removeNonPrintableChars(it->second);
}

void HttpRequest::parseLocationConfig()
{
    _path = _incomingRequest["Resource"];
    if (!isDirectory(_path) && !hasFileExtension(_path))
    {
        this->addError("noSlash");
        return;
    }
    _locationConfig = NULL;
    long i = _path.size();
    while (i >= 0)
    {
        std::string key = _path.substr(0, i);
        if (key == "")
            key = "/";
        if (locationIsSet(key))
        {
            _locationConfig = &_config->locationConfigs[key];
            PRINT(HTTPREQUEST, BG_BLUE, "location is set");
            parsePath(key, i);
            break;
        }
        i = key.find_last_of("/");
    }
}

void HttpRequest::parsePath(std::string &key, long i)
{
    if (_locationConfig->root != "")
    {
        if (key == "/")
            _path = _locationConfig->root + _path;
        else
            _path = _locationConfig->root + _path.substr(i);
    }
    if (_path[0] == '/')
        _path = _path.substr(1);
    PRINT(HTTPREQUEST, BG_BLUE, "path: " << _path)
}

void HttpRequest::parseIndex()
{
    if (isDirectory(_path))
    {
        if (_locationConfig && _locationConfig->index != "")
            _path = _path + _locationConfig->index;
    }
    PRINT(HTTPREQUEST, BG_BLUE, "path: " << _path)
}

void HttpRequest::parseMethod()
{
    std::string requestMethod = _incomingRequest["Method"];
    std::cout << "Parsing method: " << requestMethod << std::endl;

    if (requestMethod == "GET" || requestMethod == "POST" || requestMethod == "DELETE")
    {
        if (!_locationConfig)
            return;
        std::vector<std::string> locationMethods;
        locationMethods = _locationConfig->methods;
        if (std::find(locationMethods.begin(), locationMethods.end(), requestMethod) == locationMethods.end())
            this->addError("methodNotAllowed");
    }
    else
        this->addError("methodNotAllowed");
}

void HttpRequest::parseContentType()
{
    _contentType = findContentType(_path);
}

void HttpRequest::parseHeaderLine(const std::string& line, std::map<std::string, std::string> &headers) {
	std::string::size_type colonPos = line.find(':');
	if (colonPos != std::string::npos) {
		std::string field = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);
		// Remove leading whitespaces from the value
		std::string::size_type valueStart = value.find_first_not_of(" \t");
		if (valueStart != std::string::npos)
			value = value.substr(valueStart);
		// Store the field and value in the map
		headers[field] = value;
	}
}

void HttpRequest::parseHeaderBlockFromString(std::string& inputString, std::string& line, std::map<std::string, std::string> &headers)
{
	std::istringstream iss(inputString);  // Treat the input string as a stream
	// Read lines from the stringstream
	while (std::getline(iss, line) && !line.empty()) {
		parseHeaderLine(line, headers);
		removeFromStringStart(inputString, line);
	}
}

// what if there are more than two elements given here?
void HttpRequest::parseContentTypeValue()
{
	std::vector<std::string> tokens;
	// 1. Split the "Content-Type" header value by the semicolon (;)
	splitString(_incomingRequest["Content-Type"], ";", tokens);
	if (tokens.empty())
		return;
	_incomingRequest["Content-Type"] = tokens[0];
	for(size_t i = 1; i < tokens.size(); i++)
	{
		// 2. Trim any leading or trailing whitespace from each part.
		trimWhitespace(tokens[i]);
		// 3. Check each part to find the one that starts with "boundary=". This part contains the boundary string.
		if(startsWith(tokens[i], "boundary=")) {
			removeFromStringStart(tokens[i], "boundary=");
			// 4. Extract the boundary string by splitting the part that starts with "boundary=" and retrieves the value after the equal sign (=).	
			_boundary = tokens[i];
		}
	}
}

void HttpRequest::parseVectorParts()
{
    if (getMethod() == "POST" && _incomingRequest["Content-Type"] == "multipart/form-data")
	{
        parseBody();
		validate();
	}
}

void HttpRequest::parseBody()
{
    std::string body = _body;
	std::string endBoundary = "--" + _boundary + "--";
	// look for end Boundary and erase it and everything after, treating this as our body
	size_t foundPos = body.find(endBoundary);
	/*if (!(foundPos != std::string::npos)) {
		std::cout << "End boundary not found.\n";
		exit(EXIT_FAILURE);
	}*/
	if (foundPos == std::string::npos) {
        std::cout << "End boundary not found. Manually adding it.\n";
        
        // You can insert the endBoundary at a specific position in the string
        // or at the end if you prefer.
        // Here, I'm adding it at the end of the string:
        body += endBoundary;
        
        // You can also insert it at a specific position:
        // body.insert(position, endBoundary);
        
        // Now, foundPos will point to the position where endBoundary was inserted.
        foundPos = body.size() - endBoundary.size();
    }

	body.erase(foundPos);
	// split the new body by the boundary string
	std::string splitBoundary = "--" + _boundary;
	_boundary = "--" + _boundary + '\n';
	std::vector<std::string> parts;
	splitString(body, _boundary, parts);
	for(size_t i = 0; i < parts.size(); i++)
	{
		std::string line;
		Part part;
		_parts.push_back(part);
		parseHeaderBlockFromString(parts[i], line, _parts[i].headers);
		parseContentDisposition(_parts[i]);
		_parts[i].data = parts[i];
	}
}

void HttpRequest::parseContentDisposition(Part &part)
{
	std::vector<std::string> tokens;
	splitString(part.headers["Content-Disposition"], ";", tokens);
	part.contentDisposition.type = tokens[0];
	// loop over remaining tokens and extract any that match 'name' or 'filename'
	for(size_t i = 1; i < tokens.size(); i++)
	{
		trimWhitespace(tokens[i]);
		if(startsWith(tokens[i], "name=")) {
			removeFromStringStart(tokens[i], "name=");
			part.contentDisposition.name = stripDoubleQuotes(tokens[i]);
		}
		if(startsWith(tokens[i], "filename=")) {
			removeFromStringStart(tokens[i], "filename=");
			part.contentDisposition.filename = stripDoubleQuotes(tokens[i]);
		}		
	}	
}

bool HttpRequest::isContentOfType(std::map<std::string, std::string> headers, std::string typeToMatch)
{
	if (headers["Content-Type"] != typeToMatch) {
		return false;
	}
	return true;
}

// 5. Check if content type of all parts matches give type
bool HttpRequest::arePartContentsOfType(std::string typeToMatch)
{
	for(size_t i = 0; i < _parts.size(); i++)
	{
		if (!isContentOfType(_parts[i].headers, typeToMatch))
			return false;
	}
	return true;
}

void HttpRequest::validate()
{
	if(getMethod() != "POST") {
		std::cout << "Request is not a post request" << std::endl;
		exit(EXIT_FAILURE);
	}
	if(getResource() != "/upload.php") { // hardcoded for now
		std::cout << "Resource requested does not match server-side resource" << std::endl;
		exit(EXIT_FAILURE);
	}
	if(getHttpVersion() != "HTTP/1.1") {
		std::cout << "HTTP version is not HTTP/1.1" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::map<std::string, std::string>::iterator it = _incomingRequest.find("Content-Length");
	if (!(it != _incomingRequest.end())) {
		std::cout << "Content-Length field must be present in header" << std::endl;
		exit(EXIT_FAILURE);
	}
	removeNonPrintableChars(_incomingRequest["Content-Length"]); // why is this needed? try to resolve issue before here
	if(!isValidNonNegativeIntegerString(_incomingRequest["Content-Length"])) {
		std::cout << "Content-Length field is not a valid non-negative integer string" << std::endl;
		exit(EXIT_FAILURE);
	}
	for(size_t i = 0; i < _parts.size(); i++)
	{
		if (!(_parts[i].contentDisposition.type == "form-data"))
		{
			std::cout << "Content disposition type must be 'form-data'" << std::endl;
			exit(EXIT_FAILURE);
		}
	}	
}

// loop over the parts & if filename exists for the part, open a file with that name, write data payload to it and close
void HttpRequest::makeOutputFile()
{
	std::string outputDirName = "tmp/";
	for(size_t i = 0; i < _parts.size(); i++)
	{
		std::string filename = outputDirName + _parts[i].contentDisposition.filename;
		if (!filename.empty())
		{
			std::ofstream outputFile(filename.c_str());
			if (!outputFile.is_open()) {
				std::cerr << "Failed to open the file for writing." << std::endl;
				break ;
			}
			outputFile << _parts[i].data;
			outputFile.close();
			std::cout << "Output file written to: " << filename << std::endl;
		}
	}
}

// utility functions

// Check if the method consists of uppercase and lowercase letters only
bool HttpRequest::isValidMethod(const std::string& method) {
	return std::string::npos == method.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

// Check if the resource contains valid characters (letters, digits, '/', '.', '-', '_', '~')
bool HttpRequest::isValidResource(const std::string& resource) {
	return std::string::npos == resource.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/._-~");
}

bool HttpRequest::isValidNonNegativeIntegerString(const std::string& str) {
    // Check if the string is empty
    if (str.empty())
        return false;
    // Check each character in the string
    for (size_t i = 0; i < str.length(); ++i) {
        if (!isdigit(str[i])) {
            // Character is not a digit
            return false;
        }
    }
    // Check if the first character is '0' (if the length is greater than 1)
    if (str.length() > 1 && str[0] == '0')
        return false;
    return true;
}

// Check if the HTTP version follows the format HTTP/x.y
bool HttpRequest::isValidHttpVersion(const std::string& httpVersion) {
	return (httpVersion.length() >= 8 && httpVersion.substr(0, 5) == "HTTP/" && httpVersion[5] == '1' && httpVersion[6] == '.' && httpVersion[7] >= '0' && httpVersion[7] <= '9');
}

void HttpRequest::splitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& output) {
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

void HttpRequest::trimWhitespace(std::string& str) {
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

std::string HttpRequest::stripDoubleQuotes(const std::string& str) {
    std::string result = str;
    // Check if the string has at least two characters and both are quotes
    if (result.length() >= 2 && result[0] == '"' && result[result.length() - 1] == '"') {
        result = result.substr(1, result.length() - 2);
    }
    return result;
}

bool HttpRequest::startsWith(const std::string& fullString, const std::string& start) {
	if (fullString.length() < start.length())
		return false;
	for (std::string::size_type i = 0; i < start.length(); ++i) {
		if (fullString[i] != start[i])
			return false;
	}
	return true;
}

void HttpRequest::removeFromStringStart(std::string& fullString, const std::string& line) {
	std::string::size_type foundPos = fullString.find(line);

	if (foundPos == 0) {
		// Line found at the start, erase it
		fullString.erase(0, line.length());
		// Additionally, remove any leading whitespace or newline
		while (!fullString.empty() && (fullString[0] == ' ' || fullString[0] == '\t' || fullString[0] == '\n' || fullString[0] == '\r'))
			fullString.erase(0, 1);
	}
}

void HttpRequest::printPostUploadRequest() {
	if (!getMethod().empty()) {
		std::cout << "Method: " << getMethod() << std::endl;
		std::cout << "Resource: " << getResource() << std::endl;
		std::cout << "HTTP Version: " << getHttpVersion() << std::endl;
	}
}	

void HttpRequest::printPartHeaders() {
	std::cout << "Printing Multipart Headers:" << std::endl;
	for(size_t i = 0; i < _parts.size(); i++)
	{
		printHeaders(_parts[i].headers);
	}
}

void HttpRequest::printHeaders(std::map<std::string, std::string> map) {
	std::map<std::string, std::string>::iterator it;
	for (it = map.begin(); it != map.end(); ++it) {
		std::cout << "Field: " << it->first << ", Value: " << it->second << std::endl;
	}
}

bool HttpRequest::hasFileExtension(std::string const &resource)
{
    std::string::size_type pos = resource.find(".");
    if (pos != std::string::npos)
        return true;
    return false;
}

bool HttpRequest::isDirectory(std::string const &resource)
{
    if (resource == "/")
        return true;
    if (resource[resource.length() - 1] == '/')
        return true;
    return false;
}

bool HttpRequest::locationIsSet(std::string const &key)
{
    if (_config->locationConfigs.find(key) != _config->locationConfigs.end())
        return true;
    return false;
}

void HttpRequest::printRequest()
{
    for (std::map<std::string, std::string>::iterator it = _incomingRequest.begin(); it != _incomingRequest.end(); ++it)
    {
        // PRINT(HTTPREQUEST, BG_BOLD_MAGENTA, it->first << ": " << RESET << it->second)
        std::cout << BG_BOLD_MAGENTA << it->first << ": " << RESET << it->second << std::endl;
    }
}

std::string const &HttpRequest::getPath()
{
    return _path;
}

void HttpRequest::setPath(std::string const &path)
{
    _path = path;
}

std::string const &HttpRequest::getContentType() const
{
    return _contentType;
}

std::string const &HttpRequest::getRedirection() const
{
    return _locationConfig->redirection;
}

std::string const HttpRequest::getHost()
{
    std::string host = _incomingRequest["Host"];
    removeNonPrintableChars(host);
    return host;
}

ServerConfig *HttpRequest::getConfig() const
{
    return _config;
}

LocationConfig *HttpRequest::getLocationConfig() const
{
    return _locationConfig;
}

std::string const HttpRequest::getMethod()
{
    std::string method = _incomingRequest["Method"];
    removeNonPrintableChars(method);
    return method;
}

std::string const HttpRequest::getResource()
{
    std::string resource = _incomingRequest["Resource"];
    removeNonPrintableChars(resource);
    return resource;
}

std::string const HttpRequest::getHttpVersion()
{
    std::string protocol = _incomingRequest["Protocol"];
    removeNonPrintableChars(protocol);
    return protocol;
}

std::string const &HttpRequest::getBody() const
{
    return _body;
}

std::string const &HttpRequest::getBoundary() const
{
	return _boundary;
}
