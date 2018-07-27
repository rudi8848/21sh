#include <stdio.h>
#include <unistd.h>


/*
	how to check is fd opened and available? 
*/
int main(void)
{
	char *name;

	if (isatty(0))
	{
		name = ttyname(0);
		if (!name)
			name = "no name";
	}
	else
		name = "no tty";
	printf("fd 0: %s\n", name);

	if (isatty(1))
	{
		name = ttyname(1);
		if (!name)
			name = "no name";
	}
	else
		name = "no tty";
	printf("fd 1: %s\n", name);

	if (isatty(2))
	{
		name = ttyname(2);
		if (!name)
			name = "no name";
	}
	else
		name = "no tty";
	printf("fd 2: %s\n", name);

	return 0;
}