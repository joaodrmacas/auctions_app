#include "server_defs.hpp"

sys_var sv;

void get_args(int argc, char **argv){
    int flag_p = 0;
    switch (argc){
        case 1:
            break;
        case 2:
            if (strcmp(argv[1],"-v")!=0) ERR("Invalid arguments")
            sv.verbose_mode=true;
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
                    sv.verbose_mode = true;
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
                        sv.verbose_mode = true;
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
    if (!sv.verbose_mode) return 0;
    //Checkar o tipo de request
    if (!is_valid_UID(UID) || !is_valid_ip(ip) || !is_valid_port(port)){
        MSG("Invalid request.")
        return -1;
    }
    MSG_WA("%s request by %s received:\nip:%s\nport:%s", request.c_str(),UID.c_str(),ip.c_str(),port.c_str())
    return 0;
}

int main(int argc, char** argv){
    
    get_args(argc,argv);

}