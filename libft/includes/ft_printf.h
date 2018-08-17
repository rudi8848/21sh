/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gvynogra <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/12 11:46:02 by gvynogra          #+#    #+#             */
/*   Updated: 2018/04/12 11:46:07 by gvynogra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include "libft.h"
# include <stdarg.h>
# include <stdio.h>
# include <locale.h>
# include <wchar.h>
# include <inttypes.h>
# include <sys/types.h>

typedef struct	s_options
{
	int			left_align;
	int			show_sign;
	int			space_before;
	int			show_prefix;
	int			fill_by_zero;
	int			width;
	int			is_set_precision;
	int			precision;
	int			len_hh;
	int			len_h;
	int			len_l;
	int			len_ll;
	int			len_j;
	int			len_z;
}				t_propt;

typedef ssize_t(*t_pf)(char **fmt, va_list *args, t_propt *opt, int *res);
typedef enum
{
	CONV_s = 's',
	CONV_S = 'S',
	CONV_p = 'p',
	CONV_d = 'd',
	CONV_D = 'D',
	CONV_i = 'i',
	CONV_o = 'o',
	CONV_O = 'O',
	CONV_u = 'u',
	CONV_U = 'U',
	CONV_x = 'x',
	CONV_X = 'X',
	CONV_c = 'c',
	CONV_C = 'C',
	CONV_b = 'b',
	CONV_f = 'f',
	CONV_F = 'F',
	CONVERSIONS = 127
}	t_conv;

int				ft_printf(const char *format, ...);
int				ft_parse_flags(char *fp, t_propt *opt);
int				ft_parse_width(char *fp, va_list *arg, t_propt *opt);
int				ft_parse_precision(char *fp, va_list *arg, t_propt *opt);
int				ft_parse_length(char *fp, t_propt *opt);
int				ft_print_null_string(void);
ssize_t			ft_printf_putchar(char **fmt, va_list *arg, t_propt *o, int *r);
ssize_t			ft_printf_putstr(char **fmt, va_list *arg, t_propt *o, int *r);
size_t			ft_wstrlen(wchar_t *wstr);
ssize_t			print_wstr(char **fmt, va_list *arg, t_propt *opt, int *res);
int				ft_nb_bits(wint_t wc);
int				ft_nb_bytes(int n);
int				ft_putwchar(wint_t wc);
uintmax_t		ft_cut_unsigned(va_list *arg, t_propt *opt);
intmax_t		ft_cut_signed(va_list *arg, t_propt *opt);
int				ft_unbr_length(uintmax_t *n, int base);
int				ft_snbr_length(intmax_t *n, int base);
void			print_oct(uintmax_t n);
void			print_sdec(intmax_t n);
void			print_udec(uintmax_t n);
int				fillnchar(int len, int width, char c);
ssize_t			ft_printf_putnbr_oct(char **f, va_list *a, t_propt *o, int *r);
ssize_t			ft_printf_putnbr_hex(char **f, va_list *a, t_propt *o, int *r);
ssize_t			ft_printf_putnbr_sdec(char **f, va_list *a, t_propt *o, int *r);
ssize_t			ft_printf_putnbr_udec(char **f, va_list *a, t_propt *o, int *r);
void			ft_set_array(t_pf *convert_functions);
t_pf			ft_choose_type(t_conv conv, t_propt *opt);
ssize_t			ft_parse_options(const char **fmt, va_list *arg, int *res);
void			print_hex(uintmax_t n, char a);
void			print_oct(uintmax_t n);
void			print_sdec(intmax_t n);
void			print_udec(uintmax_t n);
int				ft_zero_precision(char *ptr, t_propt *opt, int *res);
int				ft_print_s_pref(t_propt *opt, intmax_t nbr);
int				ft_s_no_width(t_propt *opt, int len, intmax_t nbr);
int				ft_printf_putchar_arg(char *ptr, int symb, t_propt *opt);
ssize_t			ft_printf_putchar(char **f, va_list *arg, t_propt *o, int *r);
int				ft_putstr_width_right(char *str, char *t, t_propt *o, int len);
int				ft_putstr_width_left(char *s, char *t, t_propt *opt, int len);
int				ft_putstr_width_default(char *str, char *tmp, int len);
int				ft_putwstr(wchar_t *wstr);
int				ft_putwstr_prec(wchar_t *wstr, t_propt *opt);

#endif
