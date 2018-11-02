#include "21sh.h"

void print_pos(t_cpos *pos);

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
//	printf("width: %d\n", argp.ws_col);
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
	pos->cury = atoi(&buf[2]) - 1;
}

void	reset_selection(t_cpos *pos)
{
	pos->selection = 0;
	pos->first = -1;
	pos->last = -1;
	//tputs(tgetstr("se", NULL), 0, ft_iputchar);
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
	reset_selection(pos);
	if (start)
		pos->start = pos->curx;
	//print_pos(pos);
}



int cmd_height(t_cpos *pos, char *line)
{
	int len;
	int width;
	int height;

	height = 1;
	len = ft_strlen(line);
	width = ft_get_width();
	if (len <= width - pos->start)
		return height;
	len -= width - pos->start;
	height += len / width;
	if (len % width)
		height++;
	return height;
}

//-----------------------------------------------------------------------------DELETE
void print_pos(t_cpos *pos)
{
	ft_printf("x: %d, y: %d, start: %d\n", (*pos).curx, (*pos).cury, (*pos).start);
	ft_printf("pos->curln: %d, pos->width: %d, pos->height: %d\n", (*pos).curln, (*pos).width,(*pos).height);
	ft_printf("pos->i: %d, pos->len: %d\n", pos->i, pos->len);
}
//-----------------------------------------------------------------------------

void 	ft_move(uint64_t direction, char *line, t_cpos *pos)
{
	reset_selection(pos);
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
	reset_selection(pos);
	pos->height = cmd_height(pos, line);
	if (rb == K_HOME)
	{
		if (pos->i)
		{
			pos->i = 0;
			pos->curx = pos->start;
			pos->cury -= pos->curln;
			pos->curln = 0;
			tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx -1, (*pos).cury), 0, ft_iputchar);
		}
	}
	if (rb == K_END)
	{
		if (pos->i < pos->len)
        {
        	pos->i = pos->len;
        	if (pos->height == 1)
        	{
        		pos->curx = pos->start + pos->len;
        		tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx-1, (*pos).cury), 0, ft_iputchar);
        	}
        	else
        	{
				if (pos->curln < pos->height )
				{
					pos->cury += pos->height - pos->curln - 1;
	        		pos->curln = pos->height - 1;
				}
			int first = pos->width - pos->start + 1;
        	pos->curx = (pos->len - first)  % (pos->width) + 1;
			if (pos->curx == 0)
				pos->curx = 1;
        	tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx-1, (*pos).cury), 0, ft_iputchar);
        	}
        }
	}
}

void	delete_from_screen(char *line, t_cpos *pos, char offset)
{
			tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - offset, (*pos).cury), 0, ft_iputchar);
            tputs(tgetstr("sc", NULL), 0, ft_iputchar);      // save cursor position
           	tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of screen       
            write(STDOUT_FILENO, line + pos->i , pos->len);		//write the rest
			pos->height = cmd_height(pos, line);
            tputs(tgetstr("rc", NULL), 0, ft_iputchar);
}

void	delete_char(uint64_t rb, char *line, t_cpos *pos)
{
	int j;

	reset_selection(pos);
	pos->height = cmd_height(pos, line);
	if (rb == K_DELETE || rb == K_CTRL_D)
	{
		if (pos->len > 0 && pos->i >= 0 && pos->i < pos->len)
        {            
            pos->len--;
            j = pos->i;
            while (line[j])
            {
               line[j] = line[j + 1];
               ++j;
            }
            delete_from_screen(line, pos, 1);
        }
	}
	else if (rb == K_BSPACE)
    {
        if (pos->i > 0)
        {
            pos->i--;
            pos->len--;
            j = pos->i;
            while (line[j])
            {
                line[j] = line[j + 1];
                ++j;
            }
            if (pos->curx > 1)
            {
            	delete_from_screen(line, pos, 2);
            	--(*pos).curx;
            }
            else if (pos->curx <= 1 && pos->curln)
            {
            	pos->curx = pos->width;
            	--(*pos).cury;
            	--(*pos).curln;
            	delete_from_screen(line, pos, 1);
            }
        }
    }
}

