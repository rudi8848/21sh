/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_wstr_help.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/12 11:33:58 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/12 11:34:00 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		ft_putwstr(wchar_t *wstr)
{
	int		i;
	int		ret;

	ret = 0;
	i = 0;
	while (wstr[i] != L'\0')
	{
		ret += ft_putwchar(wstr[i]);
		i++;
	}
	return (ret);
}

int		ft_putwstr_prec(wchar_t *wstr, t_propt *options)
{
	int		i;
	int		ret;

	ret = 0;
	i = 0;
	while (ret < options->precision)
	{
		ret += ft_putwchar(wstr[i]);
		i++;
	}
	return (ret);
}
