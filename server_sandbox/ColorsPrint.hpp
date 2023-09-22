#pragma once
#include "Colors.hpp"

#define PRINT(condition, color, statement) \
    if (condition) { \
        std::cout << color << statement << RESET << std::endl; \
    }

#define HTTPREQUEST		0
#define TEXTRESPONSE	0
#define SERVERMANAGER	1
#define SOCKET			1
