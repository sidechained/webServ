#pragma once
#include "Colors.hpp"

#define PRINT(condition, color, statement) \
    if (condition) { \
        std::cout << color << statement << RESET << std::endl; \
    }

#define HTTPREQUEST		1
#define TEXTRESPONSE	1
#define SERVERMANAGER	1
#define SOCKET			1
#define CGI				1
#define FORMRESPONSE	1
