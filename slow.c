#include <unistd.h>

int		main(void)
{
	while (1)
	{
		write(1, "slowww...\n", 10);
		sleep(2);
	}
	return 0;
}
