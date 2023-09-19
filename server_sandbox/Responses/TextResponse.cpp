#include "TextResponse.hpp"

TextResponse::TextResponse(HttpRequest &request) : SimpleResponse(request)
{
    createResponse();
}

TextResponse::~TextResponse()
{
}

std::string removeNonPrintableChars(const std::string& input) {
    std::string result;
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        if (c >= 32 && c <= 126) {
            result += c;
        }
    }
    return result;
}


void TextResponse::createResponse()
{
    HttpRequest request = this->getRequest();
    std::string filePath = request.getResource();
    // std::string filePath = "www" + request.getResource();

    std::cout << BG_BOLD_MAGENTA << "Creating HTML response for resource" << request.getResource() << RESET << std::endl;
    std::cout << BG_BOLD_MAGENTA << "Looking for resource at " << filePath << RESET << std::endl;

    if (request.isNoSlash())
    {
        std::cout << "No slash found in resource" << std::endl;
        std::string new_location = "http://" + request.getRequest()["Host"] + request.getResource() + "/";
        new_location = removeNonPrintableChars(new_location);

        std::cout << "new_location: " << new_location << std::endl;
        std::ostringstream oss;

        oss << "HTTP/1.1 301 Moved Permanently\r\n";
        oss << "Location: " << new_location << "\r\n";
        oss << "Content-Length: 0\r\n";
        oss << "\r\n";

        std::string header = oss.str();
        setHeader(header);
        setBody("");
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

    std::cout << BG_BOLD_MAGENTA << "Header: " << RESET << header.substr(0, 1000) << std::endl;
    std::cout << BG_BOLD_MAGENTA << "Body: " << RESET << body.substr(0, 1000) << std::endl;

    // clear and reset htmlFile
    htmlFile.clear();
    htmlFile.seekg(0, std::ios::beg);
    htmlFile.close();
}