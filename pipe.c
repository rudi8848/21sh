#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

struct s_cmd_list
{
	char **args;
	struct s_cmd_list *next;
};

//extern char** environ;

void ft_pipe(struct s_cmd_list *cmd)
{
	printf("%s\n", __FUNCTION__);
	int pfd[2];
	int ret = 0;
	pid_t pid;
	ret = pipe(pfd);
	if (ret != 0)
	{
		perror("pipe");
		exit(1);
	}
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exit(1);
	}
	else if (pid == 0)
	{
		printf("---> %s\n", cmd->next->args[0]);
		close(STDOUT_FILENO);
		dup2(pfd[1], STDOUT_FILENO);
		close(pfd[1]);
		close(pfd[0]);
		ret = execvp(cmd->next->args[0], cmd->next->args);
		if (ret != 0)
		{
			perror("execvp");
			exit(1);
		}
	}
	else
	{
		printf("---> %s\n", cmd->args[0]);
		close(STDIN_FILENO);
		dup2(pfd[0], STDIN_FILENO);
		close(pfd[1]);
		close(pfd[0]);
		ret = execvp(cmd->args[0], cmd->args);
		if (ret != 0)
		{
			perror("execvp");
			exit(1);
		}

	}
	wait(NULL);
	wait(NULL);
}

void	ft_push(struct s_cmd_list **cmd, int argc, char **argv, int i)
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
	*cmd = first;
	first->next = second;
	second->next = NULL;
	first->args = (char**)malloc(sizeof(char*)* i);
	int j = 0;
	while (j + 1 < i)
	{
		first->args[j] = strdup(argv[j + 1]);
		printf("1 - %s\n", first->args[j]);
		j++;
	}
	first->args[j] = NULL; 
//-------------------------------------
	j = 0;
 	second->args = (char**)malloc(sizeof(char*)* (argc - i + 1));
	while (j+i+1 < argc)
	{
		second->args[j] = strdup(argv[i + 1 + j]);
		printf("2 - %s\n", second->args[j]);
		j++;
	}
	second->args[j] = NULL; 

}
void ft_print(struct s_cmd_list *list)
{
	printf("%s\n", __FUNCTION__);
	struct s_cmd_list *cmd;

	cmd = list;
	while (cmd)
	{
		int i = 0;
		while (cmd->args[i])
		{
			printf("args[%d] %s\n",i, cmd->args[i]);
			i++;
		}
		cmd = cmd->next;
	}
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
				ft_push(&cmd, argc, argv, i);
				break;
			}
			i++;
		}
		ft_print(cmd);
		ft_pipe(cmd);
	}
	else
		printf("no args\n");
	
	return 0;
}
