/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_hex.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 16:18:37 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/10 16:21:42 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		ft_print_prefix(char *ptr, t_propt *opt)
{
	int ret;

	ret = 0;
	if (opt->show_prefix)
	{
		*ptr == 'X' ? write(1, "0X", 2) : write(1, "0x", 2);
		ret += 2;
	}
	return (ret);
}

int		ft_x_width_right(char *ptr, int len, t_propt *opt, uintmax_t nbr)
{
	int ret;

	ret = 0;
	if (opt->fill_by_zero && !opt->precision)
	{
		if (opt->show_prefix)
		{
			ret += ft_print_prefix(ptr, opt);
			ret += fillnchar(len + ret, opt->width, '0');
			opt->show_prefix = 0;
		}
		else
			ret += fillnchar(len, opt->width, '0');
	}
	else if (!(opt->precision > len))
		ret += fillnchar(len + opt->show_prefix * 2, opt->width, ' ');
	else if (opt->precision > len)
		ret += fillnchar(opt->precision + opt->show_prefix * 2,
			opt->width, ' ');
	ret += ft_print_prefix(ptr, opt);
	if (opt->precision > len)
		ret += fillnchar(len, opt->precision, '0');
	print_hex(nbr, *ptr == 'X' ? 'A' : 'a');
	ret += len;
	return (ret);
}

int		ft_x_width_left(char *ptr, int len, t_propt *opt, uintmax_t nbr)
{
	int ret;

	ret = 0;
	if (opt->show_prefix)
	{
		*ptr == 'X' ? write(1, "0X", 2) : write(1, "0x", 2);
		ret += 2;
	}
	if (opt->precision > len)
		ret += fillnchar(len, opt->precision, '0');
	print_hex(nbr, *ptr == 'X' ? 'A' : 'a');
	ret += len;
	ret += fillnchar(ret, opt->width, ' ');
	return (ret);
}

int		ft_x_no_width(char *ptr, int len, t_propt *opt, uintmax_t nbr)
{
	int ret;

	ret = 0;
	if (opt->show_prefix)
	{
		*ptr == 'X' ? write(1, "0X", 2) : write(1, "0x", 2);
		ret += 2;
	}
	if (opt->precision > len)
		ret += fillnchar(len, opt->precision, '0');
	print_hex(nbr, *ptr == 'X' ? 'A' : 'a');
	ret += len;
	return (ret);
}

ssize_t	ft_printf_putnbr_hex(char **fmt, va_list *args, t_propt *opt, int *res)
{
	uintmax_t	nbr;
	int			len;
	char		*ptr;
	int			ret;

	ptr = (char*)*fmt;
	nbr = ft_cut_unsigned(args, opt);
	ret = 0;
	if (nbr == 0)
		(opt->show_prefix = 0);
	if (*ptr == 'p')
		(opt->show_prefix = 1);
	if (!nbr && opt->is_set_precision && !opt->precision)
		return (ft_zero_precision(ptr, opt, res));
	len = ft_unbr_length(&nbr, 16);
	if (opt->width > len && !opt->left_align)
		ret += ft_x_width_right(ptr, len, opt, nbr);
	else if (opt->width > len && opt->left_align)
		ret += ft_x_width_left(ptr, len, opt, nbr);
	else
		ret += ft_x_no_width(ptr, len, opt, nbr);
	*res += ret;
	return (ret);
}
