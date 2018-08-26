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

#define MAXARG 50
#define MAXFILENAME 500
#define MAXWORD 500

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
	struct termios tmodes;
} t_job;

t_token ft_gettoken(char *line, int *i,char *word, size_t maxword);
int	pack_args(char *line, t_job **j);
int     cbreak_settings();
void    read_line(char *line, int start);
void    ft_restore();
#endif
