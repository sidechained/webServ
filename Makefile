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

curlResolve1:
	curl --resolve example1.com:8080:127.0.0.1 http://example1.com:8080
	
curlResolve2:
	curl --resolve example2.com:8080:127.0.0.3 http://example2.com:8080

curlGet1:
	curl -X GET http://127.0.0.1:8080/simple.html

curlGet2:
	curl -X GET http://127.0.0.1:8080/form.html

curlGet3:
	curl -X GET http://127.0.0.3:4545/upload.html

curlGetBadPort:
	curl -X GET http://127.0.0.3:4500/index1.html

curlDelete:
	touch www/deleteTest.html
	curl -X DELETE http://127.0.0.1:8080/deleteTest.html

curlDeleteRepeat:
	curl -X DELETE http://127.0.0.1:8080/deleteTest.html	

curlPostSizeUnder:
	# we only POST only with CGI, so using GET here
	curl -X GET -H "Content-Type: plain/text" -H "Content-Length: 500" --data-binary @input_file_over http://127.0.0.1:8080
	
curlPostSizeOver:
	# we only POST only with CGI, so using GET here
	curl -X GET -H "Content-Type: plain/text" -H "Content-Length: 50000" --data-binary @input_file_over http://127.0.0.1:8080

curlUnknownMethod:
	curl -X UNKNOWN http://127.0.0.1:8080/simple.html
	
.PHONY: all clean fclean re launch curlResolve1 curlResolve2 curlGet1 curlGet2 curlGet3 curlGetBadPort curlDelete curlPostSizeUnder curlPostSizeOver curlUnknownMethod




