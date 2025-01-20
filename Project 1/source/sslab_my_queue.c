#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#define MAXSIZE 200
int queue[MAXSIZE] = {0, };
int front = 0;
int rear = 0;
int i, r;

SYSCALL_DEFINE1(os2024_enqueue, int, a) {
// Check if the queue is full
    if (rear >= MAXSIZE - 1) {
        printk(KERN_INFO "[Error] - Queue is full--------\n");
        return -1;
    }

// Check if the value is already exist in the queue 
    for (i = 0; i <= rear; i++) {
        if (queue[i] == a) {
            printk(KERN_INFO "[Error] - Already exist in the queue\n");
            return a;
        }
    }

    queue[rear] = a; // enqueue

    printk(KERN_INFO "[System call] os2024_enqueue(); -----\n");
    printk("Queue Front ----------------------\n");
    for (i = front; i <= rear; i++) {
        printk("%d\n", queue[i]);
    }
    printk("Queue Rear  ----------------------\n");

    rear++; // increase rear index
    return a;
}

SYSCALL_DEFINE0(os2024_dequeue) {
// Check if the queue is empty
    if (front >= rear) {
        printk(KERN_INFO "[Error] - Queue is empty--------\n");
        return -1;
    }

    r = queue[front]; // return value of dequeue

    printk(KERN_INFO "[System call] os2024_dequeue(); -----\n");
    printk("Queue Front ----------------------\n");
    for (i = front + 1; i < rear; i++) {
        printk("%d\n", queue[i]);
        queue[i-1] = queue[i]; // move every value forward
    }
    printk("Queue Rear  ----------------------\n");
    
    rear--; // decrease rear index
    queue[rear] = 0; // init rear
    return r;
}

