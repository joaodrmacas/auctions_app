#include "user.hpp"

//system variables
struct {
    protocol UDP, TCP;
    string UID = NO_USER;
    string pass = NO_PASS;
    int to_exit = 0;
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

int cmd_login(istringstream &cmdstream) {
    string UID, pass;
    char status[3];

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
        if (!isdigit(c)) {
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
        if (!isalnum(c)) {
            MSG("Password should be alphanumeric.")
            return false;
        }
    }
    
    if (pass.length() != 8){
        MSG("Password should be 8 characters long.")
        return -1;
    }

    string sbuff = "LIN " + UID + " " + pass + "\n";

    if(sendto(sv.UDP.fd, sbuff.c_str(), sbuff.length(), 0, sv.UDP.res->ai_addr, 
                sv.UDP.res->ai_addrlen) == -1) {
        MSG("Login failed.")
        STATUS("Could not send login message")
        return -1;
    }
    STATUS("Login message sent.")

    memset(sv.UDP.buffer,0,128);

    // sv.UDP.addrlen=sizeof(sv.UDP.addr);
    size_t n = recvfrom(sv.UDP.fd,sv.UDP.buffer,128,0,(struct sockaddr*) &sv.UDP.addr,&sv.UDP.addrlen);
    if(n==-1) {
        MSG("Login failed.")
        STATUS("Could not receive login reply.")
        return -1;
    }
    STATUS("Login reply received.");

    sv.UDP.buffer[n-1]='\0';

    if (string(sv.UDP.buffer).compare(0, 4, "RLI "))

    sbuff = string(sv.UDP.buffer + 4);

    if (status[0]=='O' && status[1]=='K'  || strcmp(status,"REG")==0){
        sv.UID=UID;
        sv.pass=pass;
        if (strcmp(status,"REG")==0) MSG("new user registered")
        else MSG("successful login")
        STATUS("Login was successful.")
    }
    
    else if (strcmp(status,"NOK")==0){
        MSG("incorrect login attempt")
        STATUS("The password is wrong. Try again.")
    }

    return 0;
}

int cmd_logout() {
    char status[3];

    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        STATUS("User not logged in.")
        return -1;
    }
    
    string sbuff = "LOU " + sv.UID + " " + sv.pass + "\n";

    if(sendto(sv.UDP.fd, sbuff.c_str(), sbuff.length(),0,sv.UDP.res->ai_addr,sv.UDP.res->ai_addrlen) == -1) {
        STATUS("Could not send logout message")
        return -1;
    }
    STATUS("Logout message sent.")

    memset(sv.UDP.buffer,0,128);

    // sv.UDP.addrlen=sizeof(sv.UDP.addr);
    size_t n = recvfrom(sv.UDP.fd,sv.UDP.buffer,128,0,(struct sockaddr*) &sv.UDP.addr,&sv.UDP.addrlen);
    if(n==-1) {
        STATUS("Could not receive logout reply.")
        return -1;
    }
    STATUS("Logout reply received.")

    sv.UDP.buffer[n-1]='\0';
    sbuff = string(sv.UDP.buffer + 4);

    if (sbuff == "OK") {
        sv.UID = NO_USER;
        sv.pass = NO_PASS;
        MSG("Successful logout.")
    }    
    else if (sbuff == "NOK") MSG("User not logged in")
    else if (sbuff == "UNR") MSG("Unknown user.")
    else if (sbuff == "ERR") MSG("Wrong syntax.")
    else MSG("Something went wrong. Can't comprehend server's reply.")
 
    return 0;
}

