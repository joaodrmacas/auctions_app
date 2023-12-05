#ifndef __DEFINITIONS_HPP__
#define __DEFINITIONS_HPP__

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

#define BUFFER_SIZE 6010

/*
-> 512 + 1 (1 for the case where the buffer receives 512 bytes and the last byte has to be'\0')
-> 512 bytes because a file name can have 255 caracteres (255 bytes) in UNIX
*/

#endif 