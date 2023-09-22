NAME			= cgiUploader
SRCS			= CGIUploader.cpp\
				../configFileParser/ConfigFileParser.cpp\
				../configFileParser/ServerParser.cpp\
				../configFileParser/LocationParser.cpp\
				../configFileParser/GenericParser.cpp\
				../postUploadRequestParser/PostUploadRequestParser.cpp\
				testMain.cpp
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
				./$(NAME) testFiles/serverConfig.txt testFiles/postRequest.txt tmp/output.txt

.PHONY:			all clean fclean re test