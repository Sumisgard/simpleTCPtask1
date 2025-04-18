#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "stuff.h"

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[BUFF_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        fprintf(stderr, "usage: ./client.out ip\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    connectfirst(&sockfd, servinfo, &p);

    if (p == NULL)
        error("client: failed to connect");

    freeaddrinfo(servinfo);

    if ((numbytes = recv(sockfd, buf, BUFF_SIZE - 1, 0)) == -1)
        error("recv");
    
    printf("Recieved message: %s\n", buf);

    return 0;
}