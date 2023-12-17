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
            ERR("Invalid number of arguments")
    }
}

void initiateDB(){
    fs::path db_dir = fs::path(DB_DIR_PATH);
    fs::path users_dir = fs::path(DB_DIR_PATH).append(USERS_DIR_PATH);
    fs::path auctions_dir = fs::path(DB_DIR_PATH).append(AUCTIONS_DIR_PATH);


    if(fs::exists(db_dir)){
        if (!fs::remove_all(db_dir)){
            STATUS("Failed to delete DB directory.")
            exit(EXIT_FAILURE);
        }
    }

    if (!fs::create_directory(db_dir)){
        STATUS("Failed to create DB directory.")
        exit(EXIT_FAILURE);
    }

    if(fs::exists(users_dir)){
        if (!fs::remove_all(users_dir)){
            STATUS("Failed to delete USERS directory.")
            exit(EXIT_FAILURE);
        }
    }

    if (!fs::create_directory(users_dir)){
        STATUS("Failed to create USERS directory.")
        exit(EXIT_FAILURE);
    }
    
    if(fs::exists(auctions_dir)){
        if (!fs::remove_all(auctions_dir)){
            STATUS("Failed to delete AUCTIONS directory.")
            exit(EXIT_FAILURE);
        }
    }

    if (!fs::create_directory(auctions_dir)){
        STATUS("Failed to create AUCTIONS directory.")
        exit(EXIT_FAILURE);
    }

    ofstream next_AID_file(sv.next_AID_file);

    if (!next_AID_file.is_open()) {
        STATUS("Failed to open NEXT_AID file.")
        exit(EXIT_FAILURE);
    }

    next_AID_file << "001";

    next_AID_file.close();

    STATUS("DB initiated successfully.")
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

    return 0;
}

int end_tcp_socket(){
    if (close(sv.TCP.fd) == -1) {
        STATUS("Could not close [TCP]")
        freeaddrinfo(sv.TCP.res);
        return -1;
    }

    freeaddrinfo(sv.TCP.res);
    return 0;
}

void signalHandlerUDP( int signum ) {

    if (signum == SIGINT || signum == SIGTERM){
        end_udp_socket();
    }
    else if (signum == SIGCHLD){
        int status;
        pid_t terminated_pid = wait(&status);
        STATUS_WA("Subprocess %d terminated with status: %d",terminated_pid,status)
        return;
    }

   exit(signum);  
}

void ignoreSignal( int signum ){
    STATUS_WA("Signal %d was ignored", signum)
} 

void signalHandlerTCP( int signum ){

    if (signum == SIGINT || signum == SIGTERM){
        end_tcp_socket();
        STATUS("Process ended safely")
    }
    else if (signum == SIGCHLD){
        int status;
        pid_t terminated_pid = wait(&status);
        STATUS_WA("Subprocess %d terminated with status: %d",terminated_pid,status)
        return;
    }

   exit(signum); 
}

int main(int argc, char** argv){
    size_t n;

    get_args(argc,argv);

    initiateDB();

    memset(sv.UDP.buffer,0,BUFFER_SIZE + 1);
    memset(sv.TCP.buffer,0,BUFFER_SIZE + 1);

    //divide em tcp e udp
    pid_t pid = fork();
    if (pid == -1){
        STATUS("Something went wrong")
        exit(EXIT_FAILURE);
    }

    //TCP
    if (pid>0){
        signal(SIGINT, signalHandlerTCP);
        signal(SIGTERM, signalHandlerTCP);
        signal(SIGCHLD, signalHandlerTCP);
        int newfd=0;

        if (start_tcp_socket() == -1){
            exit(EXIT_FAILURE);
        }


        while(1){
            sv.TCP.addrlen = sizeof(sv.TCP.addr);
            newfd=accept(sv.TCP.fd,(struct sockaddr*) &sv.TCP.addr, &sv.TCP.addrlen);
            if (newfd == -1){
                STATUS("[TCP] - Error receiving request from user.")
                exit(EXIT_FAILURE);
            }

            size_t req_pid = fork();

            if (req_pid == 0){
                // signal(SIGINT, ignoreSignal);
                // signal(SIGTERM, ignoreSignal);
                signal(SIGCHLD, ignoreSignal);

                memset(sv.TCP.buffer,0,BUFFER_SIZE + 1);

                sv.TCP.fd = newfd;

                int status = handle_TCP_req();
                end_tcp_socket();

                exit(status);
            }
        }

        exit(end_tcp_socket());
        
    }

    //UDP
    else if (pid == 0){
        signal(SIGINT, signalHandlerUDP);
        signal(SIGTERM, signalHandlerUDP);
        signal(SIGCHLD, signalHandlerUDP);
        
        if (start_udp_socket()==-1){
            exit(EXIT_FAILURE);
        }

        while(1){
            memset(sv.UDP.buffer,0,BUFFER_SIZE + 1);
            sv.UDP.addrlen = sizeof(sv.UDP.addr);
            n = recvfrom(sv.UDP.fd,sv.UDP.buffer,BUFFER_SIZE,0,(struct sockaddr*) &sv.UDP.addr, &sv.UDP.addrlen);
            if (n==-1){
                STATUS("[UDP] - Error receiving request from user.")
                exit(EXIT_FAILURE);
            }

            size_t req_pid = fork();

            if (req_pid == 0){
                // signal(SIGINT, ignoreSignal);
                // signal(SIGTERM, ignoreSignal);
                signal(SIGCHLD, ignoreSignal);

                
                LOG_WA(sv.verbose, "Received request: %s",sv.UDP.buffer)
                string request(sv.UDP.buffer);

                int status = handle_UDP_req(request);
                exit(status);
            }

        }

        exit(end_udp_socket());
    }


    
}