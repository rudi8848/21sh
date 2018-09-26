/* ladsh.c*/

#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_COMMAND_LEN 250

#define JOB_STATUS_FORMAT "[%d] %-22s %.40s\n"

struct jobSet
{
	struct job *head;
	struct job *fg;
};

enum redirectionType {REDIRECT_INPUT, REDIRECT_OVERWRITE, REDIRECT_APPEND};

struct redirectionSpecifier {
	enum redirectionType type;
	int fd;
	char *filename;
};

struct childProgram {
	pid_t pid;
	char **argv;
	int numRedirections;
	struct redirectionSpecifier *redirections;
	glob_t globResult;
	int freeGlob;
	int isStopped;
};

struct job {
	int jobId;
	int numProgs;
	int runningProgs;
	char *text;
	char *cmdBuf;
	pid_t pgrp;
	struct childProgram *progs;
	struct job *next;
	int stoppedProgs;
};

void	freeJob(struct job *cmd)
{
	int i;
	for (i = 0; i < cmd->numProgs; i++){
		free(cmd->progs[i].argv);
		if (cmd->progs[i].redirections)
			free(cmd->progs[i].redirections);
		if (cmd->progs[i].freeGlob)
			globfree(&cmd->progs[i].globResult);
	}
	free(cmd->progs);
	if (cmd->text) free(cmd->text);
	free(cmd->cmdBuf);
}

int 	getCommand(FILE *source, char *command) {
	if (source == stdin) {
		printf("# ");
		fflush(stdout);
	}

	if (!fgets(command, MAX_COMMAND_LEN, source)){
		if (source == stdin) printf("\n");
		return 1;
	}

	//
	command[strlen(command) -1] = '\0';

	return 0;
}

void	globLastArgument (struct childProgram *prog, int *argcPtr, int *argcAllocedPtr)
{
	int argc = *argcPtr;
	int argcAlloced = *argcAllocedPtr;
	int rc;
	int flags;
	int i;
	char *src, *dst;

	if (argc > 1) {
		flags = GLOB_APPEND;
		i = prog->globResult.gl_pathc;
	} else {
		prog->freeGlob = 1;
		flags = 0;
		i = 0;
	}

	rc = glob(prog->argv[argc -1], flags, NULL, &prog->globResult);
	if (rc == GLOB_NOSPACE) {
		fprintf(stderr, "No space for globbing\n");
		return;
	} else if (rc == GLOB_NOMATCH ||
		(!rc && (prog->globResult.gl_pathc -i) == 1 &&
			!strcmp(prog->argv[argc -1],
				prog->globResult.gl_pathv[i])))
	{
		src = dst = prog->argv[argc - 1];
		while (*src) {
			if (*src != '\\') *dst++ = *src;
			src++;
		}
		*dst = '\0';
	} else if (!rc) {
		argcAlloced += (prog->globResult.gl_pathc - i);
		prog->argv = realloc(prog->argv,
			argcAlloced * sizeof(*prog->argv));
		memcpy(prog->argv + (argc - 1),
			prog->globResult.gl_pathv + i,
			sizeof(*(prog->argv)) *
			(prog->globResult.gl_pathc - i));
		argc += (prog->globResult.gl_pathc - i - 1);
	}

	*argcAllocedPtr = argcAlloced;
	*argcPtr = argc;
}






