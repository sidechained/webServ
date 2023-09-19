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
    std::string filePath = request.getResource();

    std::cout << BG_BOLD_MAGENTA << "Creating HTML response for resource" << request.getResource() << RESET << std::endl;
    std::cout << BG_BOLD_MAGENTA << "Looking for resource at " << filePath << RESET << std::endl;

    if (request.isNoSlash())
    {
        setHeader(MovedHeader(request.getRequest()["Host"], request.getResource()).getHeader());
        setBodySent(true);
        return;
    }

    std::ifstream htmlFile(filePath.c_str(), std::ios::binary);
    if (!htmlFile)
    {
        std::cerr << "Error opening HTML file" << std::endl;
        return;
    }
    setBodyLength(this->fileLength(htmlFile));
    setHeader(OkHeader(this->getRequest().getContentType(), this->getBodyLength()).getHeader());

    // Set body
    std::ostringstream oss2;
    oss2 << htmlFile.rdbuf();
    std::string body = oss2.str();
    setBody(body);

    std::cout << BG_BOLD_MAGENTA << "Header: " << RESET << this->getHeader().substr(0, 1000) << std::endl;
    std::cout << BG_BOLD_MAGENTA << "Body: " << RESET << this->getBody().substr(0, 1000) << std::endl;

    // clear and reset htmlFile
    htmlFile.clear();
    htmlFile.seekg(0, std::ios::beg);
    htmlFile.close();
}