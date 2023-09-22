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
	executePhpScript(pur);
}

CGIUploader::~CGIUploader() {
	
}

    //char documentRoot[] = "DOCUMENT_ROOT=/nfs/homes/gbooth/42cursus/5_3_webserv_repo/cgiUploader"; // full path
    // envp[3] = documentRoot;
	// ("FILES_fileToUpload_name", , 1);
	// ("FILES_fileToUpload_type", postUploadRequest.parts[0].headers["content-type"].c_str(), 1);
	// ("FILES_fileToUpload_tmp_name", _dataOutputFilename.c_str(), 1);
	// ("FILES_fileToUpload_error", "0", 1); // where to get this from?
	// ("FILES_fileToUpload_size", "12345", 1);
	// // CGI_UPLOAD_DIR take from config - if not in dict, upload not allowed
    // char requestMethod[] = "REQUEST_METHOD=POST";
    // char contentType[] = "CONTENT_TYPE=multipart/form-data";

int CGIUploader::executePhpScript(PostUploadRequest &postUploadRequest)
{
	const char *phpPath = "/usr/bin/php"; // need full path to executable here
	const char *scriptPath = "upload.php";
	const char *filePath = _dataOutputFilename.c_str();

    char* envp[4];
    char contentLength[] = "CONTENT_LENGTH=1234";
	
    envp[0] = const_cast<char*>(("REQUEST_METHOD=" + postUploadRequest.method).c_str());
	envp[1] = const_cast<char*>(postUploadRequest.parts[0].contentDisposition.filename.c_str());
    envp[2] = contentLength;
    envp[3] = NULL;

	char *const args[] = {(char *)phpPath, (char *)scriptPath, (char *)filePath, NULL};
	std::cout << "Executing command: " << phpPath << " " << args[1] << " " << args[2] << "\n";
	execve(phpPath, args, envp);

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