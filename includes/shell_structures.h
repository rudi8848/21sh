/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_structures.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 15:52:20 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/29 11:06:57 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_STRUCTURES_H
# define SHELL_STRUCTURES_H
# define MAXARG 		50
# define MAXFILENAME 	500
# define MAXWORD 		500

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
}	t_token;

typedef enum {
	PLANE,
	GGREAT,
	INQUOTE,
	INWORD,
	INDQUOTE
}	t_state;

typedef struct			s_process
{
	struct s_process	*next;
	char				*argv[MAXARG];
	pid_t				pid;
	int					in_fd;
	int					out_fd;
	uint8_t				state;
	int					status;
}						t_process;

typedef struct			s_job
{
	int					nbr;
	struct s_job		*next;
	char				*command;
	t_process			*first_process;
	pid_t				pgid;
	uint8_t				notified;
	int					in_fd;
	int					out_fd;
	int					err_fd;
	char				srcfile[MAXFILENAME];
	char				dstfile[MAXFILENAME];
	char				errfile[MAXFILENAME];
	struct termios		tmodes;
	int					flags;
	int					foreground;
}						t_job;

typedef struct			s_compl
{
	char				*name;
	char				active;
	struct s_compl		*next;
}						t_compl;

typedef struct			s_cpos
{
	int					prompt_len;
	int					startline;
	int					curx;
	int					cury;
	int					width;
	int					curln;
	int					height;
	int					len;
	int					i;
	int					selection;
	int					first;
	int					last;
	t_compl				*autocompl;
	char				*bgn;
	int					autostart;
	int					autolen;
	int					is_auto;
}						t_cpos;

typedef	int	(*t_pfb) (char**, int, int);

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
	B_KILLJ,
	B_END,
	BUILT
}	t_built;

typedef struct			s_bset
{
	char				*command;
	t_pfb				function;
}						t_bset;

typedef struct			s_lex
{
	t_state				state;
	size_t				wordn;
	t_token				ret_token;
	char				*word;
	size_t				maxword;
}						t_lex;

typedef struct			s_pack
{
	t_token				token;
	t_token				tkn;
	char				word[MAXWORD];
	int					argc;
	int					makepipe;
	int					i;
}						t_pack;

typedef struct			s_launch
{
	t_process			*p;
	int					infile;
	int					outfile;
	pid_t				pid;
	int					ret;
}						t_launch;
#endif
