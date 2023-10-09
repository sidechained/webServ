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

	// if (response)
	// 	std::cout << BG_BLUE "cgi form response text" RESET << response->isCgi() << std::endl;

	return response;

}

SimpleResponse *ResponseFactory::POSTresponse(HttpRequest &request)
{
    SimpleResponse *response = NULL;
    if (request.hasError() && request.getError("noCGI"))
        response = new TextResponse(request);
    else
     response = new FormResponse(request);
    return response;
}

// what should this do?
SimpleResponse *ResponseFactory::DELETEresponse(HttpRequest &request)
{
	std::cout << "DELETE response" << std::endl;
    SimpleResponse *response = NULL;
    response = new FormResponse(request);
    return response;
}

SimpleResponse *ResponseFactory::createResponse(HttpRequest &request)
{
    SimpleResponse *response = NULL;
    
    std::cout << BG_RED << "resource" << request.getResource() << RESET << std::endl;
    std::string method = request.getMethod();
    //print all errors

    if (request.hasError() && !request.getError("noSlash"))
        response = new TextResponse(request);
	else if (method == "GET" && request.getContentType() == "application/x-httpd-php")
		response = POSTresponse(request);
    else if (method == "GET")
        response = GETresponse(request);
    else if (method == "POST")
        response = POSTresponse(request);
	else if (method == "DELETE")
		response = DELETEresponse(request);
    return response;
}
