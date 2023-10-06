#include "ErrResponse.hpp"

ErrResponse::ErrResponse(HttpRequest &request) : SimpleResponse(request)
{
}

ErrResponse::~ErrResponse()
{
}

void ErrResponse::createErrResponse(std::string &error, ServerConfig *config)
{
    std::string errPath = config->error_pages[error];
	//print current working directory
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		std::cout << BG_RED << "Current working dir: " << cwd << RESET << std::endl;
    std::ifstream htmlFile(errPath.c_str(), std::ios::binary);
    if (!htmlFile)
    {
        createDefaultErrResponse(error);
        return;
    }
    setBody(htmlFile);
    std::cout << BG_RED << "Body: " << getBody() << std::endl;

    std::ostringstream oss1;
    oss1 << "HTTP/1.1 " << error << "\r\n";
    oss1 << "Content-Type: text/html\r\n";
    oss1 << "Content-Length: " << this->getBodyLength() << "\r\n";
    oss1 << "\r\n";
    std::string header = oss1.str();
    setHeader(header);
}

void ErrResponse::createErrResponse(std::string &error)
{
    HttpRequest request = getRequest();
    std::string errPath = request.getConfig()->error_pages[error];
    // std::cout << BG_RED << "Error path: " << errPath << std::endl;
	//print current working directory
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		std::cout << BG_RED << "Current working dir: " << cwd << RESET << std::endl;
    std::ifstream htmlFile(errPath.c_str(), std::ios::binary);
    if (!htmlFile)
    {
        createDefaultErrResponse(error);
        return;
    }
    setBody(htmlFile);
    std::cout << BG_RED << "Body: " << getBody() << std::endl;

    std::ostringstream oss1;
    oss1 << "HTTP/1.1 " << error << "\r\n";
    oss1 << "Content-Type: text/html\r\n";
    oss1 << "Content-Length: " << this->getBodyLength() << "\r\n";
    oss1 << "\r\n";
    std::string header = oss1.str();
    setHeader(header);
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

    std::ostringstream oss1;
    oss1 << "HTTP/1.1 " << error << "\r\n";
    oss1 << "Content-Type: text/html\r\n";
    oss1 << "Content-Length: " << this->getBodyLength() << "\r\n";
    oss1 << "\r\n";
    std::string header = oss1.str();
    setHeader(header);
}