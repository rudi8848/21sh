/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   21sh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/27 13:16:57 by gvynogra          #+#    #+#             */
/*   Updated: 2018/07/27 13:17:00 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _21SH_H_
# define _21SH_H_
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <signal.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/ioctl.h>
# include <termios.h>
# include <curses.h>
# include <term.h>
# include "libft/includes/libft.h"
# include "libft/includes/ft_printf.h"
# include "libft/includes/get_next_line.h"
# define CHILD 0
# define PARENT 1
# define ERROR -1

//# define OK 0
# define MUTE 1

# define TRUE 1
# define FALSE 0

# define COMPLETED 1		//for state in t_process
# define STOPPED 2		//for state in t_process


# define RESET "\033[0m"
# define GREEN "\033[1;32m"
# define RED "\033[1;31m"
# define BLUE	"\033[0;34m"

char		**g_envp;
	pid_t	shell_pgid;
	int		shell_terminal;
	int		shell_is_interactive;

typedef int t_bool;

typedef enum {
	T_WORD,
	T_PIPE,
	T_BG,
	T_SEMI,
	T_GREAT,
	T_GGREAT,
	T_LESS,
	T_HRDOC,
	T_NLINE,
	T_EOF,
	T_ERROR
} t_token;

typedef enum {
	PLANE,
	GGREAT,
	INQUOTE,
	INWORD,
	INDQUOTE
} t_state;

#define MAXARG 			50
#define MAXFILENAME 	500
#define MAXWORD 		500

#define MAXLINE 		500

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
# define K_ALT_C		42947		//for copy
# define K_ALT_V		10127586	//for paste

# define TERM_BELL	tputs(tgetstr("bl", NULL), 0, ft_iputchar);
# define TERM_BACK	tputs(tgetstr("le", NULL), 0, ft_iputchar);\
                	tputs(tgetstr("dm", NULL), 0, ft_iputchar);\
                	tputs(tgetstr("dc", NULL), 0, ft_iputchar);\
                	tputs(tgetstr("ed", NULL), 0, ft_iputchar);

# define TERM_DEL	tputs(tgetstr("dm", NULL), 0, ft_iputchar);\
                	tputs(tgetstr("dc", NULL), 0, ft_iputchar);\
                	tputs(tgetstr("ed", NULL), 0, ft_iputchar); 

# define TERM_END tputs(tgoto(tgetstr("RI", NULL), 0, len - i), 0, ft_iputchar);
# define TERM_HOME tputs(tgoto(tgetstr("LE", NULL), 0, i), 0, ft_iputchar);
# define TERM_CRS_RIGHT tputs(tgetstr("nd", NULL), 0, ft_iputchar);
# define TERM_CRS_LEFT	tputs(tgetstr("le", NULL), 0, ft_iputchar);
# define FILE_PERM S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
typedef struct s_process
{
	struct s_process *next;
	char *argv[MAXARG];
	pid_t pid;
	int in_fd;
	int out_fd;
	uint8_t state;	// COMPLETED | STOPPED
	int status;
} t_process;

typedef struct s_job
{
	int nbr;
	struct s_job *next;
	char *command;
	t_process *first_process;
	pid_t pgid;
	uint8_t notified;
	int in_fd;
	int out_fd;
	int err_fd;
	char srcfile[MAXFILENAME];
	char dstfile[MAXFILENAME];
	char errfile[MAXFILENAME];
	struct termios tmodes;
	int flags;
	int foreground;
} t_job;

typedef	int	(*t_pfb) (char**, int , int);

typedef enum {
	B_ECHO,
	B_CD,
	B_SETENV,
	B_UNSETENV,
	B_ENV,
	B_HRDOC,
	B_EXIT,
	B_JOBS,
	B_BG,
	B_FG,
	B_END,
	BUILT
}	t_built;
t_job	*first_job;
t_token ft_gettoken(char *line, int *i,char *word, size_t maxword);
int	pack_args(char *line, t_job *j);
int     cbreak_settings();
void    read_line(char *line, int start);
void    ft_restore();
int ft_iputchar(int c);
void	copy_env(void);
void	ft_exit(void);
void	init_job(t_job *j);
void	read_more(char *line, int i, char *prompt);
int		ft_heredoc(char **argv, int infile, int outfile);


void	wait_for_job(t_job *j);
int	job_is_stopped(t_job *j);
int	job_is_completed(t_job *j);
void	print_processes(t_job *j);
void	put_job_in_foreground(t_job *j, int cont);
void	put_job_in_background(t_job *j, int cont);
//------------------------------

int		ft_find(t_process *cmd);
char	*get_copy_env(char *needle,  int warning);
int		ft_cd(char **args, int infile, int outfile);
int		ft_echo(char **args, int infile, int outfile);
int		b_exit(char **args, int infile, int outfile);
int		ft_setenv(char **args, int infile, int outfile);
int		ft_unsetenv(char **args, int infile, int outfile);
int		ft_env(char **args, int infile, int outfile);
int		b_exit(char **args, int infile, int outfile);
int		env_size(char **envp);
char			*get_current_wd(void);

char			*ft_path_substitute(char *path);

void			ft_move_env(int i, int find);

void			ft_change_env(char *new, char *old);
char			*ft_env_var(char *s);
int				ft_env_rewrite(char *str, int size);
int				ft_check_symb(char *s, int index);
int				check_built(char *cmd);
void			ft_cmd_exe(char **args, int infile, int outfile);
int				ft_print_env(char **args, int infile, int outfile);
int				ft_check_dir(char *name);
char			*ft_set_new_path(char **args);
char			**ft_cp_array(char **src);
void		free_arr(char **array);
void			type_prompt();
char	*ft_check_args(char **args);
void	ft_built_exe(char **args, t_built cmd, int infile, int outfile);
int		ft_jobs(char **args, int infile, int outfile);
int		ft_bg(char **args, int infile, int outfile);
int		ft_fg(char **args, int infile, int outfile);
//------------------------------

#endif
