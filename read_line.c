#include "21sh.h"

int	ft_get_width(void)
{
	struct winsize	argp;
	int				ret;

	ret = ioctl(shell_terminal, TIOCGWINSZ, &argp);
	if (ret != 0)
	{
		ft_putstr_fd("Cannot get window size\n", STDERR_FILENO);
		return -1;
	}
	return argp.ws_col;
}

void	get_curpos(t_cpos *pos)
{
	char buf[21];
	int ret = 0;
	memset(buf, 0, 21);
	write(0, "\033[6n", 4);
	ret = read(0, buf, 20);

	pos->curx = atoi(strrchr(buf, ';') + 1);
	pos->start = pos->curx;
	pos->cury = atoi(&buf[2]) -1;
	//ft_printf("> %d, %d", pos->curx, pos->cury);
}

void	init_position(t_cpos *pos, int start)
{
	get_curpos(pos);
	pos->startline = start;
	pos->width = ft_get_width();
	pos->curln = 0;
	pos->height = 1;
	pos->len = 0;
	pos->i = 0;
}

//-----------------------------------------------------------------------------
void print_pos(t_cpos *pos)
{
	ft_printf("x: %d, y: %d, start: %d\n", pos->curx, pos->cury, pos->start);
	ft_printf("pos->curln: %d, pos->width: %d\n", pos->curln, pos->width);
	ft_printf("pos->i: %d, pos->len: %d\n", pos->i, pos->len);
}
//-----------------------------------------------------------------------------

void 	ft_move(uint64_t direction, char *line, t_cpos *pos)
{
	if (direction == K_RIGHT)
	{
		if (pos->i < pos->len)
		{
			++(*pos).i;
			if (pos->curx < pos->width)
				++(*pos).curx;
			else if (pos->curx == pos->width)
			{
				++(*pos).cury;
				++(*pos).curln;
				pos->curx = 1;
			}
			tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1, (*pos).cury), 0, ft_iputchar);
		}
	}
	else if (direction == K_LEFT)
	{
		if (pos->i > 0)
		{
			--(*pos).i;
			if (((*pos).curln == 0 && (*pos).curx > (*pos).start) || 
				(pos->curln > 0 && pos->curx > 1))
				--(*pos).curx;
			else if ((*pos).curln > 0 && (*pos).curx == 1)
			{
				--(*pos).curln;
				--(*pos).cury;
				pos->curx = pos->width;
			}
			tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx-1, (*pos).cury), 0, ft_iputchar);
		}
	}
}
void	move_to_border(uint64_t rb, char *line, t_cpos *pos)
{
	if (rb == K_HOME)
	{
		pos->i = 0;
		pos->curx = pos->start;
		pos->cury -= pos->curln;
		tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx -1, (*pos).cury), 0, ft_iputchar);
	}
}

void	check_key(char *line, t_cpos *pos, uint64_t rb, int cmd)
{
	if (rb == K_RIGHT || rb == K_LEFT)
		ft_move(rb, line, pos);/*
	else if (rb == K_CTRL_RIGHT || rb == K_CTRL_LEFT)
		jump(rb, line, pos);
	else if (rb == K_DOWN || rb == K_UP)
		move_history(rb, line, pos, cmd);
	
	else if (rb == K_BSPACE || rb == K_DELETE || rb == K_CTRL_D)
		delete_char(rb, line, pos);
	else if (rb == K_CTRL_C || rb == K_ENTER)
	{
       	ft_printf("\n");
       	rb == K_CTRL_C ? ft_bzero(line, MAXLINE) : line[len] = '\n';
       	return;
    }*/
	else if (rb == K_HOME || rb == K_END)
		move_to_border(rb, line, pos);
    else if (rb == K_ESC)
        ft_exit();
}

void print(char *line, t_cpos *pos, uint64_t rb, int rr)
{
	int j;
	if (pos->len + 1 + pos->startline  == MAXLINE)
	{
        TERM_BELL          // bell
        ft_printf("\nLine is too long\n");
        return;
    }
    write(STDOUT_FILENO, &rb, rr);
    
    tputs(tgetstr("im", NULL), 0, ft_iputchar);          //insert mode
    if (line[pos->i])   //if it's at the middle of line
    {
            j = pos->len + 1;
            while (j > pos->i)
            {
                line[j] = line[j - 1];
                j--;
            }            
            tputs(tgetstr("sc", NULL), 0, ft_iputchar);      // save cursor position
            int q = pos->curln;
           	tputs(tgetstr("ce", NULL), 0, ft_iputchar);      // delete end of line            
            write(STDOUT_FILENO, line + pos->i + 1, pos->len);
            tputs(tgetstr("rc", NULL), 0, ft_iputchar);      // restore cursor position
    }
    if (pos->curx < pos->width)
		++(*pos).curx;
	else if (pos->curx == pos->width)
	{
		++(*pos).cury;
		++(*pos).curln;
		++(*pos).height;
		pos->curx = 1;
	}
    line[pos->i] = (char)rb;
    ++(*pos).len;
    if (pos->i < pos->len)
        ++(*pos).i;
    //print_pos(pos);
	//tputs(tgetstr("ei", NULL), 0, ft_iputchar);          //end of insertion mode

}

void    read_line(char *line, int start)
{
	int         rr;
    uint64_t    rb;
    t_cpos		pos;
    int 		cmd;

    cmd = g_hstr_nb;
    if (start == 0)
	    type_prompt();
	ft_bzero(line, MAXLINE - start);
	init_position(&pos, start);
	rb = 0;
	while ((rr = read(STDIN_FILENO, &rb, 8)) > 0)
	{
		if (ft_isprint(rb))
		{
			print(line, &pos, rb, rr);
		}
		else
			check_key(line, &pos, rb, cmd);
		rb = 0;
	}
	//line[pos.i] = '\n';
    return;
}