NAME = test
SRCS = cmd_table.c
HDR = 21sh.h
all: $(NAME)

$(NAME): $(SRCS) $(HDR)
	gcc -o $(NAME) $(SRCS)

clean:
	rm $(NAME)
