#pragma once
#include "../Requests/HttpRequest.hpp"
#include "SimpleResponse.hpp"
#include "TextResponse.hpp"
#include "RedirResponse.hpp"
#include "FormResponse.hpp"
#include <map>
#include <functional>

class ResponseFactory
{
private:
    ResponseFactory();


public:
    ~ResponseFactory();
    static SimpleResponse *createResponse(HttpRequest &request);
    static SimpleResponse *GETresponse(HttpRequest &request);
    static SimpleResponse *POSTresponse(HttpRequest &request);
	static SimpleResponse *DELETEresponse(HttpRequest &request);
};
