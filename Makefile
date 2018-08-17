NAME = test
SRCS = shell.c
HDRS = 21sh.h
LIB = libft/libft.a
OBJS = $(SRCS:.c=.o)
FLAGS = -ltermcap -lcurses -g3

all:	$(NAME)

$(NAME): $(OBJS) $(LIB) $(HDRS)
	gcc -o $(NAME) $(FLAGS) $(OBJS) -L. $(LIB)

$(OBJ):	$(SRCS) $(LIB)
	gcc -c $(SRCS) -g3

#$(NAME): $(SRCS)
#	gcc -o $(NAME) -L. $(LIB) $(FLAGS) $(SRCS) 

$(LIB):
	make -C libft

clean:
	/bin/rm -f $(OBJ)
	make clean -C libft

fclean: clean
	@/bin/rm -f $(NAME)
	make fclean -C libft

re: fclean all

