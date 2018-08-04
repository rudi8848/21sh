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
# include <fcntl.h>
# include <sys/wait.h>

# define CHILD 0
# define PARENT 1
# define ERROR -1


# define TRUE 1
# define FALSE 0

typedef int t_bool;
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
*/
typedef struct s_cmds
{
	char *infile;
	int in_fd;		/* fd of input file*/
	char *outfile;
	int out_fd;
	char *errfile;
	int err_fd;
	int bg;			/* background & */

	int nbr_of_cmds;
	char ***cmd_list;
	
} t_cmds;

#endif
