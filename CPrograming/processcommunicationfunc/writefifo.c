/* writefifo.c */
/* named pipe example */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MSGSIZE 64
char *testfifo = "fifo";

int main(int argc, char **argv)
{
    int fd, i, nwrite;
    char buf[MSGSIZE];

    if (argc < 2) {
        fprintf(stderr, "Usage: ex11_8_s msg ...\n");
        exit(1);
    }

    /* open testfifo, setting O_WRONLY */
    if ((fd = open(testfifo, O_WRONLY)) < 0) {
        perror("fifo open failed");
        exit(1);
    }

    /* send message */
    for (i = 1; i < argc; i++) {
        strcpy(buf, argv[i]);
        if ((nwrite = write(fd, buf, MSGSIZE)) < 0) {
            perror("fifo write failed");
            exit(1);
        }
    }
}

