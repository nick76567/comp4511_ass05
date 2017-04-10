/** 
 * @file myfifo_test.c
 * @brief Create two SCHED_MYFIFO jobs
 * @version 1.0
 * @date 2017-03-01
 */
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define LOOP_ITERS_PER_MILLISEC 200000
#define SCHED_MYFIFO 6

void burn_1millisecs()
{
    unsigned long long i;
    for (i = 0; i < LOOP_ITERS_PER_MILLISEC; ++i)
    {
    }
}

void burn_cpu(int msecs)
{
    int i;
    for (i = 0; i < msecs; ++i)
    {
        burn_1millisecs();
    }
}

int main(int argc, const char *argv[])
{
    int ret;
    int status;
    pid_t pid;
    int msecs;
    int i;
    struct sched_param sp = {.sched_priority = 0};
    struct sched_param psp = {.sched_priority = 1};
    if (argc != 2)
    {
        printf("usage: -[milli_seconds]\n");
        return -1;
    }
    msecs = atoi(argv[1]);
    printf("Parent-%d started\n", getpid());
    fflush(stdout);
    //ret = sched_setscheduler(0, SCHED_FIFO, &psp);
    ret = sched_setscheduler(0, SCHED_MYFIFO, &sp);
    if (ret == -1)
    {
        perror("sched_setscheduler");
        exit(-1);
    }


    for (i = 0; i < 2; ++i)
    {
        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(-1);
        }
        if (!pid)
        {
	    //ret = sched_setscheduler(0, SCHED_FIFO, &psp);
            ret = sched_setscheduler(getpid(), SCHED_MYFIFO, &sp);
            if (ret == -1)
            {
                perror("sched_setscheduler");
                exit(-1);
            }
            time_t end;
            burn_cpu(msecs);
            time(&end);
            printf("Child-%d finished: %s", getpid(), ctime(&end));
            fflush(stdout);
            exit(0);
        }
        else
        {
            printf("Child-%d created\n", pid);
            fflush(stdout);
        }
    }
    while ((pid = wait(&status)) > 0)
    {
    }
    printf("Parent-%d exited\n", getpid());
    return 0;
}
