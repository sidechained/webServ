#include "ResponseFactory.hpp"

ResponseFactory::~ResponseFactory()
{
}

SimpleResponse *ResponseFactory::createResponse(HttpRequest &request)
{
    SimpleResponse *response = NULL;
    if (!request.hasError() && !request.getError("noSlash"))
    {
        std::cout << "Creating text response" << std::endl << std::endl << std::endl;
        response = new TextResponse(request);
    }
    else
    {
        std::cout << "Creating REDIR response" << std::endl << std::endl << std::endl;
        response = new RedirResponse(request);
    }
    return response;
}