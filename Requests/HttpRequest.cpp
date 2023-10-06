#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
{
}

HttpRequest::~HttpRequest()
{
}

HttpRequest::HttpRequest(std::string const &request, std::vector<char> &requestVector, ServerConfig *config) : _config(config)
{
	this->clearErrors();
	_errorPages = _config->error_pages;
	fillIncomingRequestMap(request);
	if (checkContentLength(config) == EXIT_FAILURE)
		return;
	fillBodyVector(requestVector);
	parseIsFile();
	parseLocationConfig();
	parseIndex();
	parseMethod();
	parseContentType();
	attemptDelete();
	cleanUpMap(_incomingRequest);
	setDefaultEnvVars();
}

void HttpRequest::setDefaultEnvVars()
{
    std::string requestMethod = "REQUEST_METHOD=POST";
	_envVars.push_back(requestMethod);
    std::string strBoundary = "BOUNDARY=" + getBoundary();
    removeNonPrintableChars(strBoundary);
	_envVars.push_back(strBoundary);
    std::string strUploadPath = "UPLOAD_PATH=." + getLocationConfig()->uploads;
    removeNonPrintableChars(strUploadPath);
	_envVars.push_back(strUploadPath);
	// for (size_t i = 0; i < _envVars.size(); i++)
	// {
	// 	std::cout << "envVars[" << i << "]: " << _envVars[i] << std::endl;
	// }
}

std::vector<std::string> HttpRequest::getEnvVars() const
{
	return _envVars;
}


void HttpRequest::parseIsFile()
{
	std::string resource = getResource();
	size_t dotPosition = resource.rfind('.');
	if (dotPosition != std::string::npos && dotPosition > 0 && dotPosition < resource.length() - 1) {
        _isFile = true;
		return;
    }
	_isFile = false;
}

 void HttpRequest::fillBodyVector(std::vector<char> const &bufferVector)
 {
	//iterate over bodyVector and skip the header lines and the empty line and save the rest to _bodyVector
	for (std::vector<char>::const_iterator it = bufferVector.begin(); it != bufferVector.end(); ++it)
	{
		//check if we are at the end of the header
		if (*it == '\r' && *(it + 1) == '\n' && *(it + 2) == '\r' && *(it + 3) == '\n')
		{
			//skip the empty line
			it += 4;
			//save the rest of the buffer to _bodyVector
			for (std::vector<char>::const_iterator it2 = it; it2 != bufferVector.end(); ++it2)
			{
				_bodyVector.push_back(*it2);
			}
			break;
		}
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

            // Check if there are query parameters in the resource
            size_t query_start = resource.find('?');
            if (query_start != std::string::npos)
            {
                std::string query_string = resource.substr(query_start + 1);
                // You can parse and process the query string here as needed
                // For example, you can split it into key-value pairs
                // and store them in your data structures.
                // Example parsing code:
                std::istringstream queryStream(query_string);
                std::string parameter;
                while (std::getline(queryStream, parameter, '&'))
                {
                    size_t equal_sign_pos = parameter.find('=');
                    if (equal_sign_pos != std::string::npos)
                    {
                        std::string param_name = parameter.substr(0, equal_sign_pos);
						std::transform(param_name.begin(), param_name.end(), param_name.begin(), ::toupper);
						//std::cout << "param_name: " << param_name << std::endl;
                        std::string param_value = parameter.substr(equal_sign_pos + 1);
						//std::cout << "param_value: " << param_value << std::endl;
						std::string env_var = param_name + "=" + param_value;
						std::cout << "env_var: " << env_var << std::endl;
						_envVars.push_back(env_var);
                        // Store param_name and param_value as needed
                    }
                }
            }
			  // Reassign the resource variable to remove query parameters
            resource = resource.substr(0, query_start);

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

int HttpRequest::checkContentLength(ServerConfig *config)
{
	// if Content-Length is in request map
	if (!(_incomingRequest["Content-Length"].empty()))
	{	
    	std::istringstream ss(_incomingRequest["Content-Length"]);
		unsigned int contentLength;
		if (!(ss >> contentLength)) {
			std::cerr << "Failed to convert content-length string to int." << std::endl;
			this->addError("internalError");
		}
		if (contentLength > config->clientMaxBodySizeMB * 1000)
		{
			std::cerr << BG_BOLD_YELLOW "Content length exceeds client max body size." RESET << std::endl;
			this->addError("entityTooLarge");
		}
		if (contentLength > MAX_BODY_SIZE)
		{
			std::cerr << BG_BOLD_YELLOW "Content length exceeds server max body size." RESET << std::endl;
			this->addError("entityTooLarge");
		}
	}
	return EXIT_SUCCESS;
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
            // std::cout << "key:" << key << std::endl;
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
}

void HttpRequest::parseIndex()
{
	if (isDirectory(_path))
	{
		if (_locationConfig && _locationConfig->index != "")
			_path = _path + _locationConfig->index;
	}
}

void HttpRequest::parseMethod()
{
	std::string requestMethod = _incomingRequest["Method"];
	// std::cout << "Parsing method: " << requestMethod << std::endl;

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

void HttpRequest::attemptDelete()
{
	if (getMethod() == "DELETE" && !this->getError("methodNotAllowed"))
	{
		std::cout << "Attempting to delete file: " << _path << std::endl;
		if (std::remove(_path.c_str()) != 0) {	
			std::cout << "File " << _path << " not found" << std::endl;
			this->addError("fileNotFound");
		}
		std::cout << "File " << _path << " successfully deleted." << std::endl;
		this->addError("deleteSuccess");
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

std::vector<char> const &HttpRequest::getBodyVector() const
{
	return _bodyVector;
}

std::string const &HttpRequest::getBoundary() const
{
	return _boundary;
}

bool const &HttpRequest::isFile() const
{
	return _isFile;
}
std::string HttpRequest::getContentLength()
{
	return _incomingRequest["Content-Length"];
}
