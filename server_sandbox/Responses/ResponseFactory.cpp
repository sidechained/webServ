#include "ResponseFactory.hpp"

ResponseFactory::ResponseFactory()
{
}

ResponseFactory::~ResponseFactory()
{
}

SimpleResponse ResponseFactory::createResponse(HttpRequest &request)
{
    if (request.getMethod() == "GET")
    {
        return TextResponse(request);
    }
    else
        return SimpleResponse(request);
}