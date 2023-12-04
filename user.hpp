#ifndef USER_HPP
#define USER_HPP

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

#define DEBUG 1
#define STATUS_ON 1

#define PUBLIC_PORT "58011"
#define PUBLIC_IP "tejo.tecnico.ulisboa.pt"
#define MY_IP "localhost"
#define PORT "58036"

#define NO_USER ""
#define NO_PASS ""

#define FILE_NAME_MAX_SIZE 24
#define FILE_MAX_SIZE 10000000

#define ERR(...)                                                        \
    {                                                                   \
        if (DEBUG)                                                      \
            printf("[ERROR]: %s | Line %d\n", __VA_ARGS__, __LINE__);   \
        exit(1);                                                        \
    }

#define STATUS(msg)                                                     \
    {if (STATUS_ON) printf("[STATUS]: %s | Line %d\n", msg, __LINE__);}

#define STATUS_WA(format, ...)                                          \
    {if (STATUS_ON) printf("[STATUS]: " format " | Line %d\n", __VA_ARGS__, __LINE__);}

#define MSG(msg)                    \
    {printf("%s\n", msg);}               \

#define MSG_WA(format, ...)         \
    {printf(format, __VA_ARGS__ );} \

typedef struct protocol {
    int fd,errcode;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[512];
} protocol;

#define BUFFER_SIZE 511

/*
-> 512 + 1 (1 for the case where the buffer receives 512 bytes and the last byte has to be'\0')
-> 512 bytes because a file name can have 255 caracteres (255 bytes) in UNIX
*/

#endif 