int parseCommand(char **commandPtr, struct job *job, int *isBg) {
	char *command;
	char *returnCommand = NULL;
	char *src, *buf, *chptr;
	int argc = 0;
	int done = 0;
	int argvAlloced;
	int i;
	char quote = '\0';
	int count;
	struct childProgram *prog;


	while(**commandPtr && isspace(**commandPtr))
		(*commandPtr)++;

	if (!**commandPtr || (**commandPtr == '#')) {
		job->numProgs = 0;
		*commandPtr = NULL;
		return 0;
	}

	*isBg = 0;
	job->numProgs = 1;
	job->progs = malloc(sizeof(*job->progs));

	//





	job->cmdBuf = command = calloc(1, strlen(*commandPtr) + 1);
	job->text = NULL;

	prog = job->progs;
	prog->numRedirections = 0;
	prog->redirections = 0;
	prog->freeGlob = 0;
	prog->isStopped = 0;

	argvAlloced = 5;
	prog->argv = malloc(sizeof(*prog->argv) * argvAlloced);
	prog->argv[0] = job->cmdBuf;

	buf = command;
	src = *commandPtr;
	while (*src && !done) {
		if (quote == *src) {
			quote = '\0';
		} else if (quote) {
			if (*src == '\\') {
				src++;
				if (!*src) {
				fprintf(stderr, "Expected symbol after \\\n");
				freeJob(job);
				return 1;
			}


	
		if (*src != quote) *buf++ = '\\';	
		} else if (*src == '*' || *src == '?' || *src == '[' ||
			*src == ']')
		*buf++ = '\\';
		*buf++ = *src;
	} else if (isspace(*src)) {
		if (*prog->argv[argc]) {
			buf++, argc++;


			if ((argc + 1) == argvAlloced) {
				argvAlloced += 5;
				prog->argv = realloc(prog->argv,
					sizeof(*prog->argv) * argvAlloced);
			}
			prog->argv[argc] = buf;

			globLastArgument(prog, &argc, &argvAlloced);
		}
	} else switch (*src) {
		case '"':
		case '\'':
			quote = *src;
			break;

		case '#':
		done = 1;
		break;

		case '>':
		case '<':
			i = prog->numRedirections++;
			prog->redirections = realloc(prog->redirections,
				sizeof(*prog->redirections) * (i + 1));

			prog->redirections[i].fd = -1;
			if (buf != prog->argv[argc]) {


				prog->redirections[i].fd = 
				strtol(prog->argv[argc], &chptr, 10);

				if (*chptr && *prog->argv[argc]) {
					buf++, argc++;
					globLastArgument(prog, &argc, &argvAlloced);
				}
			}

		if (prog->redirections[i].fd == -1) {
			if (*src == '>')
				prog->redirections[i].fd = 1;
			else
				prog->redirections[i].fd = 0;
		}

		if (*src++ == '>') {
			if (*src == '>') {
				prog->redirections[i].type = REDIRECT_APPEND;
				src++;
			} else {
				prog->redirections[i].type = REDIRECT_OVERWRITE;
			}
		} else {
			prog->redirections[i].type = REDIRECT_INPUT;
		}


		chptr = src;
		while (isspace(*chptr)) chptr++;

		if (!*chptr) {
			fprintf(stderr, "Expected filename after %c \n",
				*src);
			freeJob(job);
			return 1;
		}

		prog->redirections[i].filename = buf;
		while (*chptr && !isspace(*chptr))
			*buf++ = *chptr++;

		src = chptr - 1;
		prog->argv[argc] = ++buf;
		break;

		case '|':
		//
			if (*prog->argv[argc]) argc++;
			if (!argc) {
				fprintf(stderr, "Empty command in pipe\n");
				freeJob(job);
				return 1;
			}
			prog->argv[argc] = NULL;


			job->numProgs++;
			job->progs = realloc(job->progs,
				sizeof(*job->progs) *
				job->numProgs);
			prog = job->progs + (job->numProgs - 1);
			prog->numRedirections = 0;
			prog->redirections = NULL;
			prog->freeGlob = 0;
			argc = 0;

			argvAlloced = 5;
			prog->argv = malloc(sizeof(*prog->argv) *
				 argvAlloced);
			prog->argv[0] = ++buf;

			src++;
			while (*src && isspace(*src)) src++;

			if (!*src) {
				fprintf(stderr, "Empty command in pipe\n");
				return 1;
			}
			src--;

			break;

			case '&':
				*isBg = 1;
			case ';':
				done = 1;
				returnCommand = *commandPtr + (src - *commandPtr) + 1;
				break;

			case '\\':
				src++;
				if (!*src) {
					freeJob(job);
					fprintf(stderr, "Expected symbol after \\\n");
					return 1;
				}
			if (*src == '*' || *src == '[' || *src == ']'
				|| *src == '?')
				*buf++ = '\\';

			default:
				*buf++ = *src;
	}

	src++;
}

if (*prog->argv[argc]) {
	argc++;
	globLastArgument(prog, &argc, &argvAlloced);
}
if (!argc) {
	freeJob(job);
	return 0;
}
prog->argv[argc] = NULL;

if (!returnCommand) {
	job->text = malloc(strlen(*commandPtr) + 1);
	strcpy(job->text, *commandPtr);
} else {


	count = returnCommand - *commandPtr;
	job->text = malloc(count + 1);
	strncpy(job->text, *commandPtr, count);
	job->text[count] = '\0';
}

*commandPtr = returnCommand;

return 0;
}

