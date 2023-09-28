#include "HttpRequest.hpp"

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
    cleanUpMap(_incomingRequest);
}

void HttpRequest::parseVectorParts()
{
    if (getMethod() == "POST" && _incomingRequest["Content-Type"] == "multipart/form-data")
    {
        std::cout << "we are multipart!!" << std::endl;
        std::istringstream bodyStream(_body);
        
    }
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
        std::cout << line << std::endl;
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
			boundary = tokens[i];
		}
	}
    std::cout << "bnd:" << boundary << std::endl;
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
    std::cout << "Method in request: " << method << std::endl;
    removeNonPrintableChars(method);
    return method;
}

std::string const &HttpRequest::getBody() const
{
    return _body;
}