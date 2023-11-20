#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>


#define PUBLIC_PORT "58011"
#define PUBLIC_IP "tejo.tecnico.ulisboa.pt"
#define PORT "58036"


#define ERR(...) \
    {printf("[ERROR]: %s | Line %d\n", __VA_ARGS__, __LINE__);}

int main(int argc, char** argv){
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[128];

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1){
        ERR("");
        exit(1);
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket

    errcode=getaddrinfo(PUBLIC_IP,PUBLIC_PORT,&hints,&res);
    if(errcode!=0){
        ERR("");
        exit(1);
    }

    n=sendto(fd,"Hello!\n",7,0,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        ERR("")
        exit(1);
    }
    
    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,128,0,(struct sockaddr*) &addr,&addrlen);
    if(n==-1){
        ERR("")
        exit(1);
    }
    write(1,"echo: ",6);
    write(1,buffer,n);
}

