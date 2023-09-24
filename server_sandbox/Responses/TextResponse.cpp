#include "TextResponse.hpp"

TextResponse::TextResponse(HttpRequest &request) : SimpleResponse(request)
{
    createResponse(request);
}

TextResponse::~TextResponse()
{
}

void TextResponse::createResponse(HttpRequest &request)
{
    request.printRequest();

	PRINT(TEXTRESPONSE, BG_BOLD_MAGENTA, "Creating HTML response for resource");


    if (request.getRedirection() != "")
    {
        std::cout << BG_BOLD_MAGENTA << "Redirection in resource" << RESET << std::endl;
        setHeader(MovedHeader(request.getHost(), request.getRedirection()).getHeader());
        setBodySent(true);
        printResponse();
        return;
    }

    std::cout << BG_BLUE << "Path: " << RESET << request.getPath() << std::endl;
    std::ifstream htmlFile(request.getPath().c_str(), std::ios::binary);

    if (!htmlFile)
    {
        std::cerr << "Error opening HTML file" << std::endl;
        ServerConfig *config = request.getConfig();
        std::string errorPage = config->error_pages["404"];
        std::cout << "Error page: " << errorPage << std::endl;
        request.setPath(errorPage);
        std::cout << "New path: " << request.getPath() << std::endl;
        htmlFile.open(request.getPath().c_str(), std::ios::binary);
        if (!htmlFile)
        {
            std::cerr << "Error opening error page" << std::endl;
            return;
        }
        setBodyLength(this->fileLength(htmlFile));
        std::ostringstream oss1;
        //create header
        oss1 << "HTTP/1.1 404 Not Found\r\n";
        oss1 << "Content-Type: text/html\r\n";
        oss1 << "Content-Length: " << this->getBodyLength() << "\r\n";
        oss1 << "\r\n";
        std::string header = oss1.str();
        setHeader(header);

        std::ostringstream oss2;
        oss2 << htmlFile.rdbuf();
        std::string body = oss2.str();
        setBody(body);

        _response = _header + _body;
        return;
    }
    setBodyLength(this->fileLength(htmlFile));
    setHeader(OkHeader(this->getRequest().getContentType(), this->getBodyLength()).getHeader());

    // Set body
    std::ostringstream oss2;
    oss2 << htmlFile.rdbuf();
    std::string body = oss2.str();
    setBody(body);


	PRINT(TEXTRESPONSE, BG_BOLD_MAGENTA, "Header: " << this->getHeader().substr(0, 1000))
    // std::cout << BG_BOLD_MAGENTA << "Header: " << RESET << this->getHeader().substr(0, 1000) << std::endl;
	PRINT(TEXTRESPONSE, BG_BOLD_MAGENTA, "Body: " << this->getBody().substr(0, 100))
    // std::cout << BG_BOLD_MAGENTA << "Body: " << RESET << this->getBody().substr(0, 100) << std::endl;

    // clear and reset htmlFile
    htmlFile.clear();
    htmlFile.seekg(0, std::ios::beg);
    htmlFile.close();
}

