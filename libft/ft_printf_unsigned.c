/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_unsigned.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/11 09:11:16 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/11 09:13:08 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		ft_u_width_right(t_propt *opt, int len, uintmax_t nbr)
{
	int		ret;

	ret = 0;
	if (opt->fill_by_zero && !opt->precision)
		ret += fillnchar(len + ret, opt->width, '0');
	else
	{
		if (opt->precision < len)
			ret += fillnchar(len, opt->width, ' ');
		else if (opt->precision > len && nbr)
			ret += fillnchar(opt->precision + opt->show_sign,
				opt->width, ' ');
		if (opt->precision >= len)
			ret += fillnchar(len, opt->precision, '0');
	}
	return (ret);
}

ssize_t	ft_printf_putnbr_udec(char **fmt, va_list *args, t_propt *opt, int *res)
{
	uintmax_t	nbr;
	int			len;
	int			ret;

	ret = 0;
	if (!fmt)
		exit(EXIT_FAILURE);
	nbr = ft_cut_unsigned(args, opt);
	len = ft_unbr_length(&nbr, 10);
	if (!nbr && opt->is_set_precision && !opt->precision)
		return (ft_zero_precision(*fmt, opt, res));
	if (opt->width > len && !opt->left_align)
		ret += ft_u_width_right(opt, len, nbr);
	else
	{
		if (opt->precision > len)
			ret += fillnchar(len, opt->precision, '0');
	}
	print_udec(nbr);
	ret += len;
	ret += fillnchar(ret, opt->width, ' ');
	*res += ret;
	return (ret);
}
