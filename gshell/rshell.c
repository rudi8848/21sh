/*
    real shell (version 4)
    AUP2, Sec. 6.04

    Copyright 2003 by Marc J. Rochkind. All rights reserved.
    May be copied only for purposes and under conditions described
    on the Web page www.basepath.com/aup/copyright.htm.

    The Example Files are provided "as is," without any warranty;
     without even the implied warranty of merchantability or fitness
     for a particular purpose. The author and his publisher are not
     responsible for any damages, direct or incidental, resulting
     from the use or non-use of these Example Files.
 
     The Example Files may contain defects, and some contain deliberate
     coding mistakes that were included for educational reasons.
     You are responsible for determining if and how the Example Files
     are to be used.
 
 */
 #include "defs.h"
 #include "statuspr.h"
 #include "builtin.h"
//------------------------------------------------------------------
//#include "21sh.h"
#include <ctype.h>
#include <strings.h>
# include <termios.h>
# include <curses.h>
# include <term.h>
#include <inttypes.h>

# define K_LEFT     4479771
# define K_RIGHT    4414235
# define K_UP       4283163
# define K_DOWN     4348699

//# define K_ENTER  13  //if raw mode
# define K_ENTER    10
# define K_TAB      9
# define K_SPACE    32
# define K_ESC      27
# define K_DELETE   2117294875L
# define K_BSPACE   127


#define MAXLINE 500

struct termios saved;
struct termios changed;

void    ft_exit(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
    //system("leaks a.out");
    exit(EXIT_SUCCESS);
}

int ft_putchar(int c)
{
    return(write(1, &c, 1));
}

void    ft_prompt(void)
{
    write(STDOUT_FILENO, "# ", 2);
}

static void     ft_int_handler(int signum)
{
    ft_exit();
}

void            ft_set_signals(void)
{
    //signal(SIGTSTP, ft_tstp_handler);
    //signal(SIGCONT, ft_cont_handler);
    signal(SIGINT, ft_int_handler);
    signal(SIGTERM, ft_int_handler);
    signal(SIGABRT, ft_int_handler);
    signal(SIGQUIT, ft_int_handler);
    //signal(SIGWINCH, ft_winch_handler);
}

int     cbreak_settings()
{
    changed = saved;

    changed.c_lflag &= ~(ICANON | ECHO);
    changed.c_cc[VMIN] = 1;
    changed.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &changed) == -1)
    {
        perror(__FUNCTION__);
        ft_exit();
    }

    return 0;
}

