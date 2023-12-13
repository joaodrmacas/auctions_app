#include "user_defs.hpp"

//system variables
sys_var sv; 

namespace fs = std::filesystem;

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
                switch (argv[3][1]) {
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

    fs::path assets_dir = fs::path(argv[0]).parent_path().append("assets");
    
    //cria na pasta do executavel.
    
    if(!fs::exists(assets_dir)){
        if (fs::create_directory(assets_dir)){
            STATUS("Assets directory created successfully")
        }
        else {
            MSG("Something went wrong.")
            STATUS("Failed to create assests directory.")
            exit(EXIT_FAILURE);
        }
    }
    
    memset(sv.UDP.buffer,0,BUFFER_SIZE);
    memset(sv.TCP.buffer,0,BUFFER_SIZE);

    while(!sv.to_exit){
        cout << "> ";
        getline(cin, cmd);
        processCommand(cmd);    
    }

    return 0;
}

