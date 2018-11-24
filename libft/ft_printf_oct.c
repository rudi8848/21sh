/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_oct.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 17:31:20 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/10 17:35:22 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		ft_o_width_right(uintmax_t nbr, t_propt *opt, int len)
{
	int ret;

	ret = 0;
	if (opt->fill_by_zero && !opt->precision)
		ret += fillnchar(len, opt->width, '0');
	else if (opt->precision > len)
	{
		ret += fillnchar(opt->precision, opt->width, ' ');
		ret += fillnchar(len, opt->precision, '0');
	}
	else
		ret += fillnchar(len, opt->width, ' ');
	print_oct(nbr);
	ret += len;
	return (ret);
}

int		ft_o_width_left(uintmax_t nbr, t_propt *opt, int len)
{
	int ret;

	ret = 0;
	if (opt->precision > len)
		ret += fillnchar(len, opt->precision, '0');
	print_oct(nbr);
	ret += len;
	ret += fillnchar(ret, opt->width, ' ');
	return (ret);
}

int		ft_o_no_width(uintmax_t nbr, t_propt *opt, int len)
{
	int ret;

	ret = 0;
	if (opt->precision > len)
		ret += fillnchar(len, opt->precision, '0');
	print_oct(nbr);
	ret += len;
	return (ret);
}

ssize_t	ft_printf_putnbr_oct(char **ft, va_list *arg, t_propt *opt, int *res)
{
	uintmax_t	nbr;
	int			len;
	int			ret;

	ret = 0;
	if (!ft)
		exit(EXIT_FAILURE);
	nbr = ft_cut_unsigned(arg, opt);
	if (!nbr && opt->is_set_precision && !opt->precision &&
			!opt->show_prefix)
		return (ft_zero_precision(*ft, opt, res));
	len = ft_unbr_length(&nbr, 8);
	if (nbr != 0 && opt->show_prefix)
	{
		if (opt->precision < len)
			opt->precision = len + 1;
	}
	if (opt->width > len && !opt->left_align)
		ret += ft_o_width_right(nbr, opt, len);
	else if (opt->width > len && opt->left_align)
		ret += ft_o_width_left(nbr, opt, len);
	else
		ret += ft_o_no_width(nbr, opt, len);
	*res += ret;
	return (ret);
}
