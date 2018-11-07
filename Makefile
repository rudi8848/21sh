NAME = test
SRCS = shell.c lexer.c pack.c \
		builtin.c built_func.c environement.c find.c help_func.c heredoc.c job_builtin.c \
		read_line.c
HDRS = 21sh.h
LIB = libft/libft.a
OBJS = $(SRCS:.c=.o)
FLAGS = -Wall -Wextra -Werror

all:	$(NAME)

$(NAME): $(OBJS) $(LIB) $(HDRS)
	gcc -o $(NAME) $(OBJS) -L. $(LIB) $(FLAGS) -lncurses -ltermcap

$(OBJS):	$(SRCS) $(LIB)
	gcc -g3 -c $(SRCS) $(FLAGS)

$(LIB):
	make -C libft

clean:
	/bin/rm -f $(OBJS)
	make clean -C libft

clear history:	.history
	/bin/rm .history

fclean: clean
	@/bin/rm -f $(NAME)
	make fclean -C libft

re: fclean all

