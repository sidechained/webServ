#pragma once
#include "SimpleResponse.hpp"

class ErrResponse : public SimpleResponse
{
private:
    void createDefaultErrResponse(std::string &error);
public:
    ErrResponse(HttpRequest &request);
    ~ErrResponse();
    void createErrResponse(std::string &error);
};
