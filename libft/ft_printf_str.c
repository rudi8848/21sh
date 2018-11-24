/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_str.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/11 14:23:59 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/11 14:24:03 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		ft_print_null_string(void)
{
	write(1, "(null)", 6);
	return (6);
}

int		ft_putstr_arg(char *str, char *tmp, t_propt *options, int *res)
{
	int ret;
	int len;

	len = 0;
	ret = 0;
	len = ft_strlen(str);
	if (options->is_set_precision && options->precision < len)
	{
		tmp = (char*)ft_memalloc((options->precision + 1) * sizeof(char));
		tmp = ft_strncpy(tmp, str, options->precision);
		len = options->precision;
	}
	if (len < options->width && !options->left_align)
		ret += ft_putstr_width_right(str, tmp, options, len);
	else if (len < options->width && options->left_align)
		ret += ft_putstr_width_left(str, tmp, options, len);
	else
		ret += ft_putstr_width_default(str, tmp, len);
	*res += ret;
	ft_strdel(&tmp);
	return (len);
}

ssize_t	ft_printf_putstr(char **fmt, va_list *args, t_propt *options, int *res)
{
	int		len;
	char	*str;
	char	*tmp;

	len = 0;
	if (!fmt || !options)
		exit(EXIT_FAILURE);
	str = (char*)va_arg(*args, const char*);
	tmp = NULL;
	if (str)
		len = ft_putstr_arg(str, tmp, options, res);
	else
		(options->width && options->is_set_precision) ? (*res += fillnchar(0,
			options->width, '0')) : (*res += ft_print_null_string());
	return (len);
}

int		ft_printf_wstr_precision(wchar_t *wstr, t_propt *options, int *res)
{
	int ret;

	ret = 0;
	if (options->fill_by_zero)
	{
		if (options->precision)
			ret += fillnchar(0, options->precision - ft_wstrlen(wstr), '0');
		else
			ret += fillnchar(0, options->width - ft_wstrlen(wstr), '0');
	}
	else if (options->width > (int)ft_wstrlen(wstr) && !options->left_align)
		ret += fillnchar(0, options->width - ft_wstrlen(wstr), ' ');
	if (options->is_set_precision)
		ret += ft_putwstr_prec(wstr, options);
	else
		ret += ft_putwstr(wstr);
	if (options->left_align && options->width > (int)ft_wstrlen(wstr))
		ret += fillnchar(0, options->width - ft_wstrlen(wstr), ' ');
	*res += ret;
	return (ret);
}

ssize_t	print_wstr(char **fmt, va_list *args, t_propt *options, int *res)
{
	wchar_t	*wstr;
	int		ret;

	ret = 0;
	if (!fmt || !options)
		exit(EXIT_FAILURE);
	wstr = (wchar_t *)va_arg(*args, wchar_t*);
	if (!wstr)
	{
		*res += ft_print_null_string();
		return (6);
	}
	if (options->is_set_precision || options->width > (int)ft_wstrlen(wstr))
		return (ft_printf_wstr_precision(wstr, options, res));
	else
		ret += ft_putwstr(wstr);
	*res += ret;
	return (ret);
}
