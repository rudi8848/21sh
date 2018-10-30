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

void	init_position(t_cpos *pos, int start)
{
	get_curpos(pos);
	pos->startline = start;
	pos->width = ft_get_width();
	pos->curln = 0;
	pos->height = 1;
	pos->len = 0;
	pos->i = 0;
	if (start)
		pos->start = pos->curx;
//	print_pos(pos);
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

//-----------------------------------------------------------------------------
void print_pos(t_cpos *pos)
{
	ft_printf("x: %d, y: %d, start: %d\n", (*pos).curx, (*pos).cury, (*pos).start);
	ft_printf("pos->curln: %d, pos->width: %d, pos->height: %d\n", (*pos).curln, (*pos).width,(*pos).height);
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
        		pos->curx = (pos->len - first)  % (pos->width)+1;
			if (pos->curx == 0)
				pos->curx = 1;
        		tputs(tgoto(tgetstr("cm", NULL),  (*pos).curx-1, (*pos).cury), 0, ft_iputchar);
        	}
        }
	}
}

void	delete_char(uint64_t rb, char *line, t_cpos *pos)
{
	int j;
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
               j++;
            }
            tputs(tgoto(tgetstr("cm", NULL), (*pos).curx - 1, (*pos).cury), 0, ft_iputchar);
            tputs(tgetstr("sc", NULL), 0, ft_iputchar);      // save cursor position
           	tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of screen       
            write(STDOUT_FILENO, line + pos->i , pos->len);//write the rest
	pos->height = cmd_height(pos, line);
            tputs(tgetstr("rc", NULL), 0, ft_iputchar);      // restore cursor position
        }
	}
	/*
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
                    j++;
                }
                TERM_BACK
            }
        }
	*/
}

void	move_history(uint64_t rb, char *line, t_cpos *pos, int *cmd)
{
	if (rb == K_DOWN)
        {
        	//	here will be history navigation
   			if (g_hstr_nb)
   			{
	        	ft_bzero(line, MAXLINE);
	        	init_position(pos, 1);
	        	
	        	tputs(tgoto(tgetstr("LE", NULL), 0, pos->i), 0, ft_iputchar);
	        	tputs(tgetstr("sc", NULL), 0, ft_iputchar);
	        	
	        	tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of line
				
				ft_strcpy(line, g_history[*cmd]);
				if (*cmd < g_hstr_nb -1)
					++(*cmd);
	            pos->len = ft_strlen(line);
	            pos->i = pos->len;
	            pos->height = cmd_height(pos, line);
		        ft_printf("%s",line);
		        move_to_border(K_END, line, pos);
		        //tputs(tgoto(tgetstr("cm", NULL), (*pos).curx, (*pos).cury), 0, ft_iputchar);
		        //tputs(tgetstr("rc", NULL), 0, ft_iputchar); 
		        //tputs(tgoto(tgetstr("RI", NULL), 0, pos->i), 0, ft_iputchar);
        	}
            //TERM_BELL         // bell
        }
        else if (rb == K_UP)
        {
        	if (g_hstr_nb)
        	{
	        	ft_bzero(line, MAXLINE);
	        	init_position(pos, 1);

	           	tputs(tgoto(tgetstr("LE", NULL), pos->start, pos->cury), 0, ft_iputchar);
	           	tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of line
	        	tputs(tgetstr("sc", NULL), 0, ft_iputchar);
	            //----------------------------------
	        	if (*cmd)
	        	{
					--(*cmd);
					ft_strcpy(line, g_history[*cmd]);
	        	}
	        				
	            //----------------------------------
	            /*
	            pos->len = ft_strlen(line);
	            pos->i = pos->len;
	        	ft_printf("%s",line);
	            tputs(tgetstr("rc", NULL), 0, ft_iputchar); 
	        	tputs(tgoto(tgetstr("RI", NULL), 0, pos->i), 0, ft_iputchar);
        		*/pos->len = ft_strlen(line);
	            pos->i = pos->len;
	            pos->height = cmd_height(pos, line);
		        ft_printf("%s",line);
		        move_to_border(K_END, line, pos);
        	}
           //TERM_BELL          // bell
        }
}