void	move_history(uint64_t rb, char *line, t_cpos *pos, int *cmd)
{
	reset_selection(pos);
	if (rb == K_DOWN)
    {
   			if (g_hstr_nb && *cmd < g_hstr_nb -1)
   			{
   				move_to_border(K_HOME, line, pos);
	           	tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of screen
				ft_bzero(line, MAXLINE);
	        	init_position(pos, 1);
				++(*cmd);
				ft_strcpy(line, g_history[*cmd]);         
	            pos->len = ft_strlen(line);
	            pos->i = pos->len - 1;
	        	write(STDOUT_FILENO,line, pos->len);
	            pos->height = cmd_height(pos, line);
	            move_to_border(K_END, line, pos);
        	}
        	else if (g_hstr_nb && *cmd == g_hstr_nb - 1)
        	{
        		move_to_border(K_HOME, line, pos);
	           	tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of screen
				ft_bzero(line, MAXLINE);
	        	init_position(pos, 1);
	        	++(*cmd);
	        	TERM_BELL
        	}
        	else
            	TERM_BELL         // bell
        }
        else if (rb == K_UP)
        {
        	if (g_hstr_nb && *cmd)
        	{
	           	move_to_border(K_HOME, line, pos);
	           	tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of screen
				ft_bzero(line, MAXLINE);
	        	init_position(pos, 1);
	        	if (*cmd)
	        	{
						--(*cmd);
					ft_strcpy(line, g_history[*cmd]);
	        	}	            
	            pos->len = ft_strlen(line);
	            pos->i = pos->len - 1;
	        	write(STDOUT_FILENO,line, pos->len);
	            pos->height = cmd_height(pos, line);
	            move_to_border(K_END, line, pos);
        	}
        	else
	           TERM_BELL          // bell
        }
}

void	ft_jump(uint64_t rb, char *line,t_cpos *pos)
{
	int j;
	int dif;

	reset_selection(pos);
	if (rb == K_CTRL_RIGHT)
	{
		if (pos->i < pos->len)
        {
         	j = pos->i;
         	if (line[j] && line[j] != ' ' && line[j] != '\t')		//start in word
         	{
         		while(line[j] && line[j] != ' ' && line[j] != '\t')	//go to next space
         			++j;
         	}
         	if (line[j] && (line[j] == ' ' || line[j] == '\t'))		//we're in space
         	{
         		while (line[j] && (line[j] == ' ' || line[j] == '\t'))	//go to next not space
         			++j;
         	}
         	if ((dif = j - pos->i) >= 0)
        	{
        		if (pos->curx + dif < pos->width)
        			pos->curx += dif;
         		tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx -1, (*pos).cury), 0, ft_iputchar);
        	}
         	pos->i = j;
        }
	}
	else if (rb == K_CTRL_LEFT)
	{
		j = pos->i;
		if (j>0 && !line[j])	// if cursor stands at the end of line
        	--j;
        if (j>0 && line[j] != ' ' && line[j] != '\t' && line[j - 1] != ' ' && line[j - 1] != '\t' )	//if we are in word
        {
        	while(j>0 && (line[j] != ' ' && line[j] != '\t'))
        		--j;
        	if (line[j] == ' ' || line[j] == '\t')
        		++j;
        }
        else if (j>0 && line[j] != ' ' && line[j] != '\t' && (line[j-1] == ' ' || line[j-1] == '\t'))
        	--j;
        if (j>0 && (line[j] == ' ' || line[j] == '\t'))
        {
        	while(j>0 && (line[j] == ' ' || line[j] == '\t'))
        		--j;
        	if (j>0 && line[j] != ' ' && line[j] != '\t')	//if we are in word
	       	{
	       		while(j>0)
	       		{
	       			if (line[j] == ' ' || line[j] == '\t')
	       			{
	       				++j;
	       				break;
	       			}
	       			--j;
	       		}
	       	}
        }
        if ((dif = pos->i - j) >= 0)
        {
        	if (pos->curx > dif)
        			pos->curx -= dif;
        	tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx -1, (*pos).cury), 0, ft_iputchar);
        }
        pos->i = j;
	}
}

void	ft_jump_vertical(uint64_t rb, char *line,t_cpos *pos)
{
	reset_selection(pos);
	pos->height = cmd_height(pos, line);
	if (rb == K_CTRL_UP)
	{
		if (pos->curln)
		{
			if (pos->curln == 1)
			{
				if (pos->curx < pos->start)
				{
					pos->curx = pos->start;
					pos->i = 0;
				}
			}
			if (pos->i)
				pos->i -= pos->width;
			--(*pos).cury;
			--(*pos).curln;
			tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx -1, (*pos).cury), 0, ft_iputchar);
		}
	}
	else if (rb == K_CTRL_DOWN)
	{
		if (pos->height > 1 && pos->curln < pos->height - 1)
		{
			if (pos->curln == pos->height - 2)
			{
				int tail = (pos->len - (pos->width - pos->start)) % pos->width;
				if (pos->curx > tail)
				{
					pos->curx = tail;
					pos->i = pos->len;
				}
			}
			if (pos->i + pos->width < pos->len)
				pos->i += pos->width;
			++(*pos).cury;
			++(*pos).curln;
			tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx - 1, (*pos).cury), 0, ft_iputchar);
		}
	}
}

