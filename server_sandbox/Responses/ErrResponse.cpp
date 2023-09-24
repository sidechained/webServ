#include "ErrResponse.hpp"

ErrResponse::ErrResponse(HttpRequest &request) : SimpleResponse(request)
{
}

ErrResponse::~ErrResponse()
{
}

void ErrResponse::createErrResponse(std::string &error)
{
    HttpRequest request = getRequest();
    std::string errPath = request.getConfig()->error_pages[error];
    std::ifstream htmlFile(errPath.c_str(), std::ios::binary);
    if (!htmlFile)
    {
        createDefaultErrResponse(error);
        return;
    }
    setBodyLength(this->fileLength(htmlFile));
    std::ostringstream oss1;
    //create header
    oss1 << "HTTP/1.1 " << error << "\r\n";
    oss1 << "Content-Type: text/html\r\n";
    oss1 << "Content-Length: " << this->getBodyLength() << "\r\n";
    oss1 << "\r\n";
    std::string header = oss1.str();
    setHeader(header);
    std::ostringstream oss2;
    oss2 << htmlFile.rdbuf();
    std::string body = oss2.str();
    setBody(body);
}


void ErrResponse::createDefaultErrResponse(std::string &error)
{

    std::ostringstream oss2;
    oss2 << "<html>\n";
    oss2 << "<head>\n";
    oss2 << "<title>" << error << "</title>\n";
    oss2 << "</head>\n";
    oss2 << "<body>\n";
    oss2 << "<h1>" << error << "</h1>\n";
    oss2 << "<p>Error page not found for error code: " << error << "</p>\n";
    oss2 << "</body>\n";
    oss2 << "</html>\n";

    std::string body = oss2.str();
    setBody(body);

    setBodyLength(this->getBody().length());
    std::ostringstream oss1;
    oss1 << "HTTP/1.1 " << error << "\r\n";
    oss1 << "Content-Type: text/html\r\n";
    oss1 << "Content-Length: " << this->getBodyLength() << "\r\n";
    oss1 << "\r\n";
    std::string header = oss1.str();
    setHeader(header);
}