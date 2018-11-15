/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_insert_variable.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 14:07:44 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 14:07:46 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/shell.h"

static size_t	ft_len(int nbr)
{
	size_t			i;

	i = 0;
	if (nbr == 0)
		return (1);
	while (nbr != 0)
	{
		nbr /= 10;
		++i;
	}
	return (i);
}

static size_t	ft_itoa_buf(int value, char *buf)
{
	size_t		len;
	int			i;

	len = ft_len(value);
	i = len - 1;
	buf[len] = '\0';
	while (i > -1)
	{
		buf[i] = (value % 10) + '0';
		value = value / 10;
		--i;
	}
	return (len);
}

static int		get_var_name(char *line, int *i, char *var)
{
	int		j;
	int		vname_len;

	vname_len = 0;
	j = *i;
	while (line[j] != '\0')
	{
		if (line[j] == ';' || line[j] == '&' ||
		line[j] == '|' || line[j] == '<' || line[j] == '>' ||
		line[j] == '\n' || line[j] == '\t' || line[j] == ' ' ||
		line[j] == '\"' || line[j] == '\'')
			break ;
		var[vname_len] = line[j];
		++vname_len;
		++j;
	}
	return (vname_len);
}

void			insert_variable(char *line, char *word, int *i, size_t *wordn)
{
	int		vname_len;
	char	var[256];
	char	*ptr;

	vname_len = 0;
	ptr = &var[0];
	ft_bzero(var, 256);
	if (line[*i + 1] == '$')
	{
		vname_len = ft_itoa_buf(getpid(), &word[*wordn]);
		*wordn += vname_len;
		*i += 1;
		return ;
	}
	vname_len = get_var_name(line, i, &var[0]);
	if ((ptr = get_copy_env(var + 1, MUTE)) != NULL)
	{
		ft_strcpy(var, ptr);
		word = ft_strcat(word, var);
		*wordn += ft_strlen(var);
	}
	if (vname_len)
		*i += vname_len - 1;
}
