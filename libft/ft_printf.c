/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 14:44:29 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/11 09:10:35 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int				fillnchar(int len, int width, char c)
{
	int			i;
	char		str[256];

	i = 0;
	while (len < width)
	{
		str[i] = c;
		len++;
		i++;
	}
	return (write(1, str, i));
}

t_pf			ft_choose_type(t_conv conv, t_propt *options)
{
	static t_pf *convert_functions;

	if (!convert_functions)
	{
		convert_functions = (t_pf*)ft_memalloc(sizeof(t_pf) * CONVERSIONS);
		if (!convert_functions)
			exit(EXIT_FAILURE);
		ft_set_array(convert_functions);
	}
	if (conv == CONV_s && options->len_l)
		return (convert_functions[CONV_S]);
	if (conv == CONV_c && options->len_l)
		return (convert_functions[CONV_C]);
	return (convert_functions[conv]);
}

static int		check_type(char c, t_propt *options)
{
	if (c == 's' || c == 'd' || c == 'c' || c == 'i')
		return (1);
	else if (c == 'S' || c == 'C')
	{
		options->len_l = 1;
		return (1);
	}
	else if (c == 'o' || c == 'u' || c == 'x' || c == 'X')
		return (1);
	else if (c == 'D' || c == 'U' || c == 'O' || c == 'p')
	{
		options->len_l = 1;
		return (1);
	}
	else
		return (0);
}

ssize_t			ft_parse_options(const char **format, va_list *args, int *res)
{
	t_propt		*options;
	char		*fmtp;
	t_pf		ft_transformer;

	fmtp = (char*)++(*format);
	options = (t_propt*)ft_memalloc(sizeof(t_propt));
	if (!options)
		exit(EXIT_FAILURE);
	if (*fmtp)
	{
		fmtp += ft_parse_flags(fmtp, options);
		fmtp += ft_parse_width(fmtp, args, options);
		fmtp += ft_parse_precision(fmtp, args, options);
		fmtp += ft_parse_length(fmtp, options);
		if (check_type(*fmtp, options))
		{
			ft_transformer = ft_choose_type(*fmtp, options);
			ft_transformer(&fmtp, args, options, res);
		}
		else
			ft_printf_putchar(&fmtp, NULL, options, res);
		free(options);
		return (fmtp - *format);
	}
	return (0);
}

int				ft_printf(const char *format, ...)
{
	va_list		args;
	int			res;
	char		*ptr;

	res = 0;
	va_start(args, format);
	while (*format)
	{
		if (*format == '%')
		{
			ptr = (char*)format + 1;
			if (!*ptr)
				return (res);
			format += ft_parse_options(&format, &args, &res);
		}
		else
		{
			ft_putchar(*format);
			res++;
		}
		format++;
	}
	va_end(args);
	return (res);
}
