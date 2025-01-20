#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main() {
    int r ,e;
    int i;

    for (i = 0; i < 3; i++) {
        e = rand() % 100 + 1; // randomly generated integer
        r = syscall(335, e);
        printf("Enqueued: %d\n", r);
    }

    for (i = 0; i < 3; i++) {
        r = syscall(336);
        printf("Dequeued: %d\n", r);
    }
}
