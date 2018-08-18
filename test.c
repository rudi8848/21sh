#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
# define STR "qwerty"

int		main()
{
	//printf("%d\n", sizeof(STR));
	//printf("%d\n", _POSIX_JOB_CONTROL);
	//write(1, STR, sizeof(STR));
	//
	int pfd[2];
	int flags;
	int i;

	if (pipe(pfd) != -1)
	{
		for (i = 0; i < 2; i++)
		{
			if ((flags = fcntl(pfd[i], F_GETFL)) != -1)
			{
				if ((flags & O_ACCMODE) == O_RDONLY)
					printf("pfd[%d] O_RDONLY\n", i);
				if ((flags & O_ACCMODE) == O_WRONLY)
					printf("pfd[%d] O_WRONLY\n", i);
				if ((flags & O_ACCMODE) == O_RDWR)
					printf("pfd[%d] O_RDWR\n", i);
			}
		}
	}
	return 0;
}
