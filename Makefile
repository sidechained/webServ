FILES = main.cpp \
        Requests/HttpRequest.cpp \
		Requests/HttpError.cpp \
        Servers/Server.cpp \
        Sockets/Socket.cpp \
		Servers/ServerManager.cpp \
		Responses/ResponseFactory.cpp \
        Responses/TextResponse.cpp \
		Responses/RedirResponse.cpp \
		Responses/ErrResponse.cpp \
        Responses/SimpleResponse.cpp \
		Responses/FormResponse.cpp \
		Responses/Headers/SimpleHeader.cpp \
		Responses/Headers/MovedHeader.cpp \
		Responses/Headers/OkHeader.cpp \
		Parser/GenericParser.cpp \
		Parser/LocationParser.cpp \
		Parser/ServerParser.cpp \
		Parser/ConfigFileParser.cpp \
		utils/ContentTypeUtils.cpp \
		utils/LoadMapFromFile.cpp \
		utils/StatusCodeUtils.cpp \
		utils/RemoveNonPrintable.cpp 

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
NAME = server
OBJ_DIR = obj
OBJECTS = $(addprefix $(OBJ_DIR)/, $(FILES:.cpp=.o))

YELLOW		=	\e[33;1m
RESET		=	\e[0m

all: $(NAME)

$(NAME): $(OBJECTS)
	@echo "🚀 Building $(NAME)..."
	@mkdir -p $(@D)
	@$(CC) $(OBJECTS) -o $(NAME)
	@echo "✅ Build successful!"

$(OBJ_DIR)/%.o: %.cpp 
#$(OBJ_DIR)/%.o: %.cpp %.hpp
	@echo "🛠️ Compiling $<..."
	@mkdir -p $(@D)
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo "🧹 Cleaning object files..."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "🗑️ Cleaning $(NAME)..."
	@rm -f $(NAME)

re: fclean all

launch: all
	@echo "🚀 Launching $(NAME)..."
	@./$(NAME)

test: curlResolve1 curlResolve2 curlGet1 curlGet2 curlGet3 curlGetBadPort curlDelete curlPostSizeUnder curlPostSizeOver curlUnknownMethod

curlResolve1:
	@echo  "\n$(YELLOW)curl --resolve example1.com:8080:$(RESET)\n" 
	curl --resolve example1.com:8080:127.0.0.1 http://example1.com:8080
	
curlResolve2:
	@echo  "\n$(YELLOW)curl --resolve example2.com:8080:$(RESET)\n"
	curl --resolve example2.com:8080:127.0.0.3 http://example2.com:8080

curlGet1:
	@echo  "\n$(YELLOW) simple page get request \n curl -X GET http://127.0.0.1:8080/simple.html $(RESET)\n"
	curl -X GET http://127.0.0.1:8080/simple.html

curlGet2:
	@echo  "\n$(YELLOW) same for a different page \n curl -X GET http://127.0.0.1:8080/form.html $(RESET)\n"
	curl -X GET http://127.0.0.1:8080/form.html

curlGet3:
	@echo  "\n$(YELLOW) same for a different page \n curl -X GET http://127.0.0.3:4545/upload.html $(RESET)\n"
	curl -X GET http://127.0.0.3:4545/upload.html

curlGetBadPort:
	@echo  "\n$(YELLOW) bad port - not part of server - $(RESET)\n"
	- curl -X GET http://127.0.0.3:4500/index1.html

curlDelete:
	@echo  "\n$(YELLOW) delete request \n curl -X DELETE - file created - $(RESET)\n"
	touch www/deleteTest.html
	curl -X DELETE http://127.0.0.1:8080/deleteTest.html

curlDeleteRepeat:
	@echo  "\n$(YELLOW) delete request \n curl -X DELETE $(RESET)\n"
	curl -X DELETE http://127.0.0.1:8080/deleteTest.html	

curlPostSizeUnder:
	@echo  "\n$(YELLOW) post request with content-length under 500 \n $(RESET)\n"
	# we only POST only with CGI, so using GET here
	curl -X GET -H "Content-Type: plain/text" -H "Content-Length: 500" --data-binary @input_file_over http://127.0.0.1:8080
	
curlPostSizeOver:
	@echo  "\n$(YELLOW) post request with content-length over 500 \n $(RESET)\n"
	# we only POST only with CGI, so using GET here
	curl -X GET -H "Content-Type: plain/text" -H "Content-Length: 50000" --data-binary @input_file_over http://127.0.0.1:8080

curlUnknownMethod:
	@echo  "\n$(YELLOW) unknown method \n $(RESET)\n"
	curl -X UNKNOWN http://127.0.0.1:8080/simple.html
	
.PHONY: all clean fclean re launch curlResolve1 curlResolve2 curlGet1 curlGet2 curlGet3 curlGetBadPort curlDelete curlPostSizeUnder curlPostSizeOver curlUnknownMethod




