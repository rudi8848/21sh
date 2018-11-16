NAME 	=	21sh
SRCS 	= 	src/builtin.c \
			src/builtin_cd.c \
			src/builtin_env.c \
			src/builtin_setenv.c \
			src/environement.c \
			src/find.c \
			src/help_func.c \
			src/heredoc.c \
			src/is_builtin.c \
			src/jobs_builtin.c \
			src/lexer.c \
			src/lexer_help.c \
			src/lexer_insert_variable.c \
			src/lexer_plane.c \
			src/lexer_quotes.c \
			src/lexer_word.c \
			src/pack.c \
			src/pack_check_tokens.c \
			src/pack_end.c \
			src/pack_redirection.c \
			src/pack_redirection_help.c \
			src/read_line.c \
			src/read_line_autocomplete.c \
			src/read_line_autocomp_help.c \
			src/read_line_autocomp_read.c \
			src/read_line_copy_paste.c \
			src/read_line_delete.c \
			src/read_line_help.c \
			src/read_line_highlight.c \
			src/read_line_history.c \
			src/read_line_jump.c \
			src/read_line_move.c \
			src/read_line_move_border.c \
			src/read_line_print.c \
			src/read_line_vertical.c \
			src/shell_help.c \
			src/shell_init.c \
			src/shell_job_control.c \
			src/shell_job_control1.c \
			src/shell_launch_job.c \
			src/shell_launch_job1.c \
			src/shell_launch_process.c \
			src/shell_main.c \
			src/shell_process_status.c \
			src/shell_prompt.c \
			src/shell_signals.c
HDRS	= 	includes/shell.h includes/shell_structures.h
LIB 	= 	libft/libft.a
OBJS 	= 	$(SRCS:.c=.o)
CC		=	gcc
FLAGS 	= 	-Wall -Wextra -Werror
DIR_SRC = 	srcs

all:	$(NAME)

$(NAME): $(LIB) $(OBJS) $(HDRS)
	$(CC) -g3 -o $(NAME) $(OBJS) -L. $(LIB) $(FLAGS) -lncurses -ltermcap

$(OBJS): $(SRCS)
	$(CC) -g3 $(FLAGS) $(SRCS)

$(LIB):
	@make -C libft

clean:
	/bin/rm -f $(OBJS)
	make clean -C libft

fclean: clean
	/bin/rm -f $(NAME)
	make fclean -C libft

re: fclean all

.PHONY: clean fclean all re