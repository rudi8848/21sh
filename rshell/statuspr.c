statuspr.c
Go to the documentation of this file.
00001 /*
00002     Display exit status
00003     AUP2, Sec. 5.08
00004 
00005     Copyright 2003 by Marc J. Rochkind. All rights reserved.
00006     May be copied only for purposes and under conditions described
00007     on the Web page www.basepath.com/aup/copyright.htm.
00008 
00009     The Example Files are provided "as is," without any warranty;
00010     without even the implied warranty of merchantability or fitness
00011     for a particular purpose. The author and his publisher are not
00012     responsible for any damages, direct or incidental, resulting
00013     from the use or non-use of these Example Files.
00014 
00015     The Example Files may contain defects, and some contain deliberate
00016     coding mistakes that were included for educational reasons.
00017     You are responsible for determining if and how the Example Files
00018     are to be used.
00019 
00020 */
00021 #include "defs.h"
00022 #include "statuspr.h"
00023 
00024 /*[display_status]*/
00025 void display_status(pid_t pid, int status)
00026 {
00027     if (pid != 0)
00028         printf("Process %ld: ", (long)pid);
00029     if (WIFEXITED(status))
00030         printf("Exit value %d\n", WEXITSTATUS(status));
00031     else {
00032         char *desc;
00033         char *signame = get_macrostr("signal", WTERMSIG(status), &desc);
00034         if (desc[0] == '?')
00035             desc = signame;
00036         if (signame[0] == '?')
00037             printf("Signal #%d", WTERMSIG(status));
00038         else
00039             printf("%s", desc);
00040         if (WCOREDUMP(status))
00041             printf(" - core dumped");
00042         if (WIFSTOPPED(status))
00043             printf(" (stopped)");
00044 #if defined(_XOPEN_UNIX) && !defined(LINUX)
00045         else if (WIFCONTINUED(status))
00046             printf(" (continued)");
00047 #endif
00048         printf("\n");
00049     }
00050 }
00051 /*[]*/