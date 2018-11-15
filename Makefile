NAME 	=	test
SRCS 	= 	builtin.c \
			builtin_cd.c \
			builtin_env.c \
			builtin_setenv.c \
			environement.c \
			find.c \
			help_func.c \
			heredoc.c \
			is_builtin.c \
			jobs_builtin.c \
			lexer.c \
			lexer_help.c \
			lexer_insert_variable.c \
			lexer_plane.c \
			lexer_quotes.c \
			lexer_word.c \
			pack.c \
			pack_check_tokens.c \
			pack_end.c \
			pack_redirection.c \
			pack_redirection_help.c \
			read_line.c \
			read_line_autocomplete.c \
			read_line_autocomp_help.c \
			read_line_copy_paste.c \
			read_line_delete.c \
			read_line_help.c \
			read_line_highlight.c \
			read_line_history.c \
			read_line_jump.c \
			read_line_move.c \
			read_line_move_border.c \
			read_line_print.c \
			read_line_vertical.c \
			shell_help.c \
			shell_init.c \
			shell_job_control.c \
			shell_job_control1.c \
			shell_launch_job.c \
			shell_launch_job1.c \
			shell_launch_process.c \
			shell_main.c \
			shell_process_status.c \
			shell_prompt.c \
			shell_signals.c
HDRS	= 	shell.h shell_structures.h
LIB 	= 	libft/libft.a
OBJS 	= 	$(SRCS:.c=.o)
FLAGS 	= 	-Wall -Wextra -Werror

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

