/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_wchar.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/11 15:51:31 by gvynogra          #+#    #+#             */
/*   Updated: 2018/12/07 14:42:39 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int			ft_nb_bits(wint_t wc)
{
	int i;

	i = 0;
	while (wc)
	{
		wc /= 2;
		i++;
	}
	return (i);
}

int			ft_nb_bytes(int n)
{
	if (n <= 7)
		return (1);
	else if (n <= 11)
		return (2);
	else if (n <= 16)
		return (3);
	else
		return (4);
}

void		ft_fill_char(char *tmp, wint_t wc, int bytes)
{
	if (bytes == 1)
		tmp[0] = wc;
	else
	{
		if (bytes == 2)
			tmp[0] = ((wc & (0x1f << 6)) >> 6) | 0xC0;
		else
		{
			if (bytes == 3)
				tmp[0] = ((wc >> 12) & 0xf) | 0xE0;
			else
			{
				tmp[0] = ((wc >> 18) & 7) | 0xF0;
				tmp[1] = ((wc >> 12) & 0x3f) | 0x80;
			}
			tmp[bytes - 2] = ((wc >> 6) & 0x3f) | 0x80;
		}
		tmp[bytes - 1] = (wc & 0x3f) | 0x80;
	}
}

int			ft_putwchar(wint_t wc)
{
	char	tmp[4];
	int		bytes;
	int		bits;

	tmp[0] = 0;
	tmp[1] = 0;
	tmp[2] = 0;
	tmp[3] = 0;
	bits = ft_nb_bits(wc);
	bytes = ft_nb_bytes(bits);
	if (bytes <= bits && bytes <= MB_CUR_MAX)
		ft_fill_char(tmp, wc, bytes);
	return (write(1, tmp, bytes));
}

size_t		ft_wstrlen(wchar_t *wstr)
{
	size_t	len;
	int		i;

	len = 0;
	i = 0;
	while (wstr[i] != L'\0')
	{
		len += ft_nb_bytes(ft_nb_bits(wstr[i]));
		i++;
	}
	return (len);
}
