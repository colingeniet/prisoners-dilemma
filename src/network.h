#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <sys/socket.h>
#include <stddef.h>

/** Connect to a server.
 * Opens a TCP stream connection.
 * @param server the server hostname to connect to.
 * @param port the port on which to connect to the server.
 * @return a file descriptor to communicate to the server,
 *   or -1 if an error occured. */
int connect_to_server(char *server, short port);

/** Creates a socket in listen mode.
 * @param port the port on which to bind the socket.
 * @return the socket, or -1 if an error occured. */
int open_listen_socket(short port);

/** Wait for a client to connect on a socket.
 * @param socket the socket.
 * @return a file descriptor to communicate with the client,
 *   or -1 if an error occured. */
int wait_for_client(int socket);

#endif /* end of include guard: NETWORK_H_INCLUDED */
