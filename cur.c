#include <sys/ioctl.h>
#include <stdio.h>
#include <termios.h>
#include <curses.h>
#include <term.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXWORD 256


int	get_curx(void)
{
	char buf[21];
	int ret = 0;
	memset(buf, 0, 21);
	write(0, "\033[6n", 4);
	ret = read(0, buf, 20);

	int nbr = atoi(&buf[2]);
	return nbr;
}

int	main(void)
{

	char buf[21];
	int ret = 0;
	memset(buf, 0, 21);
	write(0, "\033[6n", 4);
	ret = read(0, buf, 20);

	int nbr = atoi(&buf[2]);
	printf("> %d\n", nbr);

	return 0;
}


/*
int		main(void)
{
	struct winsize ws;
	struct termios saved;

//---------- GET WINSIZE
	if (ioctl(1, TIOCGWINSZ, &ws) != 0)
	{
		perror("get winsize");
		return 1;
	}
	printf("col: [%d], row: [%d]\n", ws.ws_col, ws.ws_row);*/
	/*
	char buf[12];
	memset(buf, 0, 12);
	read(0, buf, 12);
	printf("%s\n", buf);
	*/
//---------- GET TERMINAL
/*
	char buf[MAXWORD];
	 if ((tcgetattr(STDOUT_FILENO, &saved)) == -1)
    {
        printf("Cannot get terminal settings\n");
        exit(EXIT_FAILURE);
    }
    char *name = NULL;
    if (!(name = getenv("TERM")))
    {
        printf("TERM is not set\n");
        exit(EXIT_FAILURE);
    }
    if (tgetent(buf, name) < 1)
    {
        printf("Error at tgetent\n");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", name);*/
/*
    char *ptr;
    ptr = tgetstr("cm", &ptr);
    printf("%s\n", ptr);
    //char *ptr1 = tgoto(ptr, ws.ws_col - 10, ws.ws_row - 10);
    char *ptr1 = tgoto(ptr, 5, 5);
    printf("%s\n", ptr1);
    write(1, "DONE\n", 5);

*/
    //tputs(tgoto(tgetstr("RI", NULL), 0, ws.ws_col - 10), 0, putchar);
/*
    tputs(tgoto(tgetstr("cm", NULL), 10, 15), 0, putchar);

    char arr[12];
	memset(arr, 0, 12);
	read(0, arr, 12);
	
	sleep(1);
	tputs(tgoto(tgetstr("cm", NULL), 20, 50), 0, putchar);
	sleep(1);
printf("%s\n", arr);
*/
//	return 0;
//}
