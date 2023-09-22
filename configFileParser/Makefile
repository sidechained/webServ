NAME			= configFileParser
SRCS			= GenericParser.cpp LocationParser.cpp ServerParser.cpp ConfigFileParser.cpp testMain.cpp
OBJS			= $(SRCS:.cpp=.o)
CXX				= g++
CXXFLAGS		= -g -Wall -Wextra -Werror -std=c++98

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
				rm -f $(OBJS)

fclean:			clean
				rm -f $(NAME)

re:				fclean $(NAME)

run:		
				./$(NAME) testFiles/server1s.txt

.PHONY:			all clean fclean re test