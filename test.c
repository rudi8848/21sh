#include <unistd.h>
#include <stdio.h>

# define STR "qwerty"

int		main()
{
	//printf("%d\n", sizeof(STR));
	printf("%d\n", _POSIX_JOB_CONTROL);
	//write(1, STR, sizeof(STR));
	return 0;
}
