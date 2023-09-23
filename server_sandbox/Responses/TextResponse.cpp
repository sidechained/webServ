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
    HttpRequest request = getRequest();
    request.printRequest();

	PRINT(TEXTRESPONSE, BG_BOLD_MAGENTA, "Creating HTML response for resource");

    if (request.getError("noSlash"))
    {
        std::cout << BG_BOLD_MAGENTA << "No slash in resource" << RESET << std::endl;
        setHeader(MovedHeader(request.getHost(), request.getPath()).getHeader());
        setBodySent(true);
        return;
    }
    if (request.getRedirection() != "")
    {
        std::cout << BG_BOLD_MAGENTA << "Redirection in resource" << RESET << std::endl;
        setHeader(MovedHeader(request.getHost(), request.getRedirection()).getHeader());
        setBodySent(true);
        printResponse();
        return;
    }

    std::ifstream htmlFile(request.getPath().c_str(), std::ios::binary);

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

	_response = _header + _body;

	PRINT(TEXTRESPONSE, BG_BOLD_MAGENTA, "Header: " << this->getHeader().substr(0, 1000))
    //std::cout << BG_BOLD_MAGENTA << "Header: " << RESET << this->getHeader().substr(0, 1000) << std::endl;
	PRINT(TEXTRESPONSE, BG_BOLD_MAGENTA, "Body: " << this->getBody().substr(0, 100))
    //std::cout << BG_BOLD_MAGENTA << "Body: " << RESET << this->getBody().substr(0, 100) << std::endl;

    // clear and reset htmlFile
    htmlFile.clear();
    htmlFile.seekg(0, std::ios::beg);
    htmlFile.close();
}