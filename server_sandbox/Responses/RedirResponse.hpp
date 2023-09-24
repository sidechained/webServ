#pragma once
#include "SimpleResponse.hpp"

class RedirResponse : public SimpleResponse
{
private:
public:
    RedirResponse(HttpRequest &request);
    ~RedirResponse();
    void createResponse(HttpRequest &request);
};

