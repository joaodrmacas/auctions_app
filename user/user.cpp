#include "user_defs.hpp"
#include "commands.hpp"

//system variables
sys_var sv; 

int is_valid_ip (string ip) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
    return result != 0;
}

int is_valid_port (string port) {
    int port_int = atoi(port.c_str());
    return port_int >= 0 && port_int <= 65535;
}

void get_args(int argc, char **argv) {
    switch (argc) {
        case 1:
            break;
        case 3:
        case 5:
            if (strlen(argv[1]) != 2 || argv[1][0] != '-') ERR("Invalid arguments")
            switch (argv[1][1]) {
                case 'n':
                    sv.ASIP=argv[2];
                    if (!is_valid_ip(sv.ASIP)) ERR("Invalid IP")
                    break;
                case 'p':
                    sv.ASport=argv[2];
                    if (!is_valid_port(sv.ASport)) ERR("Invalid port")
                    break;
                default:
                    ERR("Invalid arguments")
            }

            if (argc == 5) {
                if (strlen(argv[3]) != 2 || argv[3][0] != '-' || argv[1][1]==argv[3][1]) ERR("Invalid arguments")
                switch (argv[1][1]) {
                    case 'n':
                        sv.ASIP=argv[4];
                        if (!is_valid_ip(sv.ASIP)) ERR("Invalid IP")
                        break;
                    case 'p':
                        sv.ASport=argv[4];
                        if (!is_valid_port(sv.ASport)) ERR("Invalid port")
                        break;
                    default:
                        ERR("Invalid arguments")
                }
            }

            break;
        default:
            ERR("Invalid number of arguments")
    }
}

int main(int argc, char** argv) {

    string cmd;

    get_args(argc, argv);
    
    memset(sv.UDP.buffer,0,129);
    memset(sv.TCP.buffer,0,129);

    // FIXME all socket errors treated as STATUS (no exit) 
    // UDP SOCKET
    sv.UDP.fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(sv.UDP.fd == -1) STATUS("Could not create socket [UDP]")

    memset(&sv.UDP.hints, 0, sizeof sv.UDP.hints);
    sv.UDP.hints.ai_family=AF_INET; //IPv4
    sv.UDP.hints.ai_socktype=SOCK_DGRAM; //UDP socket

    sv.UDP.errcode=getaddrinfo( sv.ASIP.c_str(), sv.ASport.c_str() ,&sv.UDP.hints,&sv.UDP.res);
    if(sv.UDP.errcode!=0) {
        STATUS("Could not get address info [UDP]")
        exit(1);
    }

    sv.UDP.addrlen=sizeof(sv.UDP.addr);

    while(!sv.to_exit){
        cout << "> ";
        getline(cin, cmd);
        processCommand(cmd);    
    }

    close(sv.UDP.fd);
    freeaddrinfo(sv.UDP.res);

    return 0;
}

