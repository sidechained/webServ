#include "CGIUploader.hpp"

CGIUploader::CGIUploader()
{
    std::fstream requestFile = openRequestFile("postRequest.txt");
    if (matchResource("cgi/upload.php"))
        prepareEnvironment();
    else

    requestFile.close();
}

CGIUploader::~CGIUploader()
{
    
}

std::fstream CGIUploader::openRequestFile(std::string filename) {
    std::fstream requestFile(filename.c_str());
	if(!requestFile.is_open())
    {
        std::cout << "could not open file" << std::endl;
        exit(EXIT_FAILURE);
    }
    return requestFile;
}

bool CGIUploader::matchResource(const std::string& resourceToMatch) {
    HttpRequest request;
    std::istringstream requestStream(request_str);
    requestStream >> request.method >> request.resource >> request.httpVersion; // first line
    if (request.resource == resourceToMatch)
        return true;
    else
       return false;
}

bool CGIUploader::prepareEnvironment() {

    setenv("CGI_REQUEST_METHOD", request.method, 1);
    setenv("CGI_FILE_TO_UPLOAD", , 1);
    setenv("CGI_MAX_CLIENT_BODY_SIZE_MB", config.);
    CGI_MAX_CLIENT_BODY_SIZE_MB
    CGI_POST_ALLOWED - dict of route, true - take from config
    CGI_UPLOAD_DIR - dict of route, path - take from config - if not in dict, upload not allowed
}