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

    while(!sv.to_exit){
        cout << "> ";
        getline(cin, cmd);
        processCommand(cmd);    
    }

    return 0;
}

