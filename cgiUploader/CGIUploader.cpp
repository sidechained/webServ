#include "CGIUploader.hpp"

CGIUploader::CGIUploader() {
	ConfigFileParser configFileParser("serverConfig.txt");
	ServerConfig serverConfig = configFileParser.serverConfigs[0];
	std::string resourceToMatch = "/cgi/upload.php";
	std::fstream requestFile("postRequest.txt");
	if(!requestFile.is_open()) {
		std::cout << "Could not open file" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string firstLine;
	getline(requestFile, firstLine);
	HttpRequest httpRequest;
	if (matchResource(firstLine, resourceToMatch, httpRequest)) {
		prepareData();
		// these must be checked before calling php:
		// CGI_POST_ALLOWED take from config
		// setenv("CGI_CLIENT_MAX_BODY_SIZE_MB", itoa(serverConfig.clientMaxBodySizeMB), 1);		
		prepareEnvironment(httpRequest);
	}
	else {
		std::cout << "Resource does not match '" << resourceToMatch << "'"<< std::endl;
	}
	requestFile.close();
}

CGIUploader::~CGIUploader() {
	
}

bool CGIUploader::matchResource(const std::string& firstLine, const std::string& resourceToMatch, HttpRequest &httpRequest) {
	std::istringstream requestStream(firstLine);
	requestStream >> httpRequest.method >> httpRequest.resource >> httpRequest.httpVersion;
	if (httpRequest.resource == resourceToMatch)
		return true;
	return false;
}

void CGIUploader::prepareData() {
	// loop and get lines
}

void CGIUploader::prepareEnvironment(HttpRequest &httpRequest) {
	const char* filePath = "tmp/tmpfile123";
	setenv("REQUEST_METHOD", httpRequest.method.c_str(), 1);
    setenv("FILES_fileToUpload_name", "my_file.txt", 1);
    setenv("FILES_fileToUpload_type", "text/plain", 1);
    setenv("FILES_fileToUpload_tmp_name", filePath, 1);
    setenv("FILES_fileToUpload_error", "0", 1);
    setenv("FILES_fileToUpload_size", "12345", 1);
	// CGI_UPLOAD_DIR take from config - if not in dict, upload not allowed
	executePhpScript(filePath);
}

int CGIUploader::executePhpScript(const char *filePath)
{
    const char *phpPath = "/usr/bin/php"; // need full path to executable here
    const char *scriptPath = "upload.php";

    char *const args[] = {(char *)phpPath, (char *)scriptPath, (char *)filePath, NULL};
	std::cout << "Executing command: " << phpPath << " " << args[1] << " " << args[2] << "\n";
    execve(phpPath, args, NULL);

    perror("execve");
    return 1;	
}