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

static void	find_home(char **home, char **pwd, char **tmp)
{
	int		len;

	len = ft_strlen(*home);
	if (ft_strnequ(*home, *pwd, len))
	{
		*home = "~";
		*tmp = ft_strsub(*pwd, len, ft_strlen(*pwd) - len);
	}
}

int			type_prompt(void)
{
	char	*user;
	char	*pwd;
	char	*home;
	char	*tmp;
	int		totallen;

	tmp = NULL;
	user = get_copy_env("LOGNAME", MUTE);
	pwd = get_current_wd();
	home = get_copy_env("HOME", MUTE);
	if (home)
		find_home(&home, &pwd, &tmp);
	else
	{
		home = "";
		tmp = pwd;
	}
	totallen = ft_strlen(user) + ft_strlen(home) + ft_strlen(tmp) + 5;
	ft_printf("%s%s: %s%s%s>%s ", RED, user, BLUE, home, tmp, RESET);
	if (tmp != pwd)
		ft_strdel(&tmp);
	return (totallen);
}
