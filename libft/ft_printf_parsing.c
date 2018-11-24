/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_parsing.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 15:06:40 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/10 15:52:06 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void		ft_correct_opt(t_propt *options)
{
	if (options->left_align)
		options->fill_by_zero = 0;
	if (options->show_sign && options->space_before)
		options->space_before = 0;
}

int				ft_parse_flags(char *fp, t_propt *options)
{
	int i;

	i = 0;
	while (fp[i] && (fp[i] == '-' || fp[i] == '+' || fp[i] == ' '
				|| fp[i] == '#' || fp[i] == '0'))
	{
		if (fp[i] && fp[i + 1])
		{
			if (fp[i] == '+')
				options->show_sign = 1;
			else if (fp[i] == ' ' && !options->show_sign)
				options->space_before = 1;
			else if (fp[i] == '#')
				options->show_prefix = 1;
			else if (fp[i] == '0')
				options->fill_by_zero = 1;
			else if (fp[i] == '-')
				options->left_align = 1;
		}
		i++;
	}
	ft_correct_opt(options);
	return (i);
}

int				ft_parse_width(char *fp, va_list *args, t_propt *options)
{
	int i;
	int arg;

	i = 0;
	while (ft_isdigit(fp[i]) || fp[i] == '*')
	{
		if (ft_isdigit(fp[i]))
		{
			options->width = ft_atoi(fp + i);
			while (ft_isdigit(fp[i]))
				i++;
		}
		if (fp[i] == '*')
		{
			arg = va_arg(*args, int);
			if (arg < 0)
			{
				arg = -arg;
				options->left_align = 1;
			}
			options->width = arg;
			i++;
		}
	}
	return (i);
}

int				ft_parse_precision(char *fp, va_list *args, t_propt *options)
{
	int i;
	int arg;

	i = 0;
	if (fp[i] && fp[i] == '.')
	{
		i++;
		options->is_set_precision = 1;
		if (ft_isdigit(fp[i]))
		{
			options->precision = ft_atoi(fp + i);
			while (ft_isdigit(fp[i]))
				i++;
		}
		if (fp[i] == '*')
		{
			arg = va_arg(*args, int);
			if (arg >= 0)
				options->precision = arg;
			else
				options->is_set_precision = 0;
			i++;
		}
	}
	return (i);
}

int				ft_parse_length(char *fp, t_propt *options)
{
	int i;

	i = 0;
	while (fp[i] == 'h' || fp[i] == 'l' || fp[i] == 'j' || fp[i] == 'z')
		i++;
	if (i)
	{
		if (ft_strncmp(fp, "hh", 2) == 0)
			options->len_hh = 1;
		else if (ft_strncmp(fp, "ll", 2) == 0)
			options->len_ll = 1;
		else if (ft_strncmp(fp, "l", 1) == 0)
			options->len_l = 1;
		else if (ft_strncmp(fp, "h", 1) == 0)
			options->len_h = 1;
		else if (ft_strncmp(fp, "j", 1) == 0)
			options->len_j = 1;
		else if (ft_strncmp(fp, "z", 1) == 0)
			options->len_z = 1;
	}
	return (i);
}
