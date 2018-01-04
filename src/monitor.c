#include "network.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    char command[500];
    int fd[24];
    for(int host=1; host<=1; host++) {
        sprintf(command, "ssh %.2d.dptinfo.ens-cachan.fr '%s -p 100'", host, argv[1]);
        system(command);
        sprintf(command, "%.2d.dptinfo.ens-cachan.fr", host);
        fd[host] = connect_to_server(command, 4000);
    }
}
