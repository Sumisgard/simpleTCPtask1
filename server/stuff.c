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
#include <stdbool.h>

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

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void takeAction(char* msg, size_t size) {
    if (size == 0 || msg == NULL)
        return;

    size_t max_len = 0;
    size_t current_len = 0;
    bool in_word = false;

    // Определяем длину самого длинного слова
    for (const char *p = msg; p < msg + size && *p != '\0'; p++) {
        if (*p != ' ') {
            if (!in_word) {
                in_word = true;
                current_len = 1;
            } else {
                current_len++;
            }
        } else {
            if (in_word) {
                if (current_len > max_len)
                    max_len = current_len;
                in_word = false;
                current_len = 0;
            }
        }
    }
    if (in_word && current_len > max_len)
        max_len = current_len;

    // Удаляем первое самое длинное слово
    char *read = msg;
    char *write = msg;
    bool removed = false;
    bool first = true;

    while (read < msg + size && *read != '\0') {
        // Пропускаем пробелы
        while (read < msg + size && *read == ' ')
            read++;

        if (read >= msg + size || *read == '\0')
            break;

        // Начало слова
        char *word_start = read;
        size_t word_len = 0;

        // Вычисляем длину слова
        while (read < msg + size && *read != ' ' && *read != '\0') {
            read++;
            word_len++;
        }

        // Проверяем, нужно ли удалить это слово
        if (word_len == max_len && !removed) {
            removed = true;
            continue;
        }

        // Копируем слово, если не удалено
        if (!first) {
            *write++ = ' ';
        } else {
            first = false;
        }

        memcpy(write, word_start, word_len);
        write += word_len;
    }

    // Добавляем нуль-терминатор, если возможно
    if (write <= msg + size) {
        *write = '\0';
    }
}