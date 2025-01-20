#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define KU_CPU_PwP 340

int main(int argc, char **argv)
{
    int jobTime;
    int delayTime;
    int responseTime = -1;
    char name[4];
    int wait = 0;
    int priority;

    if (argc < 5)
    {
        printf("\nInsufficient Arguments..\n");
        return 1;
    }

    /*  first argument: job time (second)
        second argument: delay time before execution (second)
        third argument: process name
        fourth argument: priority
    */

    jobTime = atoi(argv[1]);
    delayTime = atoi(argv[2]);
    strcpy(name, argv[3]);
    priority = atoi(argv[4]);

    sleep(delayTime);
    printf("\nProcess %s: I will use CPU by %ds.\n", name, jobTime);
    jobTime *= 10; // execute system call in every 0.1 sec

    // continue requesting the system call as long as the jobTime remains
    while (jobTime)
    {
        // if request is rejected, increase wait time and response time
        if (!syscall(KU_CPU_PwP, name, jobTime, priority))
        {
            if (responseTime == -1)
                responseTime = wait;
            jobTime--;
        }
        else
            wait++;
        usleep(100000); // delay 0.1 sec
    }

    syscall(KU_CPU_PwP, name, 0, priority);
    printf(
        "\nProcess %s : Finish!\nMy response time is %ds.\nMy total wait time "
        "is %ds.\n",
        name, (responseTime + 5) / 10, (wait + 5) / 10);
    return 0;
}
