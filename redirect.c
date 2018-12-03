#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>


int		main(void)
{

/*	without fork	*/
	/*
	dup2(1, 2);
	write(2, ">\thello!\n", 9);
	*/

/*	with fork	*/
	/*
	dup2(1, 2);
	if (!fork())
	{
		write(2, ">\tchild!\n", 9);
		exit(0);
	}
	else
	{
		wait(NULL);
	}
	*/
	/*
	dup2(2, 1);
	if (!fork())
	{
		write(1, ">\tNo dollar\n", 12);
		exit(0);
	}
	else
	{
		wait(NULL);
	}
	*/
/*
	const char *argv[] = {"/bin/rm", "asdf", NULL};
	dup2(1, 2);
	if (!fork())
	{
		if (execvp(argv[0], (char**)argv) < 0)
			perror("execvp");
		exit(0);
	}
	else
	{
		wait(NULL);
	}
*/

	const char *argv[] = {"/bin/echo", "No dollar", NULL};
	dup2(2, 1);
	if (!fork())
	{
		if (execvp(argv[0], (char**)argv) < 0)
			perror("execvp");
		exit(0);
	}
	else
	{
		wait(NULL);
	}

	return 0;	
}
