/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_putstr_help.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/11 15:42:19 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/11 15:42:22 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		ft_printf_putchar_arg(char *ptr, int symb, t_propt *options)
{
	int ret;

	ret = 0;
	if (options->width && !options->left_align)
	{
		if (options->fill_by_zero)
			ret += fillnchar(ft_nb_bytes(ft_nb_bits(symb)),
				options->width, '0');
		else
			ret += fillnchar(ft_nb_bytes(ft_nb_bits(symb)),
				options->width, ' ');
	}
	if (*ptr == 'c' && !options->len_l)
	{
		ft_putchar(symb);
		ret += 1;
	}
	else
		ret += ft_putwchar(symb);
	return (ret);
}

ssize_t	ft_printf_putchar(char **fmt, va_list *args, t_propt *options, int *res)
{
	int		ret;

	ret = 0;
	if (args)
		ret += ft_printf_putchar_arg(*fmt, va_arg(*args, int), options);
	else
	{
		if (options->width && !options->left_align)
		{
			if (options->fill_by_zero)
				ret += fillnchar(1, options->width, '0');
			else
				ret += fillnchar(1, options->width, ' ');
		}
		ft_putchar(**fmt);
		ret += 1;
	}
	if (options->width && options->left_align)
		ret += fillnchar(1, options->width, ' ');
	*res += ret;
	return (ret);
}

int		ft_putstr_width_right(char *str, char *tmp, t_propt *options, int len)
{
	int ret;

	ret = 0;
	if (options->fill_by_zero)
		ret += fillnchar(len, options->width, '0');
	else
		ret += fillnchar(len, options->width, ' ');
	tmp ? ft_putstr(tmp) : ft_putstr(str);
	ret += len;
	return (ret);
}

int		ft_putstr_width_left(char *str, char *tmp, t_propt *options, int len)
{
	int ret;

	ret = 0;
	tmp ? ft_putstr(tmp) : ft_putstr(str);
	ret += len;
	ret += fillnchar(ret, options->width, ' ');
	return (ret);
}

int		ft_putstr_width_default(char *str, char *tmp, int len)
{
	int ret;

	ret = 0;
	tmp ? ft_putstr(tmp) : ft_putstr(str);
	ret += len;
	return (ret);
}
