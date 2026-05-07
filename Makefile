NAME			=
CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98
DEBUG			?=	0
CPPFLAGS	=	-Iincludes -DDEBUG=$(DEBUG)

SRC			=	
OBJS			=	$(SRC:.cpp=.o)

RM			=	rm -f

all:	$(NAME)

debug:	DEBUG = 1
debug:	re

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "$(NAME) compiled"

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJS)

fclean:	clean
	@$(RM) $(NAME)

re:	fclean all

.PHONY:	all debug clean fclean re