void	ft_copy_pase(uint64_t rb,char* line,t_cpos *pos)
{
	static char* buf = NULL;
	int len;

	if (rb == K_ALT_C)
	{
		len = pos->last - pos->first;
		if (buf)
			free(buf);
		buf = ft_strsub(line, pos->first, len);
	}
	else if (rb == K_ALT_V)
	{
		if (!buf)
			return;
		// paste where is pos->i, rewrite line
		

		free(buf);
		buf = NULL;
	}
	else if (rb == K_ALT_X)
	{
		len = pos->last - pos->first;
		if (buf)
			free(buf);
		buf = ft_strsub(line, pos->first, len);
		// delete from first to last, rewrite line
	}
}

void	ft_highlight(uint64_t rb,char *line,t_cpos *pos)
{
	int first;
	int last;

	/*
	tputs(tgetstr("cd", NULL), 0, ft_iputchar);
	tputs(tgetstr("mr", NULL), 0, ft_iputchar);
	write(STDOUT_FILENO, line, pos->len);
	tputs(tgetstr("me", NULL), 0, ft_iputchar);
	*/

	if (rb == K_SHFT_L)
	{
		if (pos->i > 0)
		{
			if (pos->last < pos->i)
				pos->last = pos->i;
			--(*pos).i;
			//tputs(tgetstr("le", NULL), 0, ft_iputchar);
			--(*pos).curx;
			tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx-1 , (*pos).cury), 0, ft_iputchar);
			if (pos->first > pos->i || pos->first == -1)
				pos->first = pos->i;
			//ft_printf("first: %d, last: %d \n", pos->first, pos->last);
/*			
			tputs(tgetstr("do", NULL), 0, ft_iputchar);
           	tputs(tgetstr("cr", NULL), 0, ft_iputchar);
*/
			// save cursor, go home, clear, write with different modes, restore cursor
			
			tputs(tgetstr("sc", NULL), 0, ft_iputchar);      // save cursor position
			move_to_border(K_HOME, line, pos);
			
           	tputs(tgetstr("cd", NULL), 0, ft_iputchar); 	//clear
           	
           	write(STDOUT_FILENO, line, pos->first);
           	
           	pos->curx += pos->first;
           	tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx , (*pos).cury), 0, ft_iputchar);
        	
           	tputs(tgetstr("mr", NULL), 0, ft_iputchar);
           	write(STDOUT_FILENO, line + pos->first, pos->last - pos->first);
       
           	pos->curx += pos->last - pos->first;
           	tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx , (*pos).cury), 0, ft_iputchar);

           	tputs(tgetstr("me", NULL), 0, ft_iputchar);

           	
           	write(STDOUT_FILENO, line + pos->last, pos->len - pos->last);
           	tputs(tgetstr("rc", NULL), 0, ft_iputchar);
			
		}
		
	}
	else if (rb == K_SHFT_R)
	{
		if (pos->i < pos->len)
		{
			if (pos->first > pos->i || pos->first == -1)
				pos->first = pos->i;
			++(*pos).i;

			++(*pos).curx;
			tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx-1 , (*pos).cury), 0, ft_iputchar);
			if (pos->last < pos->i || pos->last == -1)
				pos->last = pos->i;
		}
	}
	//ft_printf("first: [%d], last: [%d]\n", pos->first, pos->last);
}

void	check_key(char *line, t_cpos *pos, uint64_t rb, int *cmd)
{
	if (rb == K_RIGHT || rb == K_LEFT)
		ft_move(rb, line, pos);
	else if (rb == K_DOWN || rb == K_UP)
		move_history(rb, line, pos, cmd);
	else if (rb == K_BSPACE || rb == K_DELETE || rb == K_CTRL_D)
		delete_char(rb, line, pos);
	else if (rb == K_HOME || rb == K_END)
		move_to_border(rb, line, pos);
	else if (rb == K_ESC)
		ft_exit();
	else if (rb == K_CTRL_UP || rb == K_CTRL_DOWN)
		ft_jump_vertical(rb, line, pos);
	else if (rb == K_CTRL_RIGHT || rb == K_CTRL_LEFT)
		ft_jump(rb, line, pos);
	else if (rb == K_SHFT_L || rb == K_SHFT_R)
		ft_highlight(rb, line, pos);
	else if (rb == K_ALT_C || rb == K_ALT_V || rb == K_ALT_X)
		ft_copy_pase(rb, line, pos);

}

