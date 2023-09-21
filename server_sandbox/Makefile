FILES = main.cpp \
        Servers/SimpleServer.cpp \
        Servers/PollingServer.cpp \
        Sockets/SimpleSocket.cpp \
        Sockets/ListeningSocket.cpp \
        Requests/HttpRequest.cpp \
        Responses/TextResponse.cpp \
        Responses/SimpleResponse.cpp \
		Responses/Headers/SimpleHeader.cpp \
		Responses/Headers/MovedHeader.cpp \
		Responses/Headers/OkHeader.cpp \
		Parser/GenericConfig.cpp \
		Parser/LocationConfig.cpp \
		Parser/ServerConfig.cpp \
		Parser/ConfigFileParser.cpp \
		utils/ContentTypeUtils.cpp \
		utils/LoadMapFromFile.cpp \
		utils/StatusCodeUtils.cpp \
		utils/RemoveNonPrintable.cpp \

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
