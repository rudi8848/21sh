/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   21sh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/27 13:16:57 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 15:59:35 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_H
# define SHELL_H
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <signal.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/ioctl.h>
# include <termios.h>
# include <dirent.h>
# include <curses.h>
# include <term.h>
# include <errno.h>
# include "libft.h"
# include "shell_structures.h"
# include "ft_printf.h"
# include "get_next_line.h"

# define CHILD 			0
# define PARENT 		1

# define ERROR 			-1
# define NORM 			0
# define MUTE 			1

# define RETURN 		-1
# define CONTINUE 		-2

# define COMPLETED 		1
# define STOPPED 		2

# define RESET 			"\033[0m"
# define GREEN 			"\033[1;32m"
# define RED 			"\033[1;31m"
# define BLUE			"\033[0;34m"

# define MAXHSTR 		4096

# define MAXLINE 		1024

# define K_LEFT  		4479771
# define K_RIGHT		4414235
# define K_UP     	 	4283163
# define K_DOWN     	4348699
# define K_CTRL_LEFT    74995417045787
# define K_CTRL_RIGHT   73895905418011
# define K_CTRL_UP		71696882162459
# define K_CTRL_DOWN	72796393790235
# define K_CTRL_C		3
# define K_CTRL_D		4

# define K_ENTER		10
# define K_TAB      	9
# define K_SPACE    	32
# define K_ESC      	27
# define K_DELETE   	2117294875L
# define K_BSPACE   	127
# define K_HOME			4741915
# define K_END			4610843
# define K_SHFT_L		74982532143899
# define K_SHFT_R		73883020516123

# define K_ALT_C		42947
# define K_ALT_V		10127586
# define K_ALT_X		8948194

# define IN_MEMORY		0
# define ON_SCREEN		1

# define FILE_PERM S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

char					**g_envp;
pid_t					g_shell_pgid;
int						g_shell_terminal;
int						g_shell_is_interactive;
int						g_hstr_nb;
char 					*g_history_file;
char					*g_history[MAXHSTR];
struct termios			g_saved;
int						g_hstr_fd;
t_job					*g_first_job;

/*
**		read_line
*/
void		read_line(char *line, int start, char *eol);
void		init_position(t_cpos *pos, int start, char *line, char *eol);
void		print(char *line, t_cpos *pos, uint64_t rb, int rr);
void		ft_highlight(uint64_t rb, char *line, t_cpos *pos);
void		ft_copy_paste(uint64_t rb, char *line, t_cpos *pos);
void		move_history(uint64_t rb, char *line, t_cpos *pos, int *cmd);
int		delete_char(uint64_t rb, char *line, t_cpos *pos);
void		ft_move(uint64_t direction, char *line, t_cpos *pos);
void		move_to_border(uint64_t direction, char *line, t_cpos *pos);
void		ft_jump(uint64_t direction, char *line, t_cpos *pos);
void		ft_jump_vertical(uint64_t direction, char *line, t_cpos *pos);
void		reset_selection(t_cpos *pos, char *line);
void		ft_autocomplete(char *line, t_cpos *pos);
t_compl		*sort_list(t_compl *a, t_compl *head);
void		complete(char *line, t_cpos *pos, char *begin);
void		push_compl(t_compl **head, char *name);
void		clear_compl(t_compl **head);
int			ft_read_dir(t_compl **head, char *name, char *begin, char dironly);
int			read_path(t_compl **head, char *begin);
void		move_right(t_cpos *pos, int mode);
void		move_left(t_cpos *pos, int mode);

/*
**	shell
*/
int			cbreak_settings(void);
void		ft_restore(void);
void		free_job(t_job *j);
void		ft_exit(void);
void		check_history_capacity(void);
int			ft_iputchar(int c);
void		init_launch(t_launch *launch, int *mypipe);
void		init_terminal(void);
void		append_history(char *line);
void		init_history(void);
void		init_shell(void);
void		do_job_notification(void);
int			job_is_completed(t_job *j);
int			job_is_stopped(t_job *j);
void		put_job_in_foreground(t_job *j, int cont);
void		put_job_in_background(t_job *j, int cont);
void		wait_for_job(t_job *j);
void		continue_job(t_job *j, int foreground);
void		launch_job(t_job *j, int foreground);
void		make_child_process(t_job *j, t_launch *launch, int foreground);
void		do_builtin(t_job *j, t_launch *launch, int *mypipe);
void		set_job_number(t_job *j);
void		check_fd(t_job *j);
void		launch_process(t_launch *launch, pid_t pgid, int errfile, int fg);
int			mark_process_status(pid_t pid, int status);
void		update_status(void);
int			type_prompt(void);
void		set_stopsignals(sig_t func);
void		sig_tstp_handler(int signum);
int 		is_empty(char *line);
/*
**	pack
*/
int			pack_args(char *line, t_job *j);
int			check_tokens(t_job **j, t_process **p, char *line, t_pack *pack);
int			pack_great(t_job *j, t_process *p, char *line, t_pack *pack);
int			close_output(t_job *j, t_process *p, t_pack *pack);
int			pack_end(t_job **j, t_process **p, char *line, t_pack *pack);
int			pack_word(t_process *p, t_pack *pack);
int			remove_invalid_job(t_job *j, t_process *p, t_pack *pack);
void		init_job(t_job *j);
/*
**	lexer
*/
t_token		ft_gettoken(char *line, int *i, char *word, size_t maxword);
int			manage_plane_inword(char *line, int *i, t_lex *lex);
int			plane_inword(char *line, int *i, t_lex *lex);
int			word_tokens(char *line, int *i, t_lex *lex);
void		insert_variable(char *line, char *word, int *i, size_t *wordn);
int			dquote_tokens(char *line, int *i, t_lex *lex);
int			quote_tokens(char *line, int *i, t_lex *lex);
void		read_more(char *line, int i, char *prompt, char *eol);
int			store_char(char *word, size_t maxword, int c, size_t *np);
/*
**	find
*/
int			ft_find(t_process *commands);
/*
**	jobs builtin
*/
int			ft_jobs(char **argv, int infile, int outfile);
int			ft_kill(char **args, int infile, int outfile);
int			ft_fg(char **args, int infile, int outfile);
int			ft_bg(char **args, int infile, int outfile);
/*
**	builtin env
*/
int			ft_unsetenv(char **args, int infile, int outfile);
int			ft_env(char **args, int infile, int outfile);
/*
**	is builtin
*/
int			check_built(char **cmd);
void		ft_built_exe(char **args, t_built cmd, int infile, int outfile);
/*
**	builtin cd
*/
int			ft_cd(char **args, int infile, int outfile);
/*
**	builtin
*/
int			ft_echo(char **argv, int infile, int outfile);
int			b_exit(char **args, int infile, int outfile);
/*
**	builtin setenv
*/
int			ft_setenv(char **args, int infile, int outfile);
int			env_size(char **envp);
/*
**	environement
*/
char		*get_copy_env(char *needle, int warning);
void		copy_env(void);
/*
**	help func
*/
char		**ft_cp_array(char **src);
void		free_arr(char **array);
char		*get_current_wd(void);
/*
**	heredoc
*/
int			ft_heredoc(char **argv, int infile, int outfile);

#endif
