#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct s_cmd_list
{
	char **args;
	struct s_cmd_list *next;
};

extern char** environ;

void ft_pipe(struct s_cmd_list *cmd)
{
	int pfd[2];
	int ret = 0;

	ret = pipe(pfd);
	if (ret != 0)
	{
		perror("pipe");
		exit(1);
	}
	if (fork() == 0)
	{
		close(STDOUT_FILENO);
		dup2(pfd[1], STDOUT_FILENO);
		close(pfd[1]);
		close(pfd[0]);
		execve(cmd->next->args[0], cmd->next->args, environ);
	}
	else
	{
		close(STDIN_FILENO);
		dup2(pfd[0], STDIN_FILENO);
		close(pfd[1]);
		close(pfd[0]);
		execve(cmd->args[0], cmd->args, environ);
	}
	wait(NULL);
	wait(NULL);
}

void	ft_push(struct s_cmd_list *cmd, int argc, char **argv, int i)
{
	struct s_cmd_list *first;
	struct s_cmd_list *second;

	first = (struct s_cmd_list*)malloc(sizeof(struct s_cmd_list));
	second = (struct s_cmd_list*)malloc(sizeof(struct s_cmd_list));

	if (!first || !second)
	{
		perror("malloc");
		exit(1);
	}
	first->next = second;
	second->next = NULL;
	first->args = (char**)malloc(sizeof(char*)* i);
	int j = 1;
	while (j < i)
	{
		first->args[j] = argv[j];
		printf("1 - %s\n", argv[j]);
		j++;
	}
	first->args[j] = NULL; 
//-------------------------------------
	j = i +1;
 	second->args = (char**)malloc(sizeof(char*)* (argc - i));
	while (j < argc)
	{
		second->args[j] = argv[j];
		printf("2 - %s\n", argv[j]);
		j++;
	}
	second->args[j] = NULL; 

}

int main(int argc, char **argv)
{
	struct s_cmd_list *cmd;
	/*
		split by '|', every single part is a elem. of list 
	*/

	int i = 1;
	if (argc > 1)
	{
		while (i < argc)
		{
			if (!strcmp("@", argv[i]))
			{
				printf("%d %s\n", i, argv[i]);
				ft_push(cmd, argc, argv, i);
			}
			i++;
		}
		ft_pipe(cmd);
	}
	else
		printf("no args\n");
	
	return 0;
}