void    read_loop(char *line)
{
    
    int         rr;
    uint64_t    rb;
    int j;
    //char c;

    bzero(line, MAXLINE);
    rb = 0;
    int len = 0;
    int i = 0;
   // ft_prompt();
    while ((rr = read(STDIN_FILENO, &rb, 8)) > 0)
    {
        
        if (rb == K_RIGHT)
        {
            if (i < len)
            {
                i++;
                tputs(tgetstr("nd", NULL), 0, ft_putchar);
            }
        }
        else if (rb == K_LEFT )
        {
            if (i > 0)
            {   i--;
                tputs(tgetstr("le", NULL), 0, ft_putchar);
            }
        }
        else if (rb == K_ENTER)
            {
                //i--; index points to next character
                //printf("\n");
                //передавать в лексер сo '\n'???
                line[len] = '\n';
                /*
                if (len)
                {
                    //printf("[GOT:] %s [len = %d, index = %d]\n", line, len, i);
                    //bzero(line, len);
                    i = 0;
                    len = 0;
                }*/
                //ft_prompt();
                return;
            }
        else if (isprint(rb))
        {
            if (len + 1 == MAXLINE)
            {
                tputs(tgetstr("bl", NULL), 0, ft_putchar);          // bell
                printf("\nLine is too long\n");
                return;
            }
            write(STDOUT_FILENO, &rb, rr);
            tputs(tgetstr("im", NULL), 0, ft_putchar);          //insert mode
            if (line[i ])   //if it's at the middle of line
            {
                    j = len + 1;
                    while (j > i)
                    {
                        line[j] = line[j - 1];
                        j--;
                    }
                    //and insert in termcap
                
                tputs(tgetstr("sc", NULL), 0, ft_putchar);      // save cursor position
                tputs(tgetstr("ce", NULL), 0, ft_putchar);      // delete end of line
                write(STDOUT_FILENO, line + i + 1, len);
                tputs(tgetstr("rc", NULL), 0, ft_putchar);      // restore cursor position
                //tputs(tgetstr("nd", NULL), 0, ft_putchar);
            //  tputs(tgetstr("ip", NULL), 0, ft_putchar);
            }
            
            line[i] = (char)rb;

                len++;
            if (i < len)
            {
                i++;
            }
            tputs(tgetstr("ei", NULL), 0, ft_putchar);          //end of insertion mode
        }
        else if (rb == K_DOWN)
        {
            tputs(tgetstr("bl", NULL), 0, ft_putchar);          // bell
        }
        else if (rb == K_UP)
        {
            tputs(tgetstr("bl", NULL), 0, ft_putchar);          // bell
        }
        else if (rb == K_DELETE)
        {
            if (len > 0 && i >= 0 && i < len)
            {
                tputs(tgetstr("dm", NULL), 0, ft_putchar);      //turn on deleting mode
                tputs(tgetstr("dc", NULL), 0, ft_putchar);      //delete 1 char on cursor position
                tputs(tgetstr("ed", NULL), 0, ft_putchar);      // turn off deleting mode
                len--;
                j = i;
                while (line[j])
                {
                    line[j] = line[j + 1];
                    j++;
                }
            }
        }
        else if (rb == K_BSPACE)
        {
            if (i > 0)
            {
                i--;
                len--;
                j = i;
                while (line[j])
                {
                    line[j] = line[j + 1];
                    j++;
                }
                tputs(tgetstr("le", NULL), 0, ft_putchar);  //1 position to left
                tputs(tgetstr("dm", NULL), 0, ft_putchar);  //turn on deleting mode
                tputs(tgetstr("dc", NULL), 0, ft_putchar);  //delete 1 char on cursor position
                tputs(tgetstr("ed", NULL), 0, ft_putchar);  // turn off deleting mode
            }
        }
        
        else if (rb == K_ESC)
            ft_exit();
        //ft_print_forward(g_attr.args);
        rb = 0;
    }
    line[i] = '\n';
    return;
}



