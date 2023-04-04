#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <bits/types/sigset_t.h>
#include <sys/wait.h>
int conc_amnt;
void pa_handle_sigint()
{
    pid_t pid = getpid();
    printf("[pid](%d): SIGINT is handled in pa_handle_sigint()\n", pid);
    printf("[pid](%d): [func](exit)  with [conc_amnt](%d)\n", pid, conc_amnt);
    exit(0);
}
void pa_handle_sigchld()
{
    pid_t cpid;
    pid_t pid = getpid();
    printf("[pid](%d): SIGCHLD is handled in pa_handle_sigchld()\n", pid);

    while ((cpid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        printf("[pid](%d): [cpid](%d) is waited in pa_handle_sigchld()\n", pid, cpid);
        conc_amnt--;
    }
}

int main(int argc, char **argv)
{
    conc_amnt = atoi(argv[2]);
    pid_t pids[conc_amnt - 1];
    struct sigaction act1, act2;
    memset(&act1, 0, sizeof(act1));
    memset(&act2, 0, sizeof(act2));
    sigemptyset(&act1.sa_mask);
    sigemptyset(&act2.sa_mask);
    act1.sa_handler = pa_handle_sigint;
    act2.sa_handler = pa_handle_sigchld;
    sigaction(SIGINT, &act1, NULL);
    sigaction(SIGCHLD, &act2, NULL);
    printf("[pid](%d): start\n", getpid());
    for (int i = 0; i < conc_amnt - 1; i++)
    {
        pid_t pid = fork();
        if (!pid) // child processes
        {
            pid_t cpid = getpid();
            printf("[pid](%d): start\n", cpid);
            pause();
        }
        else
            pids[i] = pid;
    }
    sleep(1);
    for (int i = 0; i < conc_amnt - 1; i++)
        kill(pids[i], SIGINT);
    while (conc_amnt > 1)
    {
        printf("[pid](%d): [func](pause) with [conc_amnt](%d)\n", getpid(), conc_amnt);
        pause();
    }
    pause();
}
