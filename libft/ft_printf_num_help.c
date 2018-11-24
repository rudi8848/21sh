/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_num_help.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/11 14:09:26 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/11 14:09:30 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		ft_print_s_pref(t_propt *opt, intmax_t nbr)
{
	int ret;

	ret = 0;
	if (opt->space_before && nbr >= 0)
		ret += write(1, " ", 1);
	else if (opt->show_sign && nbr >= 0)
		ret += write(1, "+", 1);
	return (ret);
}

int		ft_s_no_width(t_propt *opt, int len, intmax_t nbr)
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
			len--;
			ret++;
		}
		ret += fillnchar(len, opt->precision, '0');
	}
	print_sdec(nbr);
	ret += len;
	return (ret);
}
