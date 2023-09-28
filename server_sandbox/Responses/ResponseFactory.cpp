#include "ResponseFactory.hpp"
#include <iostream>


ResponseFactory::ResponseFactory()
{
}

ResponseFactory::~ResponseFactory()
{
}

SimpleResponse *ResponseFactory::GETresponse(HttpRequest &request)
{
    SimpleResponse *response = NULL;
    if (request.hasError() && request.getError("noSlash"))
        response = new RedirResponse(request);
    else
        response = new TextResponse(request);
    return response;
}

SimpleResponse *ResponseFactory::POSTresponse(HttpRequest &request)
{
    SimpleResponse *response = NULL;
    response = new UploadResponse(request);
    return response;
}

SimpleResponse *ResponseFactory::createResponse(HttpRequest &request)
{
    SimpleResponse *response = NULL;
    
    std::string method = request.getMethod();
    std::cout << "Method: " << method << std::endl;
    if (method == "GET")
        response = GETresponse(request);
    else if (method == "POST")
        response = POSTresponse(request);
    return response;
}