int cmd_unregister() {
    char status[3];

    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        return -1;
    }

    memset(sv.UDP.buffer,0,128);
    strcpy(sv.UDP.buffer, "UNR ");
    strcat(sv.UDP.buffer, sv.UID.c_str());
    strcat(sv.UDP.buffer, " ");
    strcat(sv.UDP.buffer, sv.pass.c_str());
    strcat(sv.UDP.buffer, "\n");

    if(sendto(sv.UDP.fd,sv.UDP.buffer,strlen(sv.UDP.buffer),0,sv.UDP.res->ai_addr,sv.UDP.res->ai_addrlen) == -1) {
        MSG("Unregister failed.");
        STATUS("Could not send unregister message")
        return -1;
    }
    STATUS("Unregister message sent.")

    memset(sv.UDP.buffer,0,128);
    
    //sv.UDP.addrlen=sizeof(sv.UDP.addr);

    if(recvfrom(sv.UDP.fd,sv.UDP.buffer,128,0,(struct sockaddr*) &sv.UDP.addr,&sv.UDP.addrlen)==-1) {
        MSG("Unregister failed.");
        STATUS("Could not receive unregister reply.")
        return -1;
    }
    STATUS("Unregister reply received.")

    strncpy(status,sv.UDP.buffer+4,3);

    if (status[0]=='O' && status[1]=='K'){
        MSG("successful unregister")
        STATUS("Unregister was successful.")
        //logout
        sv.UID = NO_USER;
        sv.pass = NO_PASS;
    }
    
    else if (strcmp(status,"NOK")==0){
        STATUS("Unregister couldn't be done")
        MSG("incorrect unregister attempt")
    }

    else if (strcmp(status,"UNR")==0){
        STATUS("The unregister user doesnt exist")
        MSG("unknown user")
    }

    return 0;
}

int cmd_exit() {
    if (sv.UID!=NO_USER){
        MSG("You need to logout before exiting the program.")
        return -1;
    }

    STATUS("Exiting program...")
    sv.to_exit = 1;
}

int cmd_open(istringstream &cmdstream) {
    string name,asset_fname;
    float start_value;
    int timeactive;

    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        return -1;
    }

    if ( !(cmdstream >> name) ){
        MSG("name is not specified.")
        return -1;
    }

    if ( !(cmdstream >> asset_fname) ){
        MSG("Asset filename is not specified.")
        return -1;
    }

    if (asset_fname.length() > FILE_NAME_MAX_SIZE) {
        MSG("Asset filename is too long.")
        return -1;
    }

    if ( !(cmdstream >> start_value) ){
        MSG("start_value is not a valid float.")
        return -1;
    }

    // is float so we compare a little under the zero
    if (start_value < -0.000001) {
        MSG("start_value is negative.")
        return -1;
    }

    if ( !(cmdstream >> timeactive)){
        MSG("timeactive is not a valid int.")
        return -1;
    }
    
    if (cmdstream.eof()) {
        MSG("too many arguments.")
        return -1;
    }

    // Message
    string sbuff = "OPA " + sv.UID + " " + sv.pass + " " + name + " " 
                    + to_string(start_value) + " " + to_string(timeactive) + " "
                    + asset_fname + " ";

    size_t n = write(sv.TCP.fd, sbuff.c_str(), sbuff.length());
    if (n != sbuff.length()) {
        MSG("Could not send open message")
        return -1;
    }
    
    fstream fasset(asset_fname, ios::binary | ios::ate);

    if (fasset.is_open()) {
        int size = fasset.tellg();

        if (size > FILE_MAX_SIZE) {
            MSG("Asset file is too big (> ", FILE_MAX_SIZE, " bytes)")
            return -1;
        }

        fasset.seekg(0, ios::beg);
        
        sbuff = to_string(size) + " ";
        n = write(sv.TCP.fd, sbuff.c_str(), sbuff.length());
        if (n != sbuff.length()) {
            MSG("Could not send open message")
            return -1;
        }

        while (size > 0) {
            fasset.read(sv.TCP.buffer, 128);
            n = write(sv.TCP.fd, sv.TCP.buffer, fasset.gcount());
            if (n != fasset.gcount()) {
                MSG("Could not send open message")
                return -1;
            }
            size -= fasset.gcount();
        }

        if (write(sv.TCP.fd, "\n", 1) != 1) {
            MSG("Could not send open message")
            return -1;
        }
    }
    else {
        MSG("Could not open asset file")
        return -1;
    }

    // Reply
    // devemos enviar sempre ERR ao server se recebermos do server uma mensagem
    // que não seja coerente com o protocolo?
}



