#include "RedirResponse.hpp"

RedirResponse::RedirResponse(HttpRequest &request) : SimpleResponse(request)
{
    createResponse(request);
}

RedirResponse::~RedirResponse()
{
}

void RedirResponse::createResponse(HttpRequest &request)
{
    _cgi = false;
    if (request.getError("noSlash"))
    {
        std::cout << BG_BOLD_MAGENTA << "No slash in resource" << RESET << std::endl;
        setHeader(MovedHeader(request.getHost(), request.getPath()).getHeader());
        return;
    }
    else if (request.getRedirection() != "")
    {
        std::cout << BG_BOLD_MAGENTA << "Redirection in resource" << RESET << std::endl;
        setHeader(MovedHeader(request.getHost(), request.getRedirection()).getHeader());
        return;
    }

}