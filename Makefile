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

HOME = /nfs/homes/gbooth/42cursus/5_3_webserv_repo

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

siege_install:
	wget http://download.joedog.org/siege/siege-3.1.4.tar.gz
	tar -xzvf siege-3.1.4.tar.gz
	cd siege-3.1.4 && ./configure --prefix=$(HOME)/siege
	cd siege-3.1.4 && make
	cd siege-3.1.4 && make install
	rm -r siege-3.1.4
	rm siege-3.1.4.tar.gz
	mkdir siege/var

siege_uninstall:
	rm -r siege

siege_run:
	./siege/bin/siege -b -c 10 -t 60s http://127.0.0.1:8080

curl_all: curl_resolve1 curl_resolve2 curl_get1 curl_get2 curl_get3 curl_get_bad_port curl_delete curl_post_size_under curl_post_size_over curl_unknown_method

curl_resolve1:
	@echo  "\n$(YELLOW)curl --resolve example1.com:8080:$(RESET)\n" 
	curl --resolve example1.com:8080:127.0.0.1 http://example1.com:8080
	
curl_resolve2:
	@echo  "\n$(YELLOW)curl --resolve example2.com:8080:$(RESET)\n"
	curl --resolve example2.com:8080:127.0.0.3 http://example2.com:8080

curl_get1:
	@echo  "\n$(YELLOW) simple page get request \n curl -X GET http://127.0.0.1:8080/simple.html $(RESET)\n"
	curl -X GET http://127.0.0.1:8080/simple.html

curl_get2:
	@echo  "\n$(YELLOW) same for a different page \n curl -X GET http://127.0.0.1:8080/form.html $(RESET)\n"
	curl -X GET http://127.0.0.1:8080/form.html

curl_get3:
	@echo  "\n$(YELLOW) same for a different page \n curl -X GET http://127.0.0.3:4545/upload.html $(RESET)\n"
	curl -X GET http://127.0.0.3:4545/upload.html

curl_get_bad_port:
	@echo  "\n$(YELLOW) bad port - not part of server - $(RESET)\n"
	- curl -X GET http://127.0.0.3:4500/index1.html

curl_delete:
	@echo  "\n$(YELLOW) delete request \n curl -X DELETE - file created - $(RESET)\n"
	touch www/deleteTest.html
	curl -X DELETE http://127.0.0.1:8080/deleteTest.html

curl_delete_repeat:
	@echo  "\n$(YELLOW) delete request \n curl -X DELETE $(RESET)\n"
	curl -X DELETE http://127.0.0.1:8080/deleteTest.html	

curl_post_size_under:
	@echo  "\n$(YELLOW) post request with content-length under 500 \n $(RESET)\n"
	# we only POST only with CGI, so using GET here
	curl -X GET -H "Content-Type: plain/text" -H "Content-Length: 500" --data-binary @input_file_over http://127.0.0.1:8080
	
curl_post_size_over:
	@echo  "\n$(YELLOW) post request with content-length over 500 \n $(RESET)\n"
	# we only POST only with CGI, so using GET here
	curl -X GET -H "Content-Type: plain/text" -H "Content-Length: 50000" --data-binary @input_file_over http://127.0.0.1:8080

curl_unknown_method:
	@echo  "\n$(YELLOW) unknown method \n $(RESET)\n"
	curl -X UNKNOWN http://127.0.0.1:8080/simple.html
	
.PHONY: all clean fclean re launch siege_install siege_uninstall siege_run curl_all curl_resolve1 curl_resolve2 curl_get1 curl_get2 curl_get3 curl_get_bad_port curl_delete curl_post_size_under curl_post_size_over curl_unknown_method




