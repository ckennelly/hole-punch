/**
 * hole-punch - A simple UDP-based NAT hole punching example for C
 * (c) Chris Kennelly <chris@ckennelly.com>
 *
 * Use, modification, and distribution are subject to the Boost Software
 * License, Version 1.0.  (See accompanying file COPYING or a copy at
 * <http://www.boost.org/LICENSE_1_0.txt>.)
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

/**
 * The server listens on the specified port for UDP packets.  It then
 * alternates between two states as it receives messages from clients.
 *
 * In the first state, the server replies to the connecting client with address
 * and port 0.  A client receiving this message waits to receive UDP packets
 * another client.
 *
 * In the second state, the server replies to the connecting client with the
 * address and port of the client in the first state.  This client can then
 * connect to the listening client.
 */
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", basename(argv [0]));
        return 1;
    }

    int port;
    int ret = sscanf(argv [1], "%d", &port);
    uint16_t real_port;
    if (ret != 1) {
        fprintf(stderr, "Unable to parse port number.\n");
        return 4;
    } else if (port <= 0) {
        fprintf(stderr, "Port (%d) must be positive.\n", port);
        return 5;
    } else if (port >= 65536) {
        fprintf(stderr, "Port (%d) must be less than 65536.\n", port);
        return 6;
    } else {
        real_port = (uint16_t) port;
    }

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        fprintf(stderr, "Unable to create socket.  errno %d\n", errno);
        return 2;
    }

    { /* Bind to address */
        struct sockaddr_in my_addr;
        memset(&my_addr, 0, sizeof(my_addr));
        my_addr.sin_family      = AF_INET;
        my_addr.sin_port        = htons(real_port);
        my_addr.sin_addr.s_addr = INADDR_ANY;

        int bindret = bind(fd, (const struct sockaddr *)&my_addr,
            sizeof(my_addr));
        if (bindret < 0) {
            fprintf(stderr, "Unable to bind to port %d.  errno %d\n", port,
                errno);
            return 7;
        }
    }

    /* Type-pun for a packed struct */
    uint32_t * last_addr;
    uint16_t * last_port;
    char buf[sizeof(*last_addr) + sizeof(*last_port)];
    memset(buf, 0, sizeof(buf));

    last_addr   = (uint32_t *) (buf          );
    last_port   = (uint16_t *) (last_addr + 1);

    for (uint32_t iter = 0; ; iter++) {
        struct sockaddr_in  next_data;
        socklen_t           next_len    = sizeof(next_data);
        /* Keep Valgrind happy */
        memset(&next_data, 0, sizeof(next_data));

        // Wait for contact
        ssize_t recvret = recvfrom(fd, NULL, 0, MSG_TRUNC,
            (struct sockaddr *) &next_data, &next_len);
        if (recvret < 0) {
            fprintf(stderr, "Error on recvfrom.  errno %d\n", errno);
            return 3;
        }

        // Reply with what we know.
        ssize_t sendret = sendto(fd, buf, sizeof(buf), 0, &next_data,
            sizeof(next_data));
        if (sendret < 0) {
            fprintf(stderr, "Error on sendto.  errno %d\n", errno);
            return 6;
        }

        // Swap
        if (iter %= 2) {
            memset(buf, 0, sizeof(buf));
        } else {
            *last_addr = next_data.sin_addr.s_addr;
            *last_port = next_data.sin_port;
        }
    }

    return 0;
}
