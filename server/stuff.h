
#define PORT "4060"
#define BUFF_SIZE 512
#define BACKLOG 10

void error(const char *msg);
void bindfirst(int *sockfd, struct addrinfo *servinfo, struct addrinfo **p, int yes);
void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
void takeAction(const char *input, char *output, size_t size);