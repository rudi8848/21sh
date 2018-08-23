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
# include <termios.h>
# include <curses.h>
# include <term.h>
# include "libft/includes/libft.h"
# include "libft/includes/ft_printf.h"
# define CHILD 0
# define PARENT 1
# define ERROR -1


# define TRUE 1
# define FALSE 0

# define COMPLETED 1		//for state in t_process
# define STOPPED 2		//for state in t_process

# define TERM_BELL	tputs(tgetstr("bl", NULL), 0, ft_iputchar);
# define TERM_BACK	tputs(tgetstr("le", NULL), 0, ft_iputchar);\  //1 position to left
                	tputs(tgetstr("dm", NULL), 0, ft_iputchar);\  //turn on deleting mode
                	tputs(tgetstr("dc", NULL), 0, ft_iputchar);\  //delete 1 char on cursor position
                	tputs(tgetstr("ed", NULL), 0, ft_iputchar);  // turn off deleting mode

# define TERM_DEL	tputs(tgetstr("dm", NULL), 0, ft_iputchar);\      //turn on deleting mode
                	tputs(tgetstr("dc", NULL), 0, ft_iputchar);\      //delete 1 char on cursor position
                	tputs(tgetstr("ed", NULL), 0, ft_iputchar); 

# define TERM_END tputs(tgoto(tgetstr("RI", NULL), 0, len - i), 0, ft_iputchar);
# define TERM_HOME tputs(tgoto(tgetstr("LE", NULL), 0, i), 0, ft_iputchar);
# define TERM_CRS_RIGHT tputs(tgetstr("nd", NULL), 0, ft_iputchar);
# define TERM_CRS_LEFT	tputs(tgetstr("le", NULL), 0, ft_iputchar);


typedef int t_bool;

typedef enum {
	T_WORD,
	T_PIPE,
	T_BG,
	T_SEMI,
	T_GREAT,
	T_GGREAT,
	T_LESS,
	T_LLESS,
	T_NLINE,
	T_EOF,
	T_ERROR
} t_token;

typedef enum {
	PLANE,
	GGREAT,
	INQUOTE,
	INWORD
} t_state;

/*
	1) структура с глобальной таблицей команд
	2) структура с каждой отдельной командой (список? массив?(доступ по инд.))
		
*/
/*
typedef struct s_cmd
{
	char **cmd_args;
	struct s_cmd *next;
} t_cmd;

typedef struct s_cmds
{
	char *infile;
	int in_fd;		//fd of input file
	char *outfile;
	int out_fd;
	char *errfile;
	int err_fd;
	int bg;			// background & 

	int nbr_of_cmds;
	char ***cmd_list;
	
} t_cmds;
*/

typedef struct s_process
{
	struct s_process *next;
	char **argv;
	pid_t pid;
	uint8_t state;	// COMPLETED | STOPPED
	int status;
} t_process;

typedef struct s_job
{
	struct s_job *next;
	char *command;
	t_process *first_process;
	pid_t pgid;
	uint8_t notified;
	int in_fd;
	int out_fd;
	int err_fd;
	struct termios tmodes;
} t_job;

t_token ft_gettoken(char *line, int *i,char *word, size_t maxword);

#endif
