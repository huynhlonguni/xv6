#include "kernel/types.h"
#include "user/user.h"







int main() {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        char buf;
        read(p[0], &buf, 1);
        printf("%d: received ping\n", getpid());
        close(p[0]);
        write(p[1], &buf, 1);
        close(p[1]);
    }
    else {
        char buf = 'a';
        write(p[1], &buf, 1);
        close(p[1]);
        wait((int*)0);
        read(p[0], &buf, 1);
        printf("%d: received pong\n", getpid());
        close(p[0]);
    }
    exit(0);
}