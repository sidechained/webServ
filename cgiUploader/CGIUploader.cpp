#include "CGIUploader.hpp"
#include "../postUploadRequestParser/PostUploadRequestParser.hpp"

// TODO:
// - implement check to see if POST method allowed for given route (in serverConfig)

CGIUploader::CGIUploader(ServerConfig &serverConfig, std::string postRequestFilename, std::string dataOutputFilename) : _dataOutputFilename(dataOutputFilename)
{
	PostUploadRequestParser purp(postRequestFilename, dataOutputFilename);
	std::string resourceToMatch = "/cgi/upload.php";
	(void)serverConfig;
	// implement check here to see if POST method is allowed for this route (in serverConfig)
	if (stringToUnsignedInt(purp.getStruct().headers["content-length"]) > serverConfig.clientMaxBodySizeMB * 1000)
	{
		std::cerr << "Content length greater than client max body size" << std::endl;
		exit(EXIT_FAILURE);
	}
	PostUploadRequest pur = purp.getStruct();
	prepareEnvironment(pur);
}

CGIUploader::~CGIUploader() {
	
}

void CGIUploader::prepareEnvironment(PostUploadRequest &postUploadRequest) {
	setenv("REQUEST_METHOD", postUploadRequest.method.c_str(), 1);
	setenv("FILES_fileToUpload_name", postUploadRequest.parts[0].contentDisposition.filename.c_str(), 1);
	setenv("FILES_fileToUpload_type", postUploadRequest.parts[0].headers["content-type"].c_str(), 1);
	setenv("FILES_fileToUpload_tmp_name", _dataOutputFilename.c_str(), 1);
	// setenv("FILES_fileToUpload_error", "0", 1); // where to get this from?
	// setenv("FILES_fileToUpload_size", "12345", 1);
	// // CGI_UPLOAD_DIR take from config - if not in dict, upload not allowed
	executePhpScript(_dataOutputFilename.c_str());
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

unsigned int CGIUploader::stringToUnsignedInt(const std::string& str) {
	std::istringstream iss(str);
	unsigned int result;
	
	if (!(iss >> result)) {
		// Conversion failed
		std::cerr << "Failed to convert the string to an unsigned int." << std::endl;
		return 0;  // Return 0 or another suitable value for failure
	}

	return result;
}