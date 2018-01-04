#include "network.h"
#include "strategies.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define N_HOSTS 1

int main(int argc, char **argv) {
    char host_name[] = "00.dptinfo.ens-cachan.fr";
    char command[500];

    int fd[N_HOSTS+1];
    FILE *streams[N_HOSTS+1];

    for(int host=1; host<=N_HOSTS; host++) {
        sprintf(host_name, "%.2d.dptinfo.ens-cachan.fr", host);
        sprintf(command, "%s -p 100", argv[1]);
        if(!fork()) {
            execlp("ssh", "ssh", host_name, command, NULL);
            perror("monitor");
            exit(EXIT_FAILURE);
        }

        sprintf(command, "%.2d.dptinfo.ens-cachan.fr", host);
        int ret = -1;
        while(ret < 0) {
            sleep(1);
            ret = connect_to_server(command, 4000);
        }
        fd[host] = ret;
        streams[host] = fdopen(fd[host], "r");
        setlinebuf(streams[host]);
        printf("connected\n");
    }

    char buffer[100];
    while(fgets(buffer, 100, streams[1])) {
        printf("%s", buffer);
    }
}
