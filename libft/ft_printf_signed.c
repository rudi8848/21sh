/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_signed.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:08:48 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/10 18:08:55 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		ft_s_right_precision(t_propt *opt, int len, intmax_t nbr)
{
	int ret;

	ret = 0;
	if (opt->precision < len)
		ret += fillnchar(len, opt->width, ' ');
	else if (opt->precision > len && nbr >= 0)
	{
		if (opt->space_before)
			ret += write(1, " ", 1);
		ret += fillnchar(opt->precision + opt->show_sign +
				opt->space_before, opt->width, ' ');
	}
	else if (opt->precision > len && nbr < 0)
		ret += fillnchar(opt->precision + 1, opt->width, ' ');
	else if (opt->width > opt->precision && opt->width > len
		&& nbr >= 0)
		ret += fillnchar(opt->precision, opt->width, ' ');
	if (opt->show_sign && !opt->fill_by_zero && nbr >= 0)
		ret += write(1, "+", 1);
	return (ret);
}

int		ft_s_right_neg(t_propt *opt, int *len, intmax_t *nbr)
{
	int ret;

	ret = 0;
	ret += ft_s_right_precision(opt, *len, *nbr);
	if (opt->precision >= *len)
	{
		if (*nbr < 0)
		{
			ft_putchar('-');
			*nbr = -*nbr;
			ret++;
			*len = *len - 1;
		}
		ret += fillnchar(*len, opt->precision, '0');
	}
	return (ret);
}

int		ft_s_width_right(t_propt *opt, int len, intmax_t nbr)
{
	int		ret;

	ret = 0;
	if (opt->fill_by_zero && !opt->precision)
	{
		if (nbr < 0)
		{
			ft_putchar('-');
			ret += fillnchar(len, opt->width, '0');
			nbr = -nbr;
		}
		else
		{
			ret += ft_print_s_pref(opt, nbr);
			ret += fillnchar(len + ret, opt->width, '0');
		}
	}
	else
		ret += ft_s_right_neg(opt, &len, &nbr);
	print_sdec(nbr);
	ret += len;
	return (ret);
}

int		ft_s_width_left(t_propt *opt, int len, intmax_t nbr)
{
	int ret;

	ret = 0;
	ret += ft_print_s_pref(opt, nbr);
	if (opt->precision > len)
	{
		if (nbr < 0)
		{
			ft_putchar('-');
			nbr = -nbr;
		}
		ret += fillnchar(len, opt->precision, '0');
	}
	print_sdec(nbr);
	ret += len;
	ret += fillnchar(ret, opt->width, ' ');
	return (ret);
}

ssize_t	ft_printf_putnbr_sdec(char **fmt, va_list *args, t_propt *opt, int *res)
{
	intmax_t	nbr;
	int			len;
	int			ret;

	ret = 0;
	if (!fmt)
		exit(EXIT_FAILURE);
	nbr = ft_cut_signed(args, opt);
	len = ft_snbr_length(&nbr, 10);
	if (!nbr && opt->is_set_precision && !opt->precision)
		return (ft_zero_precision(*fmt, opt, res));
	if (opt->width > len && !opt->left_align)
		ret += ft_s_width_right(opt, len, nbr);
	else if (opt->width > len && opt->left_align)
		ret += ft_s_width_left(opt, len, nbr);
	else
		ret += ft_s_no_width(opt, len, nbr);
	*res += ret;
	return (ret);
}
