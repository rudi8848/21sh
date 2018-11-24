/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_nconv.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 17:23:44 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/10 17:25:40 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

void	ft_set_array(t_pf *convert_functions)
{
	convert_functions[CONV_c] = &ft_printf_putchar;
	convert_functions[CONV_s] = &ft_printf_putstr;
	convert_functions[CONV_d] = &ft_printf_putnbr_sdec;
	convert_functions[CONV_D] = &ft_printf_putnbr_sdec;
	convert_functions[CONV_i] = &ft_printf_putnbr_sdec;
	convert_functions[CONV_u] = &ft_printf_putnbr_udec;
	convert_functions[CONV_U] = &ft_printf_putnbr_udec;
	convert_functions[CONV_o] = &ft_printf_putnbr_oct;
	convert_functions[CONV_O] = &ft_printf_putnbr_oct;
	convert_functions[CONV_x] = &ft_printf_putnbr_hex;
	convert_functions[CONV_X] = &ft_printf_putnbr_hex;
	convert_functions[CONV_p] = &ft_printf_putnbr_hex;
	convert_functions[CONV_C] = &ft_printf_putchar;
	convert_functions[CONV_S] = &print_wstr;
}

void	print_oct(uintmax_t n)
{
	if (n >= 8)
	{
		print_oct(n >> 3);
		ft_putchar(n % 8 + '0');
	}
	else
		ft_putchar(n + '0');
}

void	print_hex(uintmax_t n, char a)
{
	if (n >= 16)
	{
		print_hex(n >> 4, a);
		print_hex(n % 16, a);
	}
	else if (n >= 10 && n <= 15)
		ft_putchar(n - 10 + a);
	else
		ft_putchar(n + '0');
}

void	print_sdec(intmax_t n)
{
	uintmax_t	nbr;

	if (n < 0)
	{
		ft_putchar('-');
		nbr = -n;
	}
	else
		nbr = n;
	if (nbr >= 10)
	{
		print_sdec(nbr / 10);
		ft_putchar(nbr % 10 + '0');
	}
	else
		ft_putchar(nbr + '0');
}

void	print_udec(uintmax_t n)
{
	if (n >= 10)
	{
		print_udec(n / 10);
		ft_putchar(n % 10 + '0');
	}
	else
		ft_putchar(n + '0');
}
