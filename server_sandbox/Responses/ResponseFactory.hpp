#pragma once
#include "../Requests/HttpRequest.hpp"
#include "SimpleResponse.hpp"
#include "TextResponse.hpp"

class ResponseFactory
{
private:
    ResponseFactory();
public:
    ~ResponseFactory();
    static SimpleResponse *createResponse(HttpRequest &request);
};