int cmd_close(istringstream &cmdstream){
    string AID;

    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        return -1;
    }

    if (!(cmdstream >> AID) ){
        MSG("AID not specified.")
        return -1;
    }

    string buff = sv.UID + " " + sv.pass + " " + AID + "\n";
    size_t n = write(sv.TCP.fd, buff.c_str(), buff.length());


    if (n != buff.length()) {
        MSG("Close failed.");
        STATUS("Could not send close request.");
        return -1;
    }

    memset(sv.TCP.buffer,0,128);
    n = read(sv.TCP.fd, sv.TCP.buffer, 128);

    if (n == -1){
        MSG("Close failed.");
        STATUS("Could not receive close reply.");
    }

    //Reply este é perciso? demos memset em cima mas a resposta pode ter lixo?
    sv.UDP.buffer[n-1] = '\0';
    buff = string(sv.UDP.buffer + 4);


}

int processCommand(string full_cmd) {
    
    istringstream cmdstream(full_cmd);
    string word, cmd;
    
    if (!(cmdstream >> cmd)){
        STATUS("invalid command")
        return -1;
    }

    if (cmd=="login") {
        if (cmd_login(cmdstream) == -1){
            STATUS("invalid login.")
        }
    }
    else if (cmd == "logout") {
        if (cmd_logout()==-1){
            STATUS("invalid logout")
        }
    }

    else if (cmd == "unregister") {
        if (cmd_unregister()==-1){
            STATUS("invalid unregister")
        }
    }

    else if (cmd == "exit") {
        if (cmd_exit()==-1){
            STATUS("invalid exit")
        }
    }

    else if (cmd == "open") {
        if (cmd_open(cmdstream)==-1){
            STATUS("invalid open")
        };
    }
    else if (cmd == "close") {
        if (cmd_close(cmdstream)==-1){
            STATUS("invalid close")
        }
    }
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

int main(int argc, char** argv) {
    string cmd;
    string ASIP=PUBLIC_IP, ASport=PUBLIC_PORT;

    get_args(argc, argv, ASIP, ASport);
    
    // FIXME all socket errors treated as STATUS (no exit) 
    // UDP SOCKET
    sv.UDP.fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(sv.UDP.fd == -1) STATUS("Could not create socket [UDP]")

    memset(&sv.UDP.hints, 0, sizeof sv.UDP.hints);
    sv.UDP.hints.ai_family=AF_INET; //IPv4
    sv.UDP.hints.ai_socktype=SOCK_DGRAM; //UDP socket

    sv.UDP.errcode=getaddrinfo( ASIP.c_str(), ASport.c_str() ,&sv.UDP.hints,&sv.UDP.res);
    if(sv.UDP.errcode!=0) {
        STATUS("Could not get address info [UDP]")
        exit(1);
    }

    // TCP SOCKET
    sv.TCP.fd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if(sv.TCP.fd == -1) {
        STATUS("Could not create socket [TCP]")
        freeaddrinfo(sv.UDP.res);
        exit(1);
    }

    memset(&sv.TCP.hints, 0, sizeof sv.TCP.hints);
    sv.TCP.hints.ai_family=AF_INET; //IPv4
    sv.TCP.hints.ai_socktype=SOCK_STREAM; //TCP socket

    sv.TCP.errcode=getaddrinfo( ASIP.c_str(), ASport.c_str() ,&sv.TCP.hints,&sv.TCP.res);
    if(sv.TCP.errcode!=0) {
        STATUS("Could not get address info [TCP]")
        freeaddrinfo(sv.UDP.res);
        close(sv.UDP.fd);
        exit(1);
    }

    if (connect(sv.TCP.fd, sv.TCP.res->ai_addr, sv.TCP.res->ai_addrlen) == -1) {
        STATUS("Could not connect [TCP]")
        freeaddrinfo(sv.UDP.res);
        close(sv.UDP.fd);
        freeaddrinfo(sv.TCP.res);
        exit(1);
    }

    while(!sv.to_exit){
        cout << "> ";
        getline(cin, cmd);
        processCommand(cmd);
    }

    freeaddrinfo(sv.UDP.res);
    close(sv.UDP.fd);
    freeaddrinfo(sv.TCP.res);
    close(sv.TCP.fd);

    return 0;
}

