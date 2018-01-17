#include "network.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>


int connect_to_server(char *server, short port) {
    // creates socket
    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock < 0) {
        perror("socket");
        return -1;
    }

    // get server address
	struct hostent *address = gethostbyname(server);
	if (!address) {
        herror("gethostbyname");
        close(sock);
        return -1;
    }

    // creates socket address
	struct sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	memcpy(&sock_addr.sin_addr, address->h_addr_list[0], address->h_length);
	sock_addr.sin_port = htons(port);

	// connect
	if(connect(sock,(struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }
	return sock;
}

int open_listen_socket(short port) {
    // creates socket
    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock < 0) {
        perror("socket");
        return -1;
    }

    // creates socket address
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port = htons(port);

    // bind socket and set in listen mode
    if(bind(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
        perror("bind");
        close(sock);
		return -1;
	}
    if(listen(sock,SOMAXCONN) < 0) {
        perror("listen");
        close(sock);
        return -1;
    }
    return sock;
}

int wait_for_client(int socket) {
    for(;;) {
        int client = accept(socket, NULL, NULL);
        if(client < 0) {
            if (errno == EINTR || errno == EWOULDBLOCK) continue;
            perror("accept");
            return -1;
        }
        return client;
    }
}
