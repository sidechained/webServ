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

.PHONY: all clean fclean re
