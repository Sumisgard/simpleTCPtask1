
#define PORT "4060"
#define BUFF_SIZE 512
#define BACKLOG 10

void error(const char *msg);
void bindfirst(int *sockfd, struct addrinfo *servinfo, struct addrinfo **p, int yes);
void *get_in_addr(struct sockaddr *sa);
void takeAction(char *msg, size_t size);