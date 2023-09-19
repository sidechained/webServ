NAME			= postUploadRequestParser
SRCS			= PostUploadRequestParser.cpp testMain.cpp
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

.PHONY:			all clean fclean re test