void print(char *line, t_cpos *pos, uint64_t rb, int rr)
{
	int j;

	reset_selection(pos);
	if (pos->len + 1 + pos->startline  == MAXLINE)
	{
        TERM_BELL          // bell
        ft_printf("\nLine is too long\n");
        return;
    }
    tputs(tgoto(tgetstr("cm", NULL), (*pos).curx-1, (*pos).cury), 0, ft_iputchar);
    tputs(tgetstr("im", NULL), 0, ft_iputchar);          //insert mode
    write(STDOUT_FILENO, &rb, rr);
    if (!pos->curln && pos->len > pos->width - pos->start)
    {
    	tputs(tgetstr("sc", NULL), 0, ft_iputchar);      // save cursor position
        tputs(tgoto(tgetstr("cm", NULL), 0, (*pos).cury + 1), 0, ft_iputchar);
        tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of screen       
        write(STDOUT_FILENO, line + pos->width - pos->start, pos->len);//write the rest
        tputs(tgetstr("rc", NULL), 0, ft_iputchar);      // restore cursor position
    }
    else if (pos->curln && pos->len > pos->width - pos->start + pos->width*pos->curln)
    {
    	tputs(tgetstr("sc", NULL), 0, ft_iputchar);      // save cursor position
        tputs(tgoto(tgetstr("cm", NULL), 0, (*pos).cury + 1), 0, ft_iputchar);
        tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of screen       
        write(STDOUT_FILENO, line + pos->width - pos->start + pos->width*pos->curln, pos->len);//write the rest
        tputs(tgetstr("rc", NULL), 0, ft_iputchar);      // restore cursor position
    }
    if (line[pos->i])   //if it's at the middle of line
    {
            j = pos->len + 1;
            while (j > pos->i)
            {
                line[j] = line[j - 1];
                --j;
            }
   		pos->height = cmd_height(pos, line);
    }
	if (pos->curx == pos->width)
	{
		++(*pos).cury;
		++(*pos).curln;
		++(*pos).height;
		pos->curx = 0;
		tputs(tgetstr("do", NULL), 0, ft_iputchar);
		tputs(tgetstr("cr", NULL), 0, ft_iputchar);
	}
	if (pos->curx < pos->width)
		++(*pos).curx;
    line[pos->i] = (char)rb;
    ++(*pos).len;
    if (pos->i < pos->len)
        ++(*pos).i;
	tputs(tgetstr("ei", NULL), 0, ft_iputchar);          //end of insertion mode
}

void    read_line(char *line, int start)
{
	int         rr;
    uint64_t    rb;
    t_cpos		pos;
    int 		cmd;

    cmd = g_hstr_nb;
    if (start == 0)
	    pos.start = type_prompt();
	else
		pos.start = 0;
	ft_bzero(line, MAXLINE - start);
	init_position(&pos, start);
	rb = 0;
	while ((rr = read(STDIN_FILENO, &rb, 8)) > 0)
	{
//		printf("> %ld\n", rb);
		if (ft_isprint(rb))
			print(line, &pos, rb, rr);
		else if (rb == K_CTRL_C || rb == K_ENTER)
		{
			move_to_border(K_END, line, &pos);
       		ft_printf("\n");
       		(rb == K_CTRL_C) ? ft_bzero(line, MAXLINE) : (line[pos.len] = '\n');
       		return;
   		}
		else
			check_key(line, &pos, rb, &cmd);
		rb = 0;
	}
	//line[pos.i] = '\n';
    return;
}


/*
 		- shift->, shift<-
		- copy/paste/cut

   			
		* history (cursor at the last symbol) + segfault
		* ctrl + left, ctrl + right
		* ctrl + up, ctrl + down
		* HOME, END 					OK
		* cmd_height, strlen(prompt)	OK
		* print - last symbol in first line is empty
		* deleting 1st symbol removes 2 from screen
		* left after END jumps through 2 symbols
		* calculate end of line and positions as strlen and width
*/
