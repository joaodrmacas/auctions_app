#ifndef __USER_DEFS_HPP__
#define __USER_DEFS_HPP__

#include "../definitions.hpp"
#include "../validations.hpp"

#define PUBLIC_PORT "58011"
#define PUBLIC_IP "tejo.tecnico.ulisboa.pt"
#define MY_IP "localhost"

#define NO_USER ""
#define NO_PASS ""

typedef struct protocol {
    int fd,errcode;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];
} protocol;

typedef struct __sys_var__ {
    string ASIP=PUBLIC_IP, ASport=PUBLIC_PORT;
    protocol UDP, TCP;
    string UID = NO_USER;
    string pass = NO_PASS;
    int to_exit = 0;
} sys_var;

#endif
