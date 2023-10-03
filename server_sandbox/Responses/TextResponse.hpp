#pragma once
#include "ErrResponse.hpp"
# include <cstdlib>
# include <iostream>
# include <fstream>
# include <dirent.h>
# include <sstream>
# include <stdio.h>
# include <errno.h>

class TextResponse : public ErrResponse
{
private:
    void createAutoIndexResponse(std::string dirName, std::string resource);
	void genDir(std::string dirName, std::string resource);
	void genLink(std::string entryName, std::string dirName);
public:
    TextResponse(HttpRequest &request);
    ~TextResponse();
    void createResponse(HttpRequest &request);
};


