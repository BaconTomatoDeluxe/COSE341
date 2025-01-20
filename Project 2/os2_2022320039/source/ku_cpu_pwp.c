#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/syscalls.h>

#define MAXSIZE 200

typedef struct {
    pid_t pid;
    int jobTime;
    int priority
} job_t;

job_t queue[MAXSIZE];
int front = 0;
int rear = 0;
int i;
job_t now = {0, 0, 0};

int new_pid(pid_t, pid) {
    for (i = front; i < rear; i++) {
        if (pid == queue[i].pid) return 0;
    }
    return 1;
}

SYSCALL_DEFINE3(os2024_ku_cpu_PwP, char *, name, int, jobTime, int, priority) {
    // store pid of current process as pid_t type
    job_t newJob = {current->pid, jobTime, priority};

    // register the process if virtual CPU is idle
    if (now.pid == 0) now = newJob;

    // If the process that sent the requset is currently using virtual CPU
    if (now.pid == newJob.pid) {
        // If the job has finished
        if (jobTime == 0) {
            printk("Process Finished: %s\n", name);
            // If queue is empty, virtual CPU becomes idel
            if (front == rear) now.pid = 0;
            // If not, get next process from queue
            else
                now = queue[front++];
        } else
            printk("Working: %s\n", name);
        // Request accepted
        return 0;
    } else {
        // If the request is not from currently handling process
        if (new_pid(newJob.pid)) {
            if (rear < MAXSIZE - 1) {
                if (newJob.priority >= now.priority) {
                    for (i = rear - 1; i >= front; i--) {
                        if (newJob.priority < queue[i].priority) {
                            queue[i + 1] = queue[i];
                        } else
                            break;
                    }
                    queue[i + 1] = newJob;
                    rear++;
                }
                // interrupted by higher priority program
                else {
                    for (i = rear - 1; i >= front; i--) queue[i + 1] = queue[i];
                    rear++;
                    queue[front] = now;
                    now = newJob;
                    printk("Turn Over -------> %s\n", name);
                }
            } else {
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