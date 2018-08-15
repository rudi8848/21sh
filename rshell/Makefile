NAME = rsh
SRC = rshell.c ec.c logf.c options.c suvreq.c macrostr.c \
extio.c statuspr.c builtin.c setenv.c syserr.c
OBJ = $(SRC:.c=.o)
all: $(NAME)

$(NAME): $(OBJ)
	gcc -o $(NAME)  $(OBJ) -lncurses -ltermcap

$(OBJ): $(SRC)
	gcc -c $(SRC) -g3

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
