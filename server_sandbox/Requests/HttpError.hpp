#pragma once
#include <map>
#include <string>

class HttpError
{
private:
   std::map<std::string, bool> _errors;
public:
    HttpError(/* args */);
    ~HttpError();
    bool hasError();
    void addError(std::string const &error);
    void removeError(std::string const &error);
    bool getError(std::string const &error);
    void clearErrors();
};


