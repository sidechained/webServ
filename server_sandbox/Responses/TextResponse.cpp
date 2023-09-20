#include "TextResponse.hpp"

TextResponse::TextResponse(HttpRequest &request) : SimpleResponse(request)
{
    createResponse();
}

TextResponse::~TextResponse()
{
}

void TextResponse::createResponse()
{
    HttpRequest request = this->getRequest();
    std::string filePath = "www" + request.getResource();

    std::cout << BG_BOLD_MAGENTA << "Creating HTML response for resource" << request.getResource() << RESET << std::endl;
    std::cout << BG_BOLD_MAGENTA << "Looking for resource at " << filePath << RESET << std::endl;

    std::ifstream htmlFile(filePath.c_str(), std::ios::binary);
    if (!htmlFile)
    {
        std::cerr << "Error opening HTML file" << std::endl;
        return;
    }
    setBodyLength(this->fileLength(htmlFile));

    std::ostringstream oss;

        oss << "HTTP/1.1 200 OK\r\n";
        oss << "Content-Type: " << this->getRequest().getContentType() << "\r\n";
        oss << "Content-Length: " << this->getBodyLength() << "\r\n";
        oss << "\r\n";

    std::string header = oss.str();
    setHeader(header);

    // Set body
    std::ostringstream oss2;
    oss2 << htmlFile.rdbuf();
    std::string body = oss2.str();
    setBody(body);

	_response = _header + _body;

    std::cout << BG_BOLD_YELLOW<< "RESPONSE" << RESET << header.substr(0, 1000) << std::endl;
    std::cout << BG_BOLD_MAGENTA << "Header: " << RESET << header.substr(0, 1000) << std::endl;
    //std::cout << BG_BOLD_MAGENTA << "Body: " << RESET << body.substr(0, 1000) << std::endl;

    // clear and reset htmlFile
    htmlFile.clear();
    htmlFile.seekg(0, std::ios::beg);
    htmlFile.close();
}