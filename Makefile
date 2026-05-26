NAME			= ircserv
CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -g
DEBUG			?=	0
CPPFLAGS	=	-Iincludes -DDEBUG=$(DEBUG)

RESET		= \033[0m
RED			= \033[0;31m
GREEN		= \033[0;32m
YELLOW		= \033[0;33m

SRC			=	./main.cpp \
				./src/Channel.cpp \
				./src/Client.cpp \
				./src/commands/CommandHandler.cpp \
				./src/Server.cpp \
				./src/commands/JoinCommand.cpp \
				./src/commands/PrivmsgCommand.cpp \
				./src/commands/KickCommand.cpp \
				./src/commands/InviteCommand.cpp \
				./src/commands/PassCommand.cpp \
				./src/commands/NickCommand.cpp \
				./src/commands/UserCommand.cpp \
				./src/commands/TopicCommand.cpp \
				./src/commands/ModeCommand.cpp \
				./src/commands/WhoCommand.cpp \


OBJ_DIR		=	obj
OBJS			=	$(OBJ_DIR)/main.o \
				$(OBJ_DIR)/Channel.o \
				$(OBJ_DIR)/Client.o \
				$(OBJ_DIR)/CommandHandler.o \
				$(OBJ_DIR)/Server.o \
				$(OBJ_DIR)/JoinCommand.o \
				$(OBJ_DIR)/PrivmsgCommand.o \
				$(OBJ_DIR)/KickCommand.o \
				$(OBJ_DIR)/InviteCommand.o \
				$(OBJ_DIR)/PassCommand.o \
				$(OBJ_DIR)/NickCommand.o \
				$(OBJ_DIR)/UserCommand.o \
				$(OBJ_DIR)/TopicCommand.o \
				$(OBJ_DIR)/ModeCommand.o \
				$(OBJ_DIR)/WhoCommand.o \


RM			=	rm -f

all:	$(NAME)

debug:	DEBUG = 1
debug:	re

$(NAME): build_objs
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled$(RESET)"

build_objs: $(OBJ_DIR) scripts/compile_progress.sh
	@sh scripts/compile_progress.sh $(CXX) "$(CXXFLAGS)" "$(CPPFLAGS)" $(OBJ_DIR) $(SRC)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(YELLOW)Cleaned proyect$(RESET)"

fclean:	clean
	@$(RM) $(NAME)
	@echo "$(RED)Fully cleaned $(NAME)$(RESET)"

re:	fclean all

.PHONY:	all debug clean fclean re build_objs