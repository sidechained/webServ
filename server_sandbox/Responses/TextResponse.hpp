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
	void createAutoIndexResponse(const char *dirName);
	void genDir(const char *dirName);
	void genLink(std::string entryName, std::string dirName);
public:
    TextResponse(HttpRequest &request);
    ~TextResponse();
    void createResponse(HttpRequest &request);
};


