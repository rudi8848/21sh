/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_prompt.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 12:56:38 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 13:18:13 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	find_home(char **home, char **pwd)
{
	int		len;

	len = ft_strlen(*home);
	if (ft_strnequ(*home, *pwd, len))
	{
		*home = "~";
		*pwd += len; 
	}
	else
		*home = "";
}

static void	get_data(char **pwd, char **home)
{
	*home = get_copy_env("HOME", MUTE);
	if (home)
		find_home(home, pwd);
	else
		*home = "";
}

int			type_prompt(void)
{
	char	*user;
	char	*pwd;
	char	*home;
	int		totallen;
	char	*tmp;

	user = get_copy_env("LOGNAME", MUTE);
	if (!user)
		user = "";
	pwd = ft_strdup(get_current_wd());
	tmp = pwd;
	get_data(&pwd, &home);
	totallen = ft_strlen(user) + ft_strlen(home) + ft_strlen(pwd) ;
	ft_printf("%s%s: %s%s%s>%s ", RED, user, BLUE, home, pwd, RESET);
	free(tmp);
	return (totallen);
}