void	check_key(char *line, t_cpos *pos, uint64_t rb, int *cmd)
{
	if (rb == K_RIGHT || rb == K_LEFT)
		ft_move(rb, line, pos);
	else if (rb == K_DOWN || rb == K_UP)
		move_history(rb, line, pos, cmd);
	else if (/*rb == K_BSPACE ||*/ rb == K_DELETE || rb == K_CTRL_D)
		delete_char(rb, line, pos);
	else if (rb == K_HOME || rb == K_END)
		move_to_border(rb, line, pos);
    else if (rb == K_ESC)
        ft_exit();
	else if (rb == K_BSPACE)
		print_pos(pos);
    /*
    else if (rb == K_CTRL_UP || rb == K_CTRL_DOWN)
    {
		
    }
	else if (rb == K_CTRL_RIGHT || rb == K_CTRL_LEFT)
		jump(rb, line, pos);*/
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
        tputs(tgoto(tgetstr("cm", NULL), (*pos).curx-1, (*pos).cury), 0, ft_iputchar);
    tputs(tgetstr("im", NULL), 0, ft_iputchar);          //insert mode
    write(STDOUT_FILENO, &rb, rr);
    if (!pos->curln && pos->len > pos->width - pos->start)
    {
            tputs(tgetstr("sc", NULL), 0, ft_iputchar);      // save cursor position
    
        tputs(tgoto(tgetstr("cm", NULL), 0, (*pos).cury+1), 0, ft_iputchar);
           tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of screen       
            write(STDOUT_FILENO, line + pos->width - pos->start, pos->len);//write the rest
           tputs(tgetstr("rc", NULL), 0, ft_iputchar);      // restore cursor position
    }
    else if (pos->curln && pos->len > pos->width - pos->start + pos->width*pos->curln)
    {
            tputs(tgetstr("sc", NULL), 0, ft_iputchar);      // save cursor position
        tputs(tgoto(tgetstr("cm", NULL), 0, (*pos).cury+1), 0, ft_iputchar);
           tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of screen       
            write(STDOUT_FILENO, line + pos->width - pos->start + pos->width*pos->curln, pos->len);//write the rest
           tputs(tgetstr("rc", NULL), 0, ft_iputchar);      // restore cursor position
    
    }
    if (line[pos->i])   //if it's at the middle of line
    {
	    //ft_printf("AAAAAAAAAAAAAA");
            j = pos->len + 1;
            while (j > pos->i)
            {
                line[j] = line[j - 1];
                j--;
            }

         //   tputs(tgetstr("sc", NULL), 0, ft_iputchar);      // save cursor position
           //tputs(tgetstr("cd", NULL), 0, ft_iputchar);      // delete end of screen       
            //write(STDOUT_FILENO, line + pos->i + 1, pos->len);//write the rest
           //tputs(tgetstr("rc", NULL), 0, ft_iputchar);      // restore cursor position
   		pos->height = cmd_height(pos, line);
    }
    
	if (pos->curx == pos->width)
	{
		++(*pos).cury;
		++(*pos).curln;
		++(*pos).height;
		pos->curx = 0;
		/*  smth wrong with last line  */
		//tputs(tgoto(tgetstr("cm", NULL), (*pos).curx, (*pos).cury), 0, ft_iputchar);
		//write(STDOUT_FILENO, line + pos->i+1, pos->len );
	//	tputs(tgoto(tgetstr("cm", NULL), (*pos).curx, (*pos).cury), 0, ft_iputchar);
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
//	printf("start: %d\n", pos.start);
	ft_bzero(line, MAXLINE - start);
	init_position(&pos, start);
	rb = 0;
	while ((rr = read(STDIN_FILENO, &rb, 8)) > 0)
	{
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
		- history (cursor at the last symbol)

		- print - last symbol in first line is empty
		- deleting 1st symbol removes 2 from screen
		- left after END jumps through 2 symbols
		- calculate end of line and positions as strlen and width

		* HOME, END 					OK
		* cmd_height, strlen(prompt)	OK
*/
