NAME			= ircserv
CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98
DEBUG			?=	0
CPPFLAGS	=	-Iincludes -DDEBUG=$(DEBUG)

RESET		= \033[0m
RED			= \033[0;31m
GREEN		= \033[0;32m
YELLOW		= \033[0;33m

SRC			=	./main.cpp \
				./src/Channel.cpp \
				./src/Client.cpp \
				./src/CommandHandler.cpp \
				./src/Server.cpp

OBJ_DIR		=	obj
OBJS			=	$(OBJ_DIR)/main.o \
				$(OBJ_DIR)/Channel.o \
				$(OBJ_DIR)/Client.o \
				$(OBJ_DIR)/CommandHandler.o \
				$(OBJ_DIR)/Server.o

RM			=	rm -f

all:	$(NAME)

debug:	DEBUG = 1
debug:	re

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled$(RESET)"

$(OBJ_DIR)/main.o: ./main.cpp | $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: ./src/%.cpp | $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(YELLOW)Cleaned proyect$(RESET)"

fclean:	clean
	@$(RM) $(NAME)
	@echo "$(RED)Fully cleaned $(NAME)$(RESET)"

re:	fclean all

.PHONY:	all debug clean fclean re