NAME = test
SRCS = shell.c lexer.c pack.c \
		builtin.c built_func.c environement.c find.c help_func.c heredoc.c job_builtin.c
HDRS = 21sh.h
LIB = libft/libft.a
OBJS = $(SRCS:.c=.o)
FLAGS = -lncurses -ltermcap

all:	$(NAME)

$(NAME): $(OBJS) $(LIB) $(HDRS)
	gcc -o $(NAME) $(OBJS) -L. $(LIB) $(FLAGS)

$(OBJS):	$(SRCS) $(LIB)
	gcc -g3 -c $(SRCS)

$(LIB):
	make -C libft

left: left.c $(SRCS) $(LIB)
	gcc -g3 -c left.c shell.c
	gcc -o left left.o shell.o -L. $(LIB) $(FLAGS)

clean:
	/bin/rm -f $(OBJS)
	make clean -C libft

clear history:	.history
	/bin/rm .history

fclean: clean
	@/bin/rm -f $(NAME)
	make fclean -C libft

re: fclean all

