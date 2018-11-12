/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_help.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/12 14:17:02 by gvynogra          #+#    #+#             */
/*   Updated: 2018/11/12 14:17:04 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int		store_char(char *word, size_t maxword, int c, size_t *np)
{
	if (*np < maxword)
	{
		word[(*np)++] = c;
		return (1);
	}
	else
		return (0);
}

void	read_more(char *line, int i, char *prompt)
{
	ft_printf("%s", prompt);
	cbreak_settings();
	read_line(line, i);
	ft_restore();
}
