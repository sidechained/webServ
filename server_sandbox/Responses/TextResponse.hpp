#pragma once
#include "../WebServ.hpp"
//#include "SimpleResponse.hpp"

class TextResponse : public SimpleResponse
{
private:
public:
    TextResponse(HttpRequest &request);
    ~TextResponse();
    void createResponse();
};


