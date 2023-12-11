#include "server_defs.hpp"

sys_var sv;

void get_args(int argc, char **argv){
    int flag_p = 0;
    switch (argc){
        case 1:
            break;
        case 2:
            if (strcmp(argv[1],"-v")!=0) ERR("Invalid arguments")
            sv.verbose=true;
            break;
        case 3:
            if (strcmp(argv[1],"-p")!=0) ERR("Invalid arguments")
            sv.ASport = argv[2];
            if (!is_valid_port(sv.ASport)) ERR("Invalid port")
            break;
        case 4:
            if (strlen(argv[1])!=2 || argv[1][0]!='-') ERR("Invalid arguments")
            switch(argv[1][1]){
                case 'p':
                    sv.ASport = argv[2];
                    if (!is_valid_port(sv.ASport)) ERR ("Invalid port")
                    flag_p = 1;
                    break;
                case 'v':
                    sv.verbose = true;
                    break;
                default:
                    ERR("Invalid arguments")
            }
            switch(flag_p){
                case 0:
                    if (strcmp(argv[2],"-p")!=0) ERR("Invalid arguments")
                    sv.ASport = argv[3];
                    if (!is_valid_port(sv.ASport)) ERR("Invalid port")
                    break;
                case 1:
                    if (strcmp(argv[3],"-v")!=0) ERR("Invalid arguments")
                        sv.verbose = true;
                        break;
                default:
                    ERR("Invalid arguments")
                    STATUS("Unexpected behaviour")
            }
            break;
        default:
            printf("%d\n", argc);
            ERR("Invalid number of arguments")
    }
}

int output_request(string UID,string request,string ip,string port){
    if (!sv.verbose) return 0;
    //Checkar o tipo de request
    if (!is_valid_UID(UID) || !is_valid_ip(ip) || !is_valid_port(port)){
        LOG(sv.verbose, "Invalid request.")
        return -1;
    }
    LOG_WA(sv.verbose, "%s request by %s received:\nip:%s\nport:%s", request.c_str(),UID.c_str(),ip.c_str(),port.c_str())
    return 0;
}

int start_udp_socket() {
    // UDP SOCKET
    sv.UDP.fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(sv.UDP.fd == -1) {
        STATUS("Could not create socket [UDP]")
        return -1;
    }

    memset(&sv.UDP.hints, 0, sizeof sv.UDP.hints);
    sv.UDP.hints.ai_family=AF_INET; //IPv4
    sv.UDP.hints.ai_socktype=SOCK_DGRAM; //UDP socket
    sv.UDP.hints.ai_flags = AI_PASSIVE;

    sv.UDP.errcode = getaddrinfo(NULL,sv.ASport.c_str(),&sv.UDP.hints,&sv.UDP.res);
    if (sv.UDP.errcode != 0){
        STATUS("Could not get address info [UDP]")
        return -1;
    }

    sv.UDP.errcode = bind(sv.UDP.fd,sv.UDP.res->ai_addr,sv.UDP.res->ai_addrlen);
    if (sv.UDP.errcode==-1){
        STATUS("Could not bind to socket.")
        return -1;
    }

    return 0;
}

int end_udp_socket() {
    // Close UDP
    if (close(sv.UDP.fd) == -1) {
        STATUS("Could not close [UDP]")
        freeaddrinfo(sv.UDP.res);
        return -1;
    }

    freeaddrinfo(sv.UDP.res);
    return 0;

}

int start_tcp_socket(){
    sv.TCP.fd = socket(AF_INET,SOCK_STREAM,0);
    if (sv.TCP.fd == -1){
        STATUS("Could not create socket [TCP]")
        return -1;
    }

    memset(&sv.TCP.hints, 0, sizeof sv.TCP.hints);
    sv.TCP.hints.ai_family=AF_INET; //IPv4
    sv.TCP.hints.ai_socktype=SOCK_STREAM; //TCP socket
    sv.TCP.hints.ai_flags = AI_PASSIVE;

    sv.TCP.errcode = getaddrinfo(NULL,sv.ASport.c_str(),&sv.TCP.hints,&sv.TCP.res);
    if (sv.TCP.errcode != 0){
        STATUS("Could not get address info [TCP]")
        return -1;
    }

    sv.TCP.errcode = bind(sv.TCP.fd,sv.TCP.res->ai_addr,sv.TCP.res->ai_addrlen);
    if (sv.TCP.errcode == -1){
        STATUS("Could not bind [TCP].")
        return -1;
    }

    sv.TCP.errcode = listen(sv.TCP.fd,SOMAXCONN);
    if (sv.TCP.errcode == -1){
        STATUS("Could not listen [TCP].")
        return -1;
    }

}

int main(int argc, char** argv){

    get_args(argc,argv);

    size_t n;

    //divide em tcp e udp
    pid_t pid = fork();
    if (pid == -1){
        STATUS("Something went wrong")
        exit(EXIT_FAILURE);
    }

    //TCP
    if (pid>0){
        start_tcp_socket();

        while(1){
            sv.TCP.addrlen = sizeof(sv.TCP.addr);
            //...
        }

        //close_tcp_socket();
        
    }
    //UDP
    else if (pid == 0){
        start_udp_socket();

        while(1){
            sv.UDP.addrlen = sizeof(sv.UDP.addr);
            n = recvfrom(sv.UDP.fd,sv.UDP.buffer,BUFFER_SIZE,0,(struct sockaddr*) &sv.UDP.addr, &sv.UDP.addrlen);
            if (n==-1){
                STATUS("Error on receive from user.")
                exit(EXIT_FAILURE);
            }

            size_t req_pid = fork();

            if (req_pid == 0){
                STATUS_WA("Received request: %s",sv.UDP.buffer)
                string request(sv.UDP.buffer);
                exit(handleRequest(request));
            }

        }

        end_udp_socket();
    }


    
}