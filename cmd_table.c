#include "21sh.h"

void	execute(t_cmds *cmds)
{
	printf("--> %s <--\n", __FUNCTION__);
	/* save in/out */
	int tmpin = dup(0);
	int tmpout = dup(1);

	/* set the initial input */
	int fdin;
	if (cmds->infile)
	{
		fdin = open(cmds->infile, O_RDONLY);
	}
	else
	{
		/* use default input */
		fdin = dup(tmpin);
	}

	int ret;
	int fdout;
	int i = 0;
	while (i < cmds->nbr_of_cmds)
	{
		printf("-> in loop [%d]\n", i);
		/* redirect input */
		if (dup2(fdin, 0) == ERROR)
		{
			perror("dup2");
			exit(1);
		}
		close(fdin);

		if (i == cmds->nbr_of_cmds - 1)
		{
			printf("the last \n");
			if (cmds->outfile)
			{
				fdout = open(cmds->outfile, O_WRONLY, O_CREAT);
				if (fdout == ERROR)
				{
					perror("open");
					exit(1);
				}
			}
			else
			{
				fdout = dup(tmpout);
				if (fdout == ERROR)
				{
					perror("dup");
					exit(1);
				}
			}
		}
		else
		{
			printf("not last \n");
			/* not last */
			int fdpipe[2];
			if (pipe(fdpipe) == ERROR)
			{
				perror("pipe");
				exit(1);
			}
			fdout = fdpipe[1];
			fdin = fdpipe[0];
		}

		ret = fork();
		if (ret == ERROR)
		{
			perror("fork");
			exit(1);
		}
		if (ret == CHILD)
		{
			printf("CHILD: %s\n", cmds->cmd_list[i][0]);
			execvp(cmds->cmd_list[i][0], cmds->cmd_list[i]);
			exit(0);
		}
		i++;
	}

	/* restore in/out defaults */
	dup2(tmpin, 0);
	dup2(tmpout, 1);
	close(tmpin);
	close(tmpout);

	if (!cmds->bg)
	{
		waitpid(ret, NULL, 0);
	}
}

void set_values(t_cmds *cmds)
{
	cmds->infile = NULL;
	cmds->outfile = NULL;
	cmds->errfile = NULL;
	cmds->in_fd = 0;
	cmds->out_fd = 1;
	cmds->err_fd = 2;
	cmds->bg = 0;

	cmds->nbr_of_cmds = 3;
	
	cmds->cmd_list = (char ***)malloc(sizeof(char**) * cmds->nbr_of_cmds);
	/* 		 ls -l | grep author | base64 		*/

	char *command1[] = {"ls", "-l", NULL};
	cmds->cmd_list[0] = command1;
	char *command2[] = {"grep", "author", NULL};
	cmds->cmd_list[1] = command2;
	char *command3[] = {"base64", NULL};
	cmds->cmd_list[2] = command3;	

}

int main(void)
{
	t_cmds cmd_table;

	set_values(&cmd_table);
	execute(&cmd_table);

	return 0;
}