NAME = test
SRCS = shell.c lexer.c pack.c
HDRS = 21sh.h
LIB = libft/libft.a
OBJS = $(SRCS:.c=.o)
FLAGS = -lncurses -ltermcap

all:	$(NAME)

$(NAME): $(OBJS) $(LIB) $(HDRS)
	gcc -o $(NAME) $(OBJS) -L. $(LIB) $(FLAGS)

$(OBJ):	$(SRCS) $(LIB)
	gcc -g3 -c $(SRCS)

#$(NAME): $(SRCS)
#	gcc -o $(NAME) -L. $(LIB) $(FLAGS) $(SRCS) 

$(LIB):
	make -C libft

clean:
	/bin/rm -f $(OBJS)
	make clean -C libft

fclean: clean
	@/bin/rm -f $(NAME)
	make fclean -C libft

re: fclean all