int setupRedirections(struct childProgram *prog) {
	int i;
	int openfd;
	int mode;
	struct redirectionSpecifier *redir = prog->redirections;

	for (i = 0; i < prog->numRedirections; i++, redir++) {
		switch (redir->type) {
			case REDIRECT_INPUT:
				mode = O_RDONLY;
				break;
			case REDIRECT_OVERWRITE:
				mode = O_RDWR | O_CREAT | O_TRUNC;
				break;
			case REDIRECT_APPEND:
				mode = O_RDWR | O_CREAT | O_APPEND;
				break;
		}

		openfd = open(redir->filename, mode, 0666);
		if (openfd < 0) {



			fprintf(stderr, "Open error %s: %s\n",
			 redir->filename, strerror(errno));
			return 1;
		}

		if (openfd != redir->fd) {
			dup2(openfd, redir->fd);
			close(openfd);
		}
	}

	return 0;
}

int runCommand(struct job newJob, struct jobSet *jobList,
		int inBg) {
	struct job *job;
	char *newdir, *buf;
	int i, len;
	int nextin, nextout;
	int pipefds[2];
	char *statusString;
	int jobNum;
	int controlfds[2];



	if (!strcmp(newJob.progs[0].argv[0], "exit")) {
	
		exit(0);
	} else if (!strcmp(newJob.progs[0].argv[0], "pwd")) {
		len = 50;
		buf = malloc(len);
		while (!getcwd(buf, len) && errno == ERANGE) {
			len += 50;
			buf = realloc(buf, len);
		}
		printf("%s\n", buf);
		free(buf);
		return 0;
	} else if (!strcmp(newJob.progs[0].argv[0], "cd")) {
		if (!newJob.progs[0].argv[1] == 1)
			newdir = getenv("HOME");
		else
			newdir = newJob.progs[0].argv[1];
		if (chdir(newdir))
			printf("cannot change directory: %s\n",
				 strerror(errno));
		return 0;
	} else if (!strcmp(newJob.progs[0].argv[0], "jobs")) {
		for (job = jobList->head; job; job = job->next) {
			if (job->runningProgs == job->stoppedProgs)
				statusString = "Stopped";
			else
				statusString = "Running";

			printf(JOB_STATUS_FORMAT, job->jobId, statusString,
				job->text);
		}
		return 0;
	} else if (!strcmp(newJob.progs[0].argv[0], "fg") ||
		!strcmp(newJob.progs[0].argv[0], "bg")) {
		if (!newJob.progs[0].argv[1] || newJob.progs[0].argv[2]) {
			fprintf(stderr,
			 "%s: one argument expected\n", 
			 newJob.progs[0].argv[0]);
			return 1;
		}

		if (sscanf(newJob.progs[0].argv[1], "%%%d", &jobNum) != 1) {
			fprintf(stderr, "%s: wrong argument: %s\n", 
				newJob.progs[0].argv[0],
				newJob.progs[0].argv[1]);
			return 1;
		}

		for (job = jobList->head; job; job = job->next)
			if (job->jobId == jobNum) break;

		if (!job) {
			fprintf(stderr, "%s: unknown job %d\n",
			newJob.progs[0].argv[0], jobNum);
			return 1;
		}

		if (*newJob.progs[0].argv[0] == 'f') {


			if (tcsetpgrp(0, job->pgrp))
				perror("tcsetpgrp");
			jobList->fg = job;
		}


		for (i = 0; i< job->numProgs; i++)
			job->progs[i].isStopped = 0;

		kill(-job->pgrp, SIGCONT);

		job->stoppedProgs = 0;

		return 0;
	}

	nextin = 0, nextout = 1;
	for (i = 0; i<newJob.numProgs; i++) {
		if ((i + 1) < newJob.numProgs) {
			pipe(pipefds);
			nextout = pipefds[1];
		} else {
			nextout = 1;
		}

		pipe(controlfds);

		if (!(newJob.progs[i].pid = fork())) {
			signal(SIGTTOU, SIG_DFL);

			close(controlfds[1]);

			read(controlfds[0], &len, 1);
			close(controlfds[0]);

			if (nextin != 0) {
				dup2(nextin, 0);
				close(nextin);
			}

			if (nextout != 1) {
				dup2(nextout, 1);
				close(nextout);
			}


			setupRedirections(newJob.progs + i);

			execvp(newJob.progs[i].argv[0], newJob.progs[i].argv);
			fprintf(stderr, "execvp error for %s: %s\n",
			newJob.progs[i].argv[0],
			strerror(errno));
			exit(1);
		}



		setpgid(newJob.progs[i].pid, newJob.progs[0].pid);


		close(controlfds[0]);
		close(controlfds[1]);

		if (nextin != 0) close(nextin);
		if (nextout != 1) close(nextout);



		nextin = pipefds[0];
	}

	newJob.pgrp = newJob.progs[0].pid;


	newJob.jobId = 1;
	for (job = jobList->head; job; job = job->next)
		if (job->jobId >= newJob.jobId)
			newJob.jobId = job->jobId + 1;


		if (!jobList->head) {
			job = jobList->head = malloc(sizeof(*job));
		} else {
			for (job = jobList->head; job->next; job = job->next);
				job->next = malloc(sizeof(*job));
			job = job->next;
		}

		*job = newJob;
		job->next = NULL;
		job->runningProgs = job->numProgs;
		job->stoppedProgs = 0;

		if (inBg) {



			printf("[%d] %d\n", job->jobId,
				newJob.progs[newJob.numProgs - 1].pid);
		} else {
			jobList->fg = job;



			if (tcsetpgrp(0, newJob.pgrp))
				perror("tcsetpgrp");
		}

		return 0;
}

