#include "HttpError.hpp"

HttpError::HttpError()
{
    //add noSlash, methodNotAllowed to map
    _errors["noSlash"] = false;
    _errors["methodNotAllowed"] = false;
}

HttpError::~HttpError()
{
}

bool HttpError::hasError()
{
    for (std::map<std::string, bool>::iterator it = _errors.begin(); it != _errors.end(); it++)
    {
        if (it->second == true)
            return true;
    }
    return false;
}

void HttpError::addError(std::string const &error)
{
    _errors[error] = true;
}

void HttpError::removeError(std::string const &error)
{
    _errors[error] = false;
}

bool HttpError::getError(std::string const &error)
{
    bool errorExists = _errors.find(error) != _errors.end();
    if (!errorExists)
        return false;
    else 
        return _errors[error];
}

