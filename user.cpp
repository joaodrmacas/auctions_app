#include "user.hpp"

//system variables
struct {
    string UID = NO_USER;
    string pass = NO_PASS;
} sv;

int is_valid_ip (string ip) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
    return result != 0;
}

int is_valid_port (string port) {
    int port_int = atoi(port.c_str());
    return port_int >= 0 && port_int <= 65535;
}

void get_args(int argc, char **argv, string &ASIP, string &ASport) {
    switch (argc) {
        case 1:
            break;
        case 3:
        case 5:
            if (strlen(argv[1]) != 2 || argv[1][0] != '-') ERR("Invalid arguments")
            switch (argv[1][1]) {
                case 'n':
                    ASIP=argv[2];
                    if (!is_valid_ip(ASIP)) ERR("Invalid IP")
                    break;
                case 'p':
                    ASport=argv[2];
                    if (!is_valid_port(ASport)) ERR("Invalid port")
                    break;
                default:
                    ERR("Invalid arguments")
            }

            if (argc == 5) {
                if (strlen(argv[3]) != 2 || argv[3][0] != '-' || argv[1][1]==argv[3][1]) ERR("Invalid arguments")
                switch (argv[1][1]) {
                    case 'n':
                        ASIP=argv[4];
                        if (!is_valid_ip(ASIP)) ERR("Invalid IP")
                        break;
                    case 'p':
                        ASport=argv[4];
                        if (!is_valid_port(ASport)) ERR("Invalid port")
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

string getMyIP(){
    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    if (hostname == -1) ERR("gethostname")
    host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL) ERR("gethostbyname")
    IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
    if (IPbuffer == NULL) ERR("inet_ntoa")
    return IPbuffer;
}

int cmd_login(istringstream &cmdstream, protocol &UDP) {
    string UID, pass;

    if (sv.UID != NO_USER){
        STATUS("User failed to login.")
        MSG("You are already logged in")
        return -1;
    }
    
    if ( !(cmdstream >> UID) ){
        MSG("UID not specified.")
        return -1;
    }

    for (char c : UID) {
        if (!std::isdigit(c)) {
            MSG("UID should be only numeric.")
            return -1;
        }
    }
    if (UID.length() != 6) {
        MSG("UID should be 6 digits.")
        return -1;
    }

    if (!(cmdstream >> pass)){
        MSG("Password is not specified.")
        return -1;
    }

    for (char c : pass) {
        if (!std::isalnum(c)) {
            MSG("Password should be alphanumeric.")
            return false;
        }
    }
    
    if (pass.length() != 8){
        MSG("Password should be 8 characters long.")
        return -1;
    }

    // always 21 chars long
    strcpy(UDP.buffer, "LIN ");
    strcat(UDP.buffer, UID.c_str());
    strcat(UDP.buffer, " ");
    strcat(UDP.buffer, pass.c_str());
    strcat(UDP.buffer, "\n\0");
    
    if(sendto(UDP.fd,UDP.buffer,128,0,UDP.res->ai_addr,UDP.res->ai_addrlen) == -1) {
        MSG("Login failed.")
        STATUS("Could not send login message")
        return -1;
    }
    STATUS("Login message sent.")

    if(recvfrom(UDP.fd,UDP.buffer,128,0,(struct sockaddr*) &UDP.addr,&UDP.addrlen)==-1) {
        MSG("Login failed.")
        STATUS("Could not receive login response.")
        return -1;
    }
    STATUS("Login response received.");

    if (string(UDP.buffer + 4)=="OK" || string(UDP.buffer + 4)=="REG"){
        sv.UID=UID;
        sv.pass=pass;
        if (UDP.buffer=="OK") MSG("successful login")
        else MSG("new user registered")
        STATUS("Login was successful.")
    }
    
    else if (string(UDP.buffer + 4)=="NOK"){
        MSG("incorrect login attempt")
        STATUS("The password is wrong. Try again.")
    }

    return 0;
}

int cmd_logout(istringstream &cmdstream, protocol &UDP) {
    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        STATUS("User not logged in.")
        return -1;
    }

    // always 21 chars long
    strcpy(UDP.buffer, "LOU ");
    strcat(UDP.buffer, sv.UID.c_str());
    strcat(UDP.buffer, " ");
    strcat(UDP.buffer, sv.pass.c_str());
    strcat(UDP.buffer, "\n\0");
    
    if(sendto(UDP.fd,UDP.buffer,128,0,UDP.res->ai_addr,UDP.res->ai_addrlen) == -1) {
        STATUS("Could not send logout message")
        return -1;
    }
    STATUS("Logout message sent.")

    UDP.addrlen=sizeof(UDP.addr);
    if(recvfrom(UDP.fd,UDP.buffer,128,0,(struct sockaddr*) &UDP.addr,&UDP.addrlen)==-1) {
        STATUS("Could not receive logout response.")
        return -1;
    }
    STATUS("Logout response received.")

    if (string(UDP.buffer + 4) == "OK") {
        sv.UID = NO_USER;
        sv.pass = NO_PASS;

        STATUS("Logout was successful.")
        MSG("Successful logout.")
    }    
    else if (string(UDP.buffer + 4) == "NOK") {
        STATUS("Logout was unsuccessful.")
        MSG("User not logged in")
    }
    else if (string(UDP.buffer + 4) == "UNR") {
        STATUS("The logout user doesn't exist")
        MSG("Unknown user.")
    }

    return 0;
}

int cmd_unregister(istringstream &cmdstream, protocol &UDP){
    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        return -1;
    }

    strcpy(UDP.buffer, "UNR ");
    strcat(UDP.buffer, sv.UID.c_str());
    strcat(UDP.buffer, " ");
    strcat(UDP.buffer, sv.pass.c_str());
    strcat(UDP.buffer, "\n\0");

    if(sendto(UDP.fd,UDP.buffer,128,0,UDP.res->ai_addr,UDP.res->ai_addrlen) == -1) {
        MSG("Unregister failed.");
        STATUS("Could not send unregister message")
        return -1;
    }
    STATUS("Unregister message sent.")
    
    UDP.addrlen=sizeof(UDP.addr);
    if(recvfrom(UDP.fd,UDP.buffer,128,0,(struct sockaddr*) &UDP.addr,&UDP.addrlen)==-1) {
        MSG("Unregister failed.");
        STATUS("Could not receive unregister response.")
        return -1;
    }
    STATUS("Unregister response received.")

    if (string(UDP.buffer)=="OK"){
        MSG("successful unregister")
        STATUS("Unregister was successful.")
        //logout
        sv.UID = NO_USER;
        sv.pass = NO_PASS;
    }
    
    else if (string(UDP.buffer)=="NOK"){
        STATUS("Unregister couldn't be done")
        MSG("incorrect unregister attempt")
    }

    else if (string(UDP.buffer+4)=="UNR"){
        STATUS("The unregister user doesnt exist")
        MSG("unknown user")
    }

    return 0;
}

int processCommand(string full_cmd, protocol &UDP, protocol &TCP) {
    
    istringstream cmdstream(full_cmd);
    string word, cmd;
    
    if (!(cmdstream >> cmd)){
        STATUS("invalid command")
        return -1;
    }

    if (cmd=="login") {
        if (cmd_login(cmdstream, UDP) == -1){
            STATUS("invalid login.")
        }
    }
    else if (cmd == "logout") {
        cmd_logout(cmdstream, UDP);
    }

    else if (cmd == "unregister") {
        if (cmd_unregister(cmdstream,UDP)==-1){
            STATUS("invalid unregister")
        }
    }
    // else if (cmd == "exit") {
    //     cmd_exit(cmdstream);
    // }
    // else if (cmd == "open") {
    //     cmd_open(cmdstream);
    // }
    // else if (cmd == "close") {
    //     cmd_close(cmdstream);
    // }
    // else if (cmd == "myauctions" || cmd == "ma") {
    //     cmd_myauctions(cmdstream);
    // }
    // else if (cmd == "create") {
    //     cmd_create(cmdstream);
    // }
    // else if (cmd == "mybids" || cmd == "mb") {
    //     cmd_mybids(cmdstream);
    // }
    // else if (cmd == "list" || cmd == "l") {
    //     cmd_list(cmdstream);
    // }
    // else if (cmd == "show_asset" || cmd == "sa") {
    //     cmd_show_asset(cmdstream);
    // }
    // else if (cmd == "bid" || cmd == "b") {
    //     cmd_bid(cmdstream);
    // }
    // else if (cmd == "show_records" || cmd == "sr") {
    //     cmd_show_records(cmdstream);
    // }
    else {
        STATUS("Invalid command")
    }

    // while (iss >> word) {
    //         std::cout << "Word: " << word << std::endl;
    // }
    return 0;
}

int main(int argc, char** argv){
    protocol UDP, TCP;
    string cmd;
    string ASIP=getMyIP(), ASport=PUBLIC_PORT;

    get_args(argc, argv, ASIP, ASport);
    
    // FIXME all socket errors treated as STATUS (no exit) 
    // UDP SOCKET
    UDP.fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(UDP.fd == -1) STATUS("Could not create socket [UDP]")

    memset(&UDP.hints, 0, sizeof UDP.hints);
    UDP.hints.ai_family=AF_INET; //IPv4
    UDP.hints.ai_socktype=SOCK_DGRAM; //UDP socket

    UDP.errcode=getaddrinfo( ASIP.c_str(), ASport.c_str() ,&UDP.hints,&UDP.res);
    if(UDP.errcode!=0) STATUS("Could not get address info [UDP]")

    // TCP SOCKET
    TCP.fd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if(TCP.fd == -1) STATUS("Could not create socket [TCP]")

    memset(&TCP.hints, 0, sizeof TCP.hints);
    TCP.hints.ai_family=AF_INET; //IPv4
    TCP.hints.ai_socktype=SOCK_STREAM; //TCP socket

    TCP.errcode=getaddrinfo( ASIP.c_str(), ASport.c_str() ,&TCP.hints,&TCP.res);
    if(TCP.errcode!=0) STATUS("Could not get address info [TCP]")

    if (connect(TCP.fd, TCP.res->ai_addr, TCP.res->ai_addrlen) == -1)
        STATUS("Could not connect [TCP]")

    while(1){
        cout << "> ";
        getline(cin, cmd);
        processCommand(cmd,UDP,TCP);
    }

    // n=sendto(fd,"Hello!\n",7,0,res->ai_addr,res->ai_addrlen);
    // if(n==-1) ERR("Could not send message")
    
    // addrlen=sizeof(addr);
    // n=recvfrom(fd,buffer,128,0,(struct sockaddr*) &addr,&addrlen);
    // if(n==-1) ERR("Could not receive message")
        
    // write(1,"echo: ",6);
    // write(1,buffer,n);

    return 0;
}

