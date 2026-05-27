NAME			= ircserv
CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -g
DEBUG			?=	0
CPPFLAGS	=	-Iincludes -DDEBUG=$(DEBUG)

RESET		= \033[0m
RED			= \033[0;31m
GREEN		= \033[0;32m
YELLOW		= \033[0;33m

SRC			= 	./main.cpp \
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
				./src/commands/QuitCommand.cpp \


OBJ_DIR		= 	obj
OBJS			= 	$(SRC:./%.cpp=$(OBJ_DIR)/%.o)

TOTAL_SRCS	= 	$(words $(SRC))


RM			=	rm -f

all:	$(NAME)

debug:	DEBUG = 1
debug:	re


$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled$(RESET)"

$(OBJ_DIR)/%.o: ./%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@idx=$$(i=0; for s in $(SRC); do i=$$((i+1)); if [ "$$s" = "$<" ]; then echo $$i; break; fi; done); \
	if [ -z "$$idx" ]; then \
		idx=$$(i=0; for s in $(SRC); do i=$$((i+1)); if [ "$$s" = "./$<" ]; then echo $$i; break; fi; done); \
	fi; \
	sh scripts/compile_progress.sh "$(CXX)" "$(CXXFLAGS)" "$(CPPFLAGS)" "$<" "$@" "$$idx" "$(TOTAL_SRCS)"

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