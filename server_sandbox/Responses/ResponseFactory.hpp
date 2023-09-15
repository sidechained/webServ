#pragma once
#include "../Requests/HttpRequest.hpp"
#include "SimpleResponse.hpp"
#include "TextResponse.hpp"

class ResponseFactory
{
private:
    /* data */
public:
    ResponseFactory();
    ~ResponseFactory();
    static SimpleResponse createResponse(HttpRequest &request);
};