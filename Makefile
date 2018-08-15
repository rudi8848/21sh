NAME = test
SRCS = lexer.c
HDR = 21sh.h
all: $(NAME)

$(NAME): $(SRCS) $(HDR)
	gcc -o $(NAME) -g3 -ltermcap -lcurses  $(SRCS)

clean:
	rm $(NAME)

re: clean all

