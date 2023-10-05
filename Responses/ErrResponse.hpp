#pragma once
#include "SimpleResponse.hpp"
#include "../Parser/ServerParser.hpp"

#include <unistd.h>

class ErrResponse : public SimpleResponse
{
private:
    void createDefaultErrResponse(std::string &error);
public:
    ErrResponse(HttpRequest &request);
    ~ErrResponse();
    void createErrResponse(std::string &error);
    void createErrResponse(std::string &error, ServerConfig *config);
};
