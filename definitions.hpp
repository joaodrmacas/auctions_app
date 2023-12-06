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
#include <regex>

using namespace std;

#define DEBUG 1
#define STATUS_ON 1

#define FILE_NAME_MAX_SIZE 24
#define FILE_MAX_SIZE 10000000
#define AID_SIZE 3
#define NAME_MAX_LEN 10
#define MAX_START_VALUE 999999
#define MAX_BID_VALUE 999999
#define MAX_TIME_ACTIVE 99999
#define UID_LEN 6
#define PASSWORD_LEN 8

typedef struct bid {
    string UID,date_time;
    int value, sec_time;
} bid;

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
    {printf(format "\n", __VA_ARGS__ );} \

#define BUFFER_SIZE 6010

#endif 