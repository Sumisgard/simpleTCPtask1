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
#include <ctype.h>

#include "stuff.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void bindfirst(int *sockfd, struct addrinfo *servinfo, struct addrinfo **p, int yes)
{
    for(*p = servinfo; *p != NULL; *p = (*p)->ai_next) {
        if ((*sockfd = socket((*p)->ai_family, (*p)->ai_socktype, (*p)->ai_protocol)) == -1)
            error("server: socket");

        if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
            error("setsockopt");

        if (bind(*sockfd, (*p)->ai_addr, (*p)->ai_addrlen) == -1) {
            close(*sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void takeAction(const char *input, char *output, size_t size) {
    if (input == NULL || output == NULL || size == 0) {
        if (size > 0) output[0] = '\0';
        return;
    }

    int max_len = 0, current_len = 0;
    int start = -1, start_max = -1, end_max = -1;

    // Находим самое длинное слово
    for (int i = 0;; ++i) {
        char c = input[i];
        if (c != '\0' && !isspace((unsigned char)c)) {
            if (current_len == 0) start = i;
            current_len++;
        } else {
            if (current_len > max_len) {
                max_len = current_len;
                start_max = start;
                end_max = i - 1;
            }
            current_len = 0;
            start = -1;
        }
        if (c == '\0') break;
    }

    // Обработка вывода с удалением лишних пробелов
    int last_was_space = 1;
    size_t out_idx = 0;

    for (size_t i = 0; input[i] != '\0' && out_idx < size - 1; ++i) {
        // Пропускаем символы самого длинного слова
        if ((size_t)i >= (size_t)start_max && (size_t)i <= (size_t)end_max) {
            continue;
        }

        char c = input[i];
        if (isspace((unsigned char)c)) {
            // Пропускаем пробелы в начале и повторяющиеся пробелы
            if (!last_was_space && out_idx > 0) {
                output[out_idx++] = ' ';
                last_was_space = 1;
            }
        } else {
            // Добавляем непробельные символы
            output[out_idx++] = c;
            last_was_space = 0;
        }
    }

    // Удаляем последний пробел, если он есть
    if (out_idx > 0 && output[out_idx - 1] == ' ') {
        out_idx--;
    }

    // Завершаем строку
    output[out_idx] = '\0';
}