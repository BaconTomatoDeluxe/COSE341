#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/syscalls.h>

#define MAXSIZE 200

typedef struct
{
    pid_t pid;
    int jobTime;
} job_t;

job_t queue1[MAXSIZE];
int front = 0;
int rear = 0;
int i;
job_t now = {0, 0};

int new_pid(pid_t, pid)
{
    for (i = front; i < rear; i++)
    {
        if (pid == queue1[i].pid)
            return 0;
    }
    return 1;
}

SYSCALL_DEFINE1(os2024_pid_print, char *, name)
{
    pid_t pid = current->pid;
    printk("Process name: %s pid: %d\n", name, pid);
    return 0;
}

SYSCALL_DEFINE2(os2024_ku_cpu_FCFS, char *, name, int, jobTime)
{
    // store pid of current process as pid_t type
    job_t newJob = {current->pid, jobTime};

    // register the process if virtual CPU is idle
    if (now.pid == 0)
        now.pid = newJob.pid;

    // If the process that sent the requset is currently using virtual CPU
    if (now.pid == newJob.pid)
    {
        // If the job has finished
        if (jobTime == 0)
        {
            printk("Process Finished: %s\n", name);
            // If queue is empty, virtual CPU becomes idel
            if (front == rear)
                now.pid = 0;
            // If not, get next process from queue
            else
                now.pid = queue1[front++].pid;
        }
        else
            printk("Working: %s\n", name);
        // Request accepted
        return 0;
    }
    else
    {
        // If the request is not from currently handling process
        if (new_pid(newJob.pid))
        {
            if (rear < MAXSIZE - 1)
                queue1[rear++] = newJob;
            else
            {
                printk(KERN_INFO
                       "[Error] - Waiting queue out of range --------\n");
                return -1;
            }
        }
        printk("Working Denied:%s\n", name);
    }
    // Request rejected
    return 1;
}

SYSCALL_DEFINE2(os2024_ku_cpu_SRTF, char *, name, int, jobTime)
{
    // store pid of current process as pid_t type
    job_t newJob = {current->pid, jobTime};

    // register the process if virtual CPU is idle
    if (now.pid == 0)
        now = newJob;

    // If the process that sent the requset is currently using virtual CPU
    if (now.pid == newJob.pid)
    {
        // If the job has finished
        if (jobTime == 0)
        {
            printk("Process Finished: %s\n", name);
            // If queue is empty, virtual CPU becomes idel
            if (front == rear)
                now.pid = 0;
            // If not, get next process from queue
            else
                now.pid = queue1[front++].pid;
        }
        else
        {
            // Update current program jobtime
            now = newJob;
            printk("Working: %s\n", name);
        }
        // Request accepted
        return 0;
    }
    else
    {
        // If the request is not from currently handling process
        if (new_pid(newJob.pid))
        {
            if (rear < MAXSIZE - 1)
            {
                // Sort waiting queue by shorter jobtime
                if (now.jobTime <= newJob.jobTime)
                {
                    for (i = rear - 1; i >= front; i--)
                    {
                        if (newJob.jobTime < queue1[i].jobTime)
                        {
                            queue1[i + 1] = queue1[i];
                        }
                        else
                            break;
                    }
                    queue1[i + 1] = newJob;
                    rear++;
                }
                // Interrupt by shorter brust time program.
                else
                {
                    for (i = rear - 1; i >= front; i--)
                        queue1[i + 1] = queue1[i];
                    rear++;
                    queue1[front] = now;
                    now = newJob;
                    printk("Switch progarm to -------> %s\n", name);
                }
            }
            else
            {
                printk(KERN_INFO
                       "[Error] - Waiting queue out of range --------\n");
                return -1;
            }
        }
        printk("Working Denied:%s\n", name);
    }
    // Request rejected
    return 1;
}

// Round Robin with 1s time slice
SYSCALL_DEFINE2(os2024_ku_cpu_RR, char *, name, int, jobTime)
{
    // store pid of current process as pid_t type
    job_t newJob = {current->pid, jobTime};

    // register the process if virtual CPU is idle
    if (now.pid == 0)
        now = newJob;

    // If the process that sent the requset is currently using virtual CPU
    if (now.pid == newJob.pid)
    {
        // If the job has finished
        if (jobTime == 0)
        {
            printk("Process Finished: %s\n", name);
            // If queue is empty, virtual CPU becomes idel
            if (front == rear)
                now.pid = 0;
            // If not, get next process from queue
            else
                now = queue1[front++];
        } // Scheduling by time slice
        else if (now.jobTime - newJob.jobTime == 10)
        {
            if (rear < MAXSIZE - 1)
            {
                printk("Turn Over -------> %s\n", name);
                queue1[rear++] = newJob;
                now = queue1[front++];
                return 1;
            }
            else
            {
                printk(KERN_INFO
                       "[Error] - Waiting queue out of range --------\n");
                return -1;
            }
        }
        else
            printk("Working: %s\n", name);
        // Request accepted
        return 0;
    }
    else
    {
        // If the request is not from currently handling process
        if (new_pid(newJob.pid))
        {
            if (rear < MAXSIZE - 1)
                queue1[rear++] = newJob;
            else
            {
                printk(KERN_INFO
                       "[Error] - Waiting queue out of range --------\n");
                return -1;
            }
        }
    }
    // Request rejected
    return 1;
}
