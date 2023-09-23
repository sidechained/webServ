#include "RedirResponse.hpp"

RedirResponse::RedirResponse(HttpRequest &request) : SimpleResponse(request)
{
    if (request.hasError())
        createResponse();
}

RedirResponse::~RedirResponse()
{
}

void RedirResponse::createResponse()
{
    HttpRequest request = this->getRequest();

    if (request.getError("noSlash"))
    {
        std::cout << BG_BOLD_MAGENTA << "No slash in resource" << RESET << std::endl;
        setHeader(MovedHeader(request.getHost(), request.getPath()).getHeader());
        setBodySent(true);
        return;
    }
    else if (request.getRedirection() != "")
    {
        std::cout << BG_BOLD_MAGENTA << "Redirection in resource" << RESET << std::endl;
        setHeader(MovedHeader(request.getHost(), request.getRedirection()).getHeader());
        setBodySent(true);
        printResponse();
        return;
    }

}