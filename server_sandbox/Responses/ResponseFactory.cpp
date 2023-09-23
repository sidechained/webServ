#include "ResponseFactory.hpp"

ResponseFactory::~ResponseFactory()
{
}

SimpleResponse *ResponseFactory::createResponse(HttpRequest &request)
{
    SimpleResponse *response = NULL;
    if (!request.hasError())
        response = new TextResponse(request);
    else
    {
       // if (request.getError("noSlash") || request.getError("redirection"))
      
    }
    return response;
}