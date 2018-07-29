#include "21sh.h"
/*
void	execute(t_cmds *cmds)
{
	
}
*/
	char *command1[] = {"ls", "-l", NULL};
	char *command2[] = {"grep", "author", NULL};
	char *command3[] = {"base64", NULL};

void	print(t_cmds *cmds)
{
	int i = 0;
	while (i < cmds->nbr_of_cmds)
	{
		int j = 0;
		while (cmds->cmd_list[i][j] != NULL)
		{
			printf("[%d][%d] %s\n", i, j, cmds->cmd_list[i][j]);
			j++;
		}
		i++;
	}
}

void	execute(t_cmds *cmds)
{
//	printf("--> %s <--\n", __FUNCTION__);
	// save in/out
	int tmpin = dup(0);
	int tmpout = dup(1);

	// set the initial input 
	int fdin;
	if (cmds->infile)
	{
		fdin = open(cmds->infile, O_RDONLY);
	}
	else
	{
		// use default input
		fdin = dup(tmpin);
	}
//fprintf(stderr, "start: tmpin = %d, tmpout = %d, fdin = %d\n", tmpin, tmpout, fdin);
	int ret;
	int fdout;
	int i = 0;
	while (i < cmds->nbr_of_cmds )
	{
//		fprintf(stderr, "----> in loop [%d]\n", i);
		// redirect input 
		if (dup2(fdin, 0) == ERROR)
		{
			perror("dup2");
			exit(1);
		}
		close(fdin);

		if (i == cmds->nbr_of_cmds - 1)
		{
//			fprintf(stderr, "the last \n");
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
//			fprintf(stderr, "not last \n");
			//not last
			int fdpipe[2];
			if (pipe(fdpipe) == ERROR)
			{
				perror("pipe");
				exit(1);
			}
			fdout = fdpipe[1];
			fdin = fdpipe[0];
		}
		if (dup2(fdout, 1) == ERROR)
		{
			perror("dup2");
			exit(1);
		}
		
		close(fdout);
//fprintf(stderr, "pipe: tmpin = %d, tmpout = %d, fdin = %d\n", tmpin, tmpout, fdin);
//	fprintf(stderr,"fork\n");
		ret = fork();
		if (ret == ERROR)
		{
			perror("fork");
			exit(1);
		}
		if (ret == CHILD)
		{
//fprintf(stderr, "in child: tmpin = %d, tmpout = %d, fdin = %d\n", tmpin, tmpout, fdin);
//			fprintf(stderr,"CHILD: %s\n", cmds->cmd_list[i][0]);
			execvp(cmds->cmd_list[i][0], cmds->cmd_list[i]);
			perror("execvp");
			exit(1);
		}
		else
		{
//			fprintf(stderr, "wait\n");
			wait(NULL);
		}
	
//		fprintf(stderr, "----> end of loop[%d]\n", i);

		i++;
	}

	// restore in/out defaults
	if (dup2(tmpin, 0) == ERROR)
		perror("dup2");
	if (dup2(tmpout, 1) == ERROR)
		perror("dup2");
	close(tmpin);
	close(tmpout);

	if (!cmds->bg)
	{
//		fprintf(stderr, "waitpid\n");
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


	cmds->cmd_list[0] = command1;

	cmds->cmd_list[1] = command2;

	cmds->cmd_list[2] = command3;
}

int main(void)
{
	t_cmds *cmd_table;
	
	cmd_table = (t_cmds*)malloc(sizeof(t_cmds));
	set_values(cmd_table);
//	print(cmd_table);
	execute(cmd_table);

	return 0;
}