void	removeJob(struct jobSet *jobList, struct job *job) {
	struct job *prevJob;

	freeJob(job);
	if (job == jobList->head) {
		jobList->head = job->next;
	} else {
		prevJob = jobList->head;
		while (prevJob->next != job) prevJob = prevJob->next;
		prevJob->next = job->next;
	}

	free(job);
}



void	checkJobs(struct jobSet *jobList) {
	struct job *job;
	pid_t childpid;
	int status;
	int progNum;
	char *msg;

	while ((childpid = waitpid(-1, &status,
		 WNOHANG | WUNTRACED)) > 0) {
		for (job = jobList->head; job; job = job->next) {
			progNum = 0;
			while (progNum < job->numProgs &&
				job->progs[progNum].pid != childpid)
				progNum++;
			if (progNum < job->numProgs) break;
		}

		if (WIFEXITED(status) || WIFSIGNALED(status)) {

			job->runningProgs--;
			job->progs[progNum].pid = 0;

			if (!WIFSIGNALED(status))
				msg = "Terminated";
			else
				msg = strsignal(WTERMSIG(status));

			if (!job->runningProgs) {
				printf(JOB_STATUS_FORMAT, job->jobId,
					msg, job->text);
				removeJob(jobList, job);
			}
		} else {
			
			job->stoppedProgs++;
			job->progs[progNum].isStopped = 1;

			if (job->stoppedProgs == job->numProgs) {
				printf(JOB_STATUS_FORMAT, job->jobId, "Stopped", 
					job->text);
			}
		}
	}

	if (childpid == -1 && errno != ECHILD)
		perror("waitpid");
}

int main(int argc, const char **argv) {
	char command[MAX_COMMAND_LEN + 1];
	char *nextCommand = NULL;
	struct jobSet jobList = {NULL, NULL};
	struct job newJob;
	FILE *input = stdin;
	int i;
	int status;
	int inBg;

	if (argc > 2) {
		fprintf(stderr, "Unexpected argument\n");

		exit(1);
	} else if (argc == 2) {
		input = fopen(argv[1], "r");
		if (!input) {
			perror("fopen");
			exit(1);
		}
	}



	signal(SIGTTOU, SIG_IGN);

	while(1) {
		if (!jobList.fg) {



			checkJobs(&jobList);

			if (!nextCommand) {
				if (getCommand(input, command)) break;
				nextCommand = command;
			}

			if (!parseCommand(&nextCommand, &newJob, &inBg) &&
				newJob.numProgs) {
				runCommand(newJob, &jobList, inBg);
			} 
		} else {

			i = 0;
			while (!jobList.fg->progs[i].pid || 
				jobList.fg->progs[i].isStopped) i++;

				waitpid(jobList.fg->progs[i].pid, &status, WUNTRACED);

			if (WIFSIGNALED(status) &&
				(WTERMSIG(status) != SIGINT)) {
				printf("%s\n", strsignal(status));
			}

			if (WIFEXITED(status) || WIFSIGNALED(status)) {

				jobList.fg->runningProgs--;
				jobList.fg->progs[i].pid = 0;

				if (!jobList.fg->runningProgs) {


					removeJob(&jobList, jobList.fg);
					jobList.fg = NULL;


					if (tcsetpgrp(0, getpid()))
						perror("tcsetpgrp");
				}
			} else {

				jobList.fg->stoppedProgs++;
				jobList.fg->progs[i].isStopped = 1;

				if (jobList.fg->stoppedProgs == 
					jobList.fg->runningProgs) {
					printf("\n" JOB_STATUS_FORMAT,
						jobList.fg->jobId,
						"Stopped", jobList.fg->text);
				jobList.fg = NULL;
				}
			}

			if (!jobList.fg) {

				if (tcsetpgrp(0, getpid()))
					perror("tcsetpgrp");
			}
		}
	}

	return 0;
}
