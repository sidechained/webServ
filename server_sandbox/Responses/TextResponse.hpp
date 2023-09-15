#pragma once
#include "SimpleResponse.hpp"

class TextResponse : public SimpleResponse
{
private:
public:
    TextResponse(HttpRequest &request);
    ~TextResponse();
    void createResponse();
};


