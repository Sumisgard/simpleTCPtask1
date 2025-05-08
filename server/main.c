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

int main(void)
{
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char msg[BUFF_SIZE];

    memset(msg, 0, BUFF_SIZE);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    rv = getaddrinfo(NULL, PORT, &hints, &servinfo);
    if (rv != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    bindfirst(&sockfd, servinfo, &p, yes);

    if (p == NULL) 
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    freeaddrinfo(servinfo);

    if (listen(sockfd, BACKLOG) == -1)
        error("listen");
    
    printf("server: waiting for connections...\n");

    while (1)
    {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if (new_fd == -1)
            error("accept");
        
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s));
        printf("server: got connection from %s\n", s);

        if (recv(new_fd, msg, BUFF_SIZE, 0) == -1)
            perror("recv");

        takeAction(msg, BUFF_SIZE);

        if (send(new_fd, msg, BUFF_SIZE, 0) == -1)
            perror("send");

        close(new_fd);
    }
    
    return 0;
}