//------------------------------------------------------------------
 
 #define WANT_TESTx
 
 /*[macros]*/
 #define MAXARG 50           /* max args in command */
 #define MAXFNAME 500        /* max chars in file name */
 #define MAXWORD 500         /* max chars in arg */
 /*[macros2]*/
 #define PROMPT "* "         /* prompt */
 /*[fd_check]*/
 static void fd_check(void)
 {
     int fd;
     bool ok = true;
 
     for (fd = 3; fd < 20; fd++)
         if (fcntl(fd, F_GETFL) != -1 || errno != EBADF) {
             ok = false;
             fprintf(stderr, "*** fd %d is open ***\n", fd);
         }
     if (!ok)
         _exit(EXIT_FAILURE);
 }
 /*[store_char]*/
 static bool store_char(char *word, size_t maxword, int c, size_t *np)
 {
     errno = E2BIG;
     ec_false( *np < maxword )
     word[(*np)++] = c;
     return true;
 
 EC_CLEANUP_BGN
     return false;
 EC_CLEANUP_END
 }
 /*[gettoken]*/
 typedef enum {T_WORD, T_BAR, T_AMP, T_SEMI, T_GT, T_GTGT, T_LT,
   T_NL, T_EOF, T_ERROR} TOKEN;
 
 static TOKEN gettoken(char *line, int *i, char *word, size_t maxword)
 {
     enum {NEUTRAL, GTGT, INQUOTE, INWORD} state = NEUTRAL;
     int c;
     size_t wordn = 0;
 
     //while ((c = getchar()) != EOF) {
     while ((c = line[(*i)++]) != '\0') {
         switch (state) {
         case NEUTRAL:
             switch (c) {
             case ';':
                 return T_SEMI;
             case '&':
                 return T_AMP;
             case '|':
                 return T_BAR;
             case '<':
                 return T_LT;
             case '\n':
                 return T_NL;
             case ' ':
             case '\t':
                 continue;
             case '>':
                 state = GTGT;
                 continue;
             case '"':
                 state = INQUOTE;
                 continue;
             default:
                 state = INWORD;
                 ec_false( store_char(word, maxword, c, &wordn) )
                 continue;
             }
         case GTGT:
             if (c == '>')
                 return T_GTGT;
            (*i)--;
             //ungetc(c, stdin);
             return T_GT;
         case INQUOTE:
             switch (c) {
             case '\\':
                 if ((c = line[*i]) == EOF)
                     c = '\\';
                 ec_false( store_char(word, maxword, c, &wordn) );
                 continue;
             case '"':
                 ec_false( store_char(word, maxword, '\0', &wordn) )
                 return T_WORD;
             default:
                 ec_false( store_char(word, maxword, c, &wordn) )
                 continue;
             }
         case INWORD:
             switch (c) {
             case ';':
             case '&':
             case '|':
             case '<':
             case '>':
             case '\n':
             case ' ':
             case '\t':
                 //ungetc(c, stdin);
                 (*i)--;
                 ec_false( store_char(word, maxword, '\0', &wordn) )
                 return T_WORD;
             default:
                 ec_false( store_char(word, maxword, c, &wordn) )
                 continue;
             }
         }
     }
     ec_false( !ferror(stdin) )
     return T_EOF;
 
 EC_CLEANUP_BGN
     return T_ERROR;
 EC_CLEANUP_END
 }
 /*[builtin]*/
 static bool builtin(int argc, char *argv[], int srcfd, int dstfd)
 {
     char *path;
 
     if (strchr(argv[0], '=') != NULL)
         asg(argc, argv);
     else if (strcmp(argv[0], "set") == 0)
         set(argc, argv);
     else if (strcmp(argv[0], "cd") == 0) {
         if (argc > 1)
             path = argv[1];
         else if ((path = getenv("HOME")) == NULL)
             path = ".";
         if (chdir(path) == -1)
             fprintf(stderr, "%s: bad directory\n", path);
     }
     else
         return false;
     if (srcfd != STDIN_FILENO || dstfd != STDOUT_FILENO)
         fprintf(stderr, "Illegal redirection or pipeline\n");
     return true;
 }
 /*[ignore_sig]*/
 static struct sigaction entry_int, entry_quit;
 
 static bool ignore_sig(void)
 {
     static bool first = true;
     struct sigaction act_ignore;
 
     memset(&act_ignore, 0, sizeof(act_ignore));
     act_ignore.sa_handler = SIG_IGN; /* may generate warning on Solaris */
     if (first) {
         first = false;
         ec_neg1( sigaction(SIGINT, &act_ignore, &entry_int) )
         ec_neg1( sigaction(SIGQUIT, &act_ignore, &entry_quit) )
     }
     else {
         ec_neg1( sigaction(SIGINT, &act_ignore, NULL) )
         ec_neg1( sigaction(SIGQUIT, &act_ignore, NULL) )
     }
     return true;
 
 EC_CLEANUP_BGN
     return false;
 EC_CLEANUP_END
 }
 
 static bool entry_sig(void)
 {
     ec_neg1( sigaction(SIGINT, &entry_int, NULL) )
     ec_neg1( sigaction(SIGQUIT, &entry_quit, NULL) )
     return true;
 
 EC_CLEANUP_BGN
     return false;
 EC_CLEANUP_END
 }
 /*[wait_and_display]*/
 static bool wait_and_display(pid_t pid)
 {
     pid_t wpid;
     int status;
 
     do {
         ec_neg1( wpid = waitpid(-1, &status, 0) )
         display_status(wpid, status);
     } while (wpid != pid);
     return true;
 
 EC_CLEANUP_BGN
     return false;
 EC_CLEANUP_END
 }
 /*[redirect]*/
 static void redirect(int srcfd, const char *srcfile, int dstfd,
   const char *dstfile, bool append, bool bckgrnd)
 {
     int flags;
 
     if (srcfd == STDIN_FILENO && bckgrnd) {
         srcfile = "/dev/null";
         srcfd = -1;
     }
     if (srcfile[0] != '\0')
         ec_neg1( srcfd = open(srcfile, O_RDONLY, 0) )
     ec_neg1( dup2(srcfd, STDIN_FILENO) )
     if (srcfd != STDIN_FILENO)
         ec_neg1( close(srcfd) )
     if (dstfile[0] != '\0') {
         flags = O_WRONLY | O_CREAT;
         if (append)
             flags |= O_APPEND;
         else
             flags |= O_TRUNC;
         ec_neg1( dstfd = open(dstfile, flags, PERM_FILE) )
     }
     ec_neg1( dup2(dstfd, STDOUT_FILENO) )
     if (dstfd != STDOUT_FILENO)
         ec_neg1( close(dstfd) )
     fd_check();
     return;
 
 EC_CLEANUP_BGN
     _exit(EXIT_FAILURE); /* we are in child */
 EC_CLEANUP_END
 }
 /*[invoke]*/
 static pid_t invoke(int argc, char *argv[], int srcfd, const char *srcfile,
   int dstfd, const char *dstfile, bool append, bool bckgrnd, int closefd)
 {
     pid_t pid;
     char *cmdname, *cmdpath;
 
     if (argc == 0 || builtin(argc, argv, srcfd, dstfd))
         return 0;
     switch (pid = fork()) {
     case -1:
         fprintf(stderr, "Can't create new process\n");
         return 0;
     case 0:
         if (closefd != -1)
             ec_neg1( close(closefd) )
         if (!bckgrnd)
             ec_false( entry_sig() )
         redirect(srcfd, srcfile, dstfd, dstfile, append, bckgrnd);
         cmdname = strchr(argv[0], '/');
         if (cmdname == NULL)
             cmdname = argv[0];
         else
             cmdname++;
         cmdpath = argv[0];
         argv[0] = cmdname;
         execvp(cmdpath, argv);
         fprintf(stderr, "Can't execute %s\n", cmdpath);
         _exit(EXIT_FAILURE);
     }
     /* parent */
     if (srcfd > STDOUT_FILENO)
         ec_neg1( close(srcfd) )
     if (dstfd > STDOUT_FILENO)
         ec_neg1( close(dstfd) )
     if (bckgrnd)
         printf("%ld\n", (long)pid);
     return pid;
 
 EC_CLEANUP_BGN
     if (pid == 0)
         _exit(EXIT_FAILURE);
     return -1;
 EC_CLEANUP_END
 }
 /*[command]*/
 static TOKEN command(char *line, int *i, pid_t *wpid, bool makepipe, int *pipefdp)
 {
     TOKEN token, term;
     int argc, srcfd, dstfd, pid, pfd[2] = {-1, -1};
     char *argv[MAXARG], srcfile[MAXFNAME] = "", dstfile[MAXFNAME] = "";
     char word[MAXWORD];
     bool append;
 
     argc = 0;
     srcfd = STDIN_FILENO;
     dstfd = STDOUT_FILENO;
     while (true) {
         switch (token = gettoken(line, i, word, sizeof(word))) {
         case T_WORD:
             if (argc >= MAXARG - 1) {
                 fprintf(stderr, "Too many args\n");
                 continue;
             }
             if ((argv[argc] = malloc(strlen(word) + 1)) == NULL) {
                 fprintf(stderr, "Out of arg memory\n");
                 continue;
             }
             strcpy(argv[argc], word);
             argc++;
             continue;
         case T_LT:
             if (makepipe) {
                 fprintf(stderr, "Extra <\n");
                 break;
             }
             if (gettoken(line, i, srcfile, sizeof(srcfile)) != T_WORD) {
                 fprintf(stderr, "Illegal <\n");
                 break;
             }
             srcfd = -1;
             continue;
         case T_GT:
         case T_GTGT:
             if (dstfd != STDOUT_FILENO) {
                 fprintf(stderr, "Extra > or >>\n");
                 break;
             }
             if (gettoken(line, i, dstfile, sizeof(dstfile)) != T_WORD) {
                 fprintf(stderr, "Illegal > or >>\n");
                 break;
             }
             dstfd = -1;
             append = token == T_GTGT;
             continue;
         case T_BAR:
         case T_AMP:
         case T_SEMI:
         case T_NL:
             argv[argc] = NULL;
             if (token == T_BAR) {
                 if (dstfd != STDOUT_FILENO) {
                     fprintf(stderr, "> or >> conflicts with |\n");
                     break;
                 }
                 term = command(line, i, wpid, true, &dstfd);
                 if (term == T_ERROR)
                     return T_ERROR;
             }
             else
                 term = token;
             if (makepipe) {
                 ec_neg1( pipe(pfd) )
                 *pipefdp = pfd[1];
                 srcfd = pfd[0];
             }
             ec_neg1( pid = invoke(argc, argv, srcfd, srcfile, dstfd,
               dstfile, append, term == T_AMP, pfd[1]) )
             if (token != T_BAR)
                 *wpid = pid;
             if (argc == 0 && (token != T_NL || srcfd > 1))
                 fprintf(stderr, "Missing command\n");
             while (--argc >= 0)
                 free(argv[argc]);
             return term;
         case T_EOF:
             exit(EXIT_SUCCESS);
         case T_ERROR:
             return T_ERROR;
         }
     }
 
 EC_CLEANUP_BGN
     return T_ERROR;
 EC_CLEANUP_END
 }
 /*[]*/
 #ifndef WANT_TEST
 /*[main]*/
 int main(void)
 {
     pid_t pid;
     TOKEN term = T_NL;
     int ret = 0;
     int i = 0;
     char line[MAXLINE];
  
 //-----------------------------------------------------------
    ret = tcgetattr(STDOUT_FILENO, &saved);
    if (ret == -1)
    {
        perror(__FUNCTION__);
        return 1;
    }

    char *name = NULL;
    if (!(name = getenv("TERM")))
    {
        perror("getenv");
        exit(0);
    }
    if (tgetent(NULL, name) < 1)
    {
        perror("tgetent");
        exit(0);
    }
    cbreak_settings();
 
    //-----------------------------------------------------------

     //ignore_sig();
     ft_set_signals();
     while (true) {
        
         if (term == T_NL)
            ft_prompt();
             //printf("%s", PROMPT);
         read_loop(&line[0]);
 
         write(1, "\n", 1);
         term = command(line, &i, &pid, false, NULL);
         if (term == T_ERROR) {
             fprintf(stderr, "Bad command\n");
             EC_FLUSH("main--bad command")
             term = T_NL;
         }
         if (term != T_AMP && pid > 0)
             wait_and_display(pid);
         fd_check();
         i = 0;
     }
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
 }
 /*[]*/
 #else
 /*[gettoken-test]*/
/* 
int main(void)
 {
     char word[200];
 
     while (1)
         switch (gettoken(word, sizeof(word))) {
         case T_WORD:
             printf("T_WORD <%s>\n", word);
             break;
         case T_BAR:
             printf("T_BAR\n");
             break;
         case T_AMP:
             printf("T_AMP\n");
             break;
         case T_SEMI:
             printf("T_SEMI\n");
             break;
         case T_GT:
             printf("T_GT\n");
             break;
         case T_GTGT:
             printf("T_GTGT\n");
             break;
         case T_LT:
             printf("T_LT\n");
             break;
         case T_NL:
             printf("T_NL\n");
             break;
         case T_EOF:
             printf("T_EOF\n");
             exit(EXIT_SUCCESS);
         case T_ERROR:
             printf("T_ERROR\n");
             exit(EXIT_SUCCESS);
         }
 }
 */
 /*[]*/
 #endif
