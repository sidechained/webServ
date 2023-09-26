#include "UploadResponse.hpp"

UploadResponse::UploadResponse(HttpRequest &request) : ErrResponse(request)
{
    //createResponse(request);
}


UploadResponse::~UploadResponse()
{
}

void UploadResponse::createResponse(HttpRequest &request)
{
    std::cout << "Creating upload response"<< std::endl;
    request.printRequest();
    std::string response = "";
    response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "\r\n";
    response += "<html><body><h1>Upload</h1></body></html>\r\n\r\n";
    response += "\r\n";
    this->setHeader(response);
}