#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>

int cin;
int cout;
int cerr;

void	restore(void)
{
	int newfd;
	if ((newfd = dup2( cin, 0)) < 0)
		perror("dup2");
	if ((newfd = dup2( cout, 1)) < 0)
		perror("dup2");
	if ((newfd = dup2( cerr, 2)) < 0)
		perror("dup2");
}

int		main(void)
{

	
	if ((cin = /*dup(0)*/dup2(STDIN_FILENO, 40)) < 0)
		perror("dup2");

	if ((cout = /*dup(1)*/dup2(STDOUT_FILENO, 41)) < 0)
		perror("dup2");
	if ((cerr = /*dup(2)*/dup2(STDERR_FILENO, 42)) < 0)
		perror("dup2");
	dprintf(4, "[1]\tcin: %d, cout: %d, cerr: %d\n", cin, cout, cerr);
/*	without fork	*/
	
	if (dup2(1, 2) < 0)
		perror("dup2");
	//dprintf(4, "[2]\tnewfd = %d; cin: %d, cout: %d, cerr: %d\n", newfd, cin, cout, cerr);
	if (write(2, ">\t1dollar!\n", 11) < 0)
			dprintf(4, "error\n");

	restore();
	
	//dprintf(4, "[3]\tnewfd = %d;cin: %d, cout: %d, cerr: %d\n", newfd,cin, cout, cerr);
	write(2, ">\tNo dollar\n", 12);

/*	with fork	*/
	
	dup2(1, 2);
	if (!fork())
	{
		write(2, ">\tdollar!\n", 10);
		exit(0);
	}
	else
	{
		wait(NULL);
	}
	restore();
	
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
	restore();

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
	restore();

	const char *args[] = {"/bin/echo", "No dollar", NULL};
	dup2(2, 1);
	if (!fork())
	{
		if (execvp(args[0], (char**)args) < 0)
			perror("execvp");
		exit(0);
	}
	else
	{
		wait(NULL);
	}
	restore();
	return 0;	
}
