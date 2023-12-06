#include "user_defs.hpp"
#include "commands.hpp"

extern sys_var sv;

int start_udp() {
    // UDP SOCKET
    sv.UDP.fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(sv.UDP.fd == -1) {
        STATUS("Could not create socket [UDP]")
        return -1;
    }

    memset(&sv.UDP.hints, 0, sizeof sv.UDP.hints);
    sv.UDP.hints.ai_family=AF_INET; //IPv4
    sv.UDP.hints.ai_socktype=SOCK_DGRAM; //UDP socket

    sv.UDP.errcode=getaddrinfo( sv.ASIP.c_str(), sv.ASport.c_str() ,&sv.UDP.hints,&sv.UDP.res);
    if(sv.UDP.errcode!=0) {
        STATUS("Could not get address info [UDP]")
        return -1;
    }

    sv.UDP.addrlen=sizeof(sv.UDP.addr);

    return 0;
}

int end_udp() {
    // Close UDP
    if (close(sv.UDP.fd) == -1) {
        MSG("Something went wrong.")
        STATUS("Could not close [UDP]")
        freeaddrinfo(sv.UDP.res);
        return -1;
    }

    freeaddrinfo(sv.UDP.res);
    return 0;

}

int start_tcp() {
    // TCP SOCKET
    sv.TCP.fd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if(sv.TCP.fd == -1) {
        STATUS("Could not create socket [TCP]")
        return -1;
    }

    memset(&sv.TCP.hints, 0, sizeof sv.TCP.hints);
    sv.TCP.hints.ai_family=AF_INET; //IPv4
    sv.TCP.hints.ai_socktype=SOCK_STREAM; //TCP socket

    sv.TCP.errcode=getaddrinfo( sv.ASIP.c_str(), sv.ASport.c_str() ,&sv.TCP.hints,&sv.TCP.res);
    if(sv.TCP.errcode!=0) {
        STATUS("Could not get address info [TCP]")
        exit(1);
    }

    sv.TCP.addrlen=sizeof(sv.TCP.addr);

    // Connect to TCP
    if (connect(sv.TCP.fd, sv.TCP.res->ai_addr, sv.TCP.res->ai_addrlen) == -1) {
        MSG("Something went wrong.")
        STATUS("Could not connect [TCP]")
        return -1;
    }

    return 0;
}

int end_tcp() {
    // Close to TCP
    if (close(sv.TCP.fd) == -1) {
        MSG("Something went wrong.")
        STATUS("Could not close [TCP]")
        freeaddrinfo(sv.TCP.res);
        return -1;
    }

    freeaddrinfo(sv.TCP.res);
    return 0;
}

bool is_valid_AID(string AID){
     for (char c : AID) {
        if (!isdigit(c)) {
            return false;  // If any character is not a digit, the string is not numeric
        }
    }
    return AID.length() == AID_SIZE;
}

bool is_valid_state(string state){
    return state == "0" || state == "1";
}

bool is_valid_fname(string fname){
    if (fname.length()>FILE_NAME_MAX_SIZE) return false;
    for (char c: fname){
        if (!isalnum(c) && c!='-' && c!='_' && c!='.') return false;
    }
    return true;
}

bool is_valid_fsize(int fsize){
    return fsize < FILE_MAX_SIZE;
}

bool is_valid_date_time(const string& dateTimeString) {
    // Define the regular expression pattern for the specified format
    std::regex dateTimeRegex("^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}$");

    // Check if the string matches the pattern
    if (std::regex_match(dateTimeString, dateTimeRegex)) {
        // Extract individual components and check validity
        int year, month, day, hour, minute, second;
        sscanf(dateTimeString.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);

        // Check validity of individual components
        if (year >= 0 && month >= 1 && month <= 12 && day >= 1 && day <= 31 &&
            hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59 && second >= 0 && second <= 59) {
            return true; // Valid format and valid components
        }
    }

    return false; // Invalid format or invalid components
}

int cmd_login(istringstream &cmdstream) {
    string UID, pass;

    if (sv.UID != NO_USER){
        MSG("You are already logged in.")
        return -1;
    }
    STATUS("User isn't logged in.")
    
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
    if (UID.length() != UID_LEN) {
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
            return -1;
        }
    }
    
    if (pass.length() != PASSWORD_LEN){
        MSG("Password should be 8 characters long.")
        return -1;
    }

    if (!cmdstream.eof()) {
        MSG("Too many arguments.")
        return -1;
    }

    string sbuff = "LIN " + UID + " " + pass + "\n";

    if(sendto(sv.UDP.fd, sbuff.c_str(), sbuff.length(), 0, sv.UDP.res->ai_addr, 
                sv.UDP.res->ai_addrlen) == -1) {
        MSG("Something went wrong.")
        STATUS("Could not send login message")
        return -1;
    }
    STATUS("Login message sent.")

    // Reply

    //memset(sv.UDP.buffer,0,128);

    // sv.UDP.addrlen=sizeof(sv.UDP.addr);
    size_t n = recvfrom(sv.UDP.fd,sv.UDP.buffer,BUFFER_SIZE,0,(struct sockaddr*) &sv.UDP.addr,&sv.UDP.addrlen);
    if(n <= 0) {
        MSG("Something went wrong.")
        if (n == -1) STATUS("Could not receive login reply.")
        else STATUS("Login reply is empty.")
        return -1;
    }
    
    // Retirar o \n no final e colocar \0
    sv.UDP.buffer[n-1]='\0';

    STATUS_WA("Login reply received: %s", sv.UDP.buffer);

    istringstream reply(string(sv.UDP.buffer));

    string opcode;
    if (!(reply >> opcode)) {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply: no opcode.")
        return -1;
    }
    
    if (opcode == "RLI") {
        string status;
        if (!(reply >> status)) {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply: no status.")
            return -1;
        }
        
        if (status == "OK"  || status == "REG") {
            sv.UID=UID;
            sv.pass=pass;
            if (status == "REG") MSG("Registration was successful. Logged in.")
            else MSG("Login was successful.")
        }
        else if (status == "NOK") MSG("The password is wrong. Try again.")
        else {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply.")
            return -1;
        }
    }
    else if (opcode == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }

    return 0;
}

int cmd_logout() {
    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        return -1;
    }
    STATUS("User is logged in.")

    string sbuff = "LOU " + sv.UID + " " + sv.pass + "\n";

    if(sendto(sv.UDP.fd, sbuff.c_str(), sbuff.length(), 0, sv.UDP.res->ai_addr, sv.UDP.res->ai_addrlen) == -1) {
        MSG("Something went wrong.")
        STATUS("Could not send logout message")
        return -1;
    }
    STATUS("Logout message sent.")

    //memset(sv.UDP.buffer,0,128);

    // sv.UDP.addrlen=sizeof(sv.UDP.addr);
    size_t n = recvfrom(sv.UDP.fd,sv.UDP.buffer,BUFFER_SIZE,0,(struct sockaddr*) &sv.UDP.addr,&sv.UDP.addrlen);
    if(n <= 0) {
        MSG("Something went wrong.")
        if (n == -1) STATUS("Could not receive logout reply.")
        else STATUS("Logout reply is empty.")
        return -1;
    }

    // Retirar o \n no final e colocar \0
    sv.UDP.buffer[n-1]='\0';

    STATUS_WA("Logout reply received: %s", sv.UDP.buffer);

    istringstream reply(string(sv.UDP.buffer));

    string opcode;
    if (!(reply >> opcode)) {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply: no opcode.")
        return -1;
    }
    
    if (opcode == "RLO") {
        string status;

        if (!(reply >> status)) {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply: no status.")
            return -1;
        }
        
        if (status == "OK") {
            sv.UID = NO_USER;
            sv.pass = NO_PASS;
            MSG("Successful logout.")
        }
        else if (status == "NOK") MSG("User not logged in.")
        else if (status == "UNR") MSG("Unknown user.")
        else {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply: status not reconizable.")
            return -1;
        }
    }
    else if (opcode == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply: opcode not reconizable")
        return -1;
    }
 
    return 0;
}

int cmd_unregister() {
    char status[3];

    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        return -1;
    }

    string sbuff = "UNR " + sv.UID + " " + sv.pass + "\n";

    if(sendto(sv.UDP.fd, sbuff.c_str(), sbuff.length(), 0, sv.UDP.res->ai_addr, sv.UDP.res->ai_addrlen) == -1) {
        MSG("Something went wrong.")
        STATUS("Could not send unregister message.")
        return -1;
    }
    STATUS("Unregister message sent.")

    //memset(sv.UDP.buffer,0,128);
    
    //sv.UDP.addrlen=sizeof(sv.UDP.addr);
    size_t n = recvfrom(sv.UDP.fd,sv.UDP.buffer,BUFFER_SIZE,0,(struct sockaddr*) &sv.UDP.addr,&sv.UDP.addrlen);
    if(n <= 0) {
        MSG("Something went wrong.")
        if (n == -1) STATUS("Could not receive unregister reply.")
        else STATUS("Unregister reply is empty.")
        return -1;
    }

    // Retirar o \n no final e colocar \0
    sv.UDP.buffer[n-1]='\0';

    STATUS_WA("Unregister reply received: %s", sv.UDP.buffer);

    istringstream reply(string(sv.UDP.buffer));

    string opcode;
    if (!(reply >> opcode)) {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply: no opcode.")
        return -1;
    }

    if (opcode == "RUR") {
        string status;

        if (!(reply >> status)) {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply: no status.")
            return -1;
        }
        
        if (status == "OK") {
            sv.UID = NO_USER;
            sv.pass = NO_PASS;
            MSG("Unregister was successful. You're now logged out.")
        }
        else if (status == "NOK") MSG("User not logged in.")
        else if (status == "UNR") {
            sv.UID = NO_USER;
            sv.pass = NO_PASS;
            MSG("Your user is no longer registered. You will be logged out.")
        } 
        else {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply: status not reconizable.")
            return -1;
        }
    }
    else if (opcode == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply: opcode not reconizable")
        return -1;
    }
    
    return 0;
}

int cmd_exit() {
    if (sv.UID!=NO_USER){
        MSG("You need to logout before exiting the program.")
        return -1;
    }

    MSG("Exiting program...")
    sv.to_exit = 1;

    return 0;
}

int cmd_open(istringstream &cmdstream) {
    string name, asset_fname;
    int start_value, timeactive;

    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        return -1;
    }

    if ( !(cmdstream >> name) ){
        MSG("Name is not specified.")
        return -1;
    }

    if (name.length() > NAME_MAX_LEN) {
        MSG("Name is too long.")
        return -1;
    }

    for (char c : name) {
        if (!isalnum(c)) {
            MSG("Name should be alphanumeric.")
            return -1;
        }
    }

    if ( !(cmdstream >> asset_fname) ){
        MSG("Asset filename is not specified.")
        return -1;
    }

    if (!is_valid_fname(asset_fname)) {
        MSG("Asset filename is not valid.")
        return -1;
    }

    if ( !(cmdstream >> start_value) ){
        MSG("Start_value is not a valid integer.")
        return -1;
    }

    if (start_value > MAX_START_VALUE) {
        MSG("Start_value is too big (> 999999).")
        return -1;
    }

    if (start_value < 0) {
        MSG("Start_value can't be negative.")
        return -1;
    }

    if ( !(cmdstream >> timeactive)){
        MSG("Timeactive is not a valid integer.")
        return -1;
    }

    if (timeactive > MAX_TIME_ACTIVE) {
        MSG("Timeactive is too big (> 99999).")
        return -1;
    }

    if (timeactive < 0) {
        MSG("Timeactive can't be negative.")
        return -1;
    }

    if (!cmdstream.eof()) {
        MSG("Too many arguments.")
        return -1;
    }

    // Message
    string sbuff = "OPA " + sv.UID + " " + sv.pass + " " + name + " " 
                    + to_string(start_value) + " " + to_string(timeactive) + " "
                    + asset_fname + " ";

    size_t n = write(sv.TCP.fd, sbuff.c_str(), sbuff.length());
    if (n != sbuff.length()) {
        MSG("Something went wrong.")
        STATUS("Could not send open request")
        return -1;
    }
    
    ifstream fasset(asset_fname, ios::ate);

    if (fasset.is_open()) {
        int size = fasset.tellg();

        if (size > FILE_MAX_SIZE) {
            MSG_WA("Asset file is too big (> %d) bytes", FILE_MAX_SIZE)
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
            fasset.read(sv.TCP.buffer, BUFFER_SIZE);
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
    n = read(sv.TCP.fd, sv.TCP.buffer, BUFFER_SIZE);
    if(n <= 0) {
        MSG("Something went wrong.")
        if (n == -1) STATUS("Could not receive open reply.")
        else STATUS("Open reply is empty.")
        return -1;
    }

    // Retirar o \n no final e colocar \0
    sv.TCP.buffer[n-1] = '\0';

    STATUS_WA("Open reply received: %s", sv.TCP.buffer);

    istringstream reply(string(sv.TCP.buffer));
    
    string opcode;
    if (!(reply >> opcode)) {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply: no opcode.")
        return -1;
    }
    
    if (opcode == "ROA") {
        string status;

        if (!(reply >> status)) {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply: no status.")
            return -1;
        }

        if (status == "OK") {
            string AID;

            if (!(reply >> AID)) {
                MSG("Something went wrong.")
                STATUS("Can't comprehend server's reply: no AID.")
                return -1;
            }
            
            for (char c : AID) {
                if (!isdigit(c)) {
                    MSG("AID is not numeric.")
                    return -1;
                }
            }
            
            if (AID.length() != 3) {
                MSG("AID should be 3 digits long.")
                return -1;
            }

            if (!cmdstream.eof()) {
                MSG("Too many arguments.")
                return -1;
            }

            MSG("Auction was successfully started.")
        }
        else if (status == "NOK") MSG("Auction can't be started.")
        else if (status == "NLG") {
            MSG("You are not logged in.")
            STATUS("NOT SUPPOSED TO HAPPEN!!!")
        }
        else {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply.")
            return -1;
        }
    }
    else if (opcode == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }

    return 0;
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

    if (!(is_valid_AID(AID))){
            MSG("AID needs to be 3 digits long.")
            return -1;
    }

    if (!cmdstream.eof()) {
        MSG("Too many arguments.")
        return -1;
    }
    string sbuff = "CLS " + sv.UID + " " + sv.pass + " " + AID + "\n";
    size_t n = write(sv.TCP.fd, sbuff.c_str(), sbuff.length());

    if (n != sbuff.length()) {
        MSG("Something went wrong.")
        STATUS("Could not send close request.")
        return -1;
    }
    STATUS("Close message sent.")

    //memset(sv.TCP.buffer,0,128);
    n = read(sv.TCP.fd, sv.TCP.buffer, BUFFER_SIZE);

    if (n <= 0){
        MSG("Something went wrong.")
        if (n == -1) STATUS("Could not receive close reply.")
        else STATUS("Close reply message is empty.")
        return -1;
    }

    sv.TCP.buffer[n-1] = '\0';

    STATUS_WA("Close reply received: %s", sv.TCP.buffer);

    istringstream reply(string(sv.TCP.buffer));

    string opcode;
    if (!(reply >> opcode)){
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply: no opcode.")
        return -1;
    }

    if ( opcode == "RCL"){
        string status;

        if (!(reply >> status)) {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply: no status.")
            return -1;
        }

        if (status == "OK") MSG("Close was successful.")
        else if (status == "NLG") MSG("You are not logged in.")
        else if (status == "EAU") MSG_WA("Auction %s does not exist.", AID.c_str())
        else if (status == "EOW") MSG_WA("You are not the owner of %s auction.",AID.c_str())
        else if (status == "END") MSG_WA("Auction %s has already finished.", AID.c_str())
        else {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply.")
            return -1;
        }
    }
    else if (opcode == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }
    

    return 0;

}

int cmd_myauctions(){
    if (sv.UID == NO_USER){
        MSG("You are not logged in.")
        return -1;
    }

    string sbuff = "LMA " + sv.UID + "\n";

    if(sendto(sv.UDP.fd, sbuff.c_str(), sbuff.length(), 0, sv.UDP.res->ai_addr, 
                sv.UDP.res->ai_addrlen) == -1) {
        MSG("Couldn't get your auctions.")
        STATUS("Could not send my auctions message")
        return -1;
    }
    STATUS("My auctions message sent.")

    size_t n = recvfrom(sv.UDP.fd,sv.UDP.buffer,BUFFER_SIZE,0,(struct sockaddr*) &sv.UDP.addr,&sv.UDP.addrlen);
    if(n<=0) {
        MSG("Couldn't get your auctions.")
        if (n == -1) STATUS("Could not receive my auction reply")
        else STATUS("My auction reply is empty.") 
        return -1;
    }

    sv.UDP.buffer[n-1] = '\0';

    STATUS_WA("My auctions reply received: %s", sv.UDP.buffer)
    istringstream reply(string(sv.UDP.buffer));


    string opcode;
    if (!(reply >> opcode)){
        MSG("Something went wrong.")
        STATUS("Myauctions reply is empty")
        return -1;
    }

    if (opcode == "RMA") {
        string status;
        if (!(reply >> status)){
            MSG("Something went wrong.")
            STATUS("Myauctions reply status is missing.")
            return -1; 
        }
        if (status == "OK"){
            string AID, state;
            while ((!(reply.eof()))){
                if (!(reply >> AID)){
                    MSG("Something went wrong.")
                    STATUS("Couldn't extract AID.")
                    return -1;
                }
                if (!(reply>>state)){
                    MSG("Something went wrong.")
                    STATUS("Auction doesn't have a state.")
                    return -1;
                }
                if (!(is_valid_AID(AID) && is_valid_state(state))){
                    MSG("Something went wrong.")
                    STATUS("Auction reply is wrongly formatted.")
                    return -1;
                }
                MSG_WA("%s %s", AID.c_str(), state.c_str())
            }
        }
        else if (status == "NOK") MSG("You have no ongoing auctions.")
        else if (status == "NLG") MSG("You are not logged in.")
        else {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply.")
            return -1;
        }
    }
    else if (opcode == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }

    return 0;

}

int cmd_mybids(){

    if (sv.UID == NO_USER){
        MSG("You are not logged in.")
        return -1;
    }

    string sbuff = "LMB " + sv.UID + "\n";

    if(sendto(sv.UDP.fd, sbuff.c_str(), sbuff.length(), 0, sv.UDP.res->ai_addr, 
                sv.UDP.res->ai_addrlen) == -1) {
        MSG("Couldn't get your bids.")
        STATUS("Could not send my bids message")
        return -1;
    }
    STATUS("My bids message sent.")

    size_t n = recvfrom(sv.UDP.fd,sv.UDP.buffer,BUFFER_SIZE,0,(struct sockaddr*) &sv.UDP.addr,&sv.UDP.addrlen);
    if(n<=0) {
        MSG("Couldn't get your bids.")
        if (n == -1) STATUS("Could not receive my bids reply")
        else STATUS("My bids reply is empty.")
        return -1;
    }

    sv.UDP.buffer[n-1] = '\0';

    STATUS_WA("My bids reply received: %s", sv.UDP.buffer)

    istringstream reply(string(sv.UDP.buffer));

    string opcode;
    if (!(reply >> opcode)){
        MSG("Something went wrong.")
        STATUS("My bids reply is empty")
        return -1;
    }

    if (opcode == "RMB"){
        string status;
        if (!(reply >> status)){
            MSG("Something went wrong.")
            STATUS("Mybids reply status is missing.")
            return -1;
        }

        if (status == "OK"){
            string AID, state;
            while ((!(reply.eof()))){
                if (!(reply >> AID)){
                    MSG("Something went wrong.")
                    STATUS("Couldn't extract AID.")
                    return -1;
                }
                if (!(reply>>state)){
                    MSG("Something went wrong.")
                    STATUS("Auction doesn't have a state.")
                    return -1;
                }
                if (!(is_valid_AID(AID) && is_valid_state(state))){
                    MSG("Something went wrong.")
                    STATUS("Auction reply is wrongly formatted")
                    return -1;
                }
                MSG_WA("%s %s", AID.c_str(), state.c_str())
            }
        }
        else if (status == "NOK") MSG("You have no ongoing bids.")
        else if (status == "NLG") MSG("You are not logged in.")
        else {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply.")
            return -1;
        }
    }
    else if (opcode == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }

    return 0;

}

int cmd_list(){
    if (sv.UID == NO_USER){
        MSG("You are not logged in.")
        return -1;
    }

    string sbuff = "LST\n";

    if(sendto(sv.UDP.fd, sbuff.c_str(), sbuff.length(), 0, sv.UDP.res->ai_addr, 
                sv.UDP.res->ai_addrlen) == -1) {
        MSG("Couldn't auction listing.")
        STATUS("Could not send list message")
        return -1;
    }
    STATUS("List message sent.")

    size_t n = recvfrom(sv.UDP.fd,sv.UDP.buffer,BUFFER_SIZE,0,(struct sockaddr*) &sv.UDP.addr,&sv.UDP.addrlen);
    if(n<=0) {
        MSG("Couldn't get auction listing.")
        if (n == -1) STATUS("Could not receive list reply")
        else STATUS("List reply is empty.")
        return -1;
    }

    sv.UDP.buffer[n-1] = '\0';

    STATUS_WA("List reply received: %s", sv.UDP.buffer)

    istringstream reply(string(sv.UDP.buffer));

    string opcode;
    if (!(reply >> opcode)){
        MSG("Something went wrong")
        STATUS("List reply is empty")
        return -1;
    }

    if (opcode == "RLS"){
        string status;
        if (!(reply >> status)){
            MSG("Something went wrong.")
            STATUS("Mybids reply status is missing.")
            return -1;
        }
        if (status == "OK"){
            string AID, state;
            while ((!(reply.eof()))){
                if (!(reply >> AID)){
                    MSG("Something went wrong.")
                    STATUS("Couldn't extract AID.")
                    return -1;
                }
                if (!(reply>>state)){
                    MSG("Something went wrong.")
                    STATUS("Auction doesn't have a state.")
                    return -1;
                }
                if (!(is_valid_AID(AID) && is_valid_state(state))){
                    MSG("Something went wrong.")
                    STATUS("Auction reply is wrongly formatted")
                    return -1;
                }
                MSG_WA("%s %s", AID.c_str(), state.c_str())
            }
        }
        else if (status == "NOK") MSG("There is no ongoing auctions.")
        else {
                MSG("Something went wrong.")
                STATUS("Can't comprehend server's reply.")
                return -1;
            }
    }
    else if (opcode == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }

    return 0;

}

int cmd_show_asset(istringstream &cmdstream){
    string AID;

    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        return -1;
    }

    if (!(cmdstream >> AID) ){
        MSG("AID not specified.")
        return -1;
    }

    if (!(is_valid_AID(AID))){
            MSG("AID needs to be 3 digits long.")
            return -1;
    }

    if (!cmdstream.eof()) {
        MSG("Too many arguments.")
        return -1;
    }

    string sbuff = "SAS " +  AID + "\n";
    size_t n = write(sv.TCP.fd, sbuff.c_str(), sbuff.length());

    if (n != sbuff.length()) {
        MSG("Something went wrong.")
        STATUS("Could not send show asset request.")
        return -1;
    }
    STATUS("Show asset message sent.")

    sbuff.clear();

    while((n = read(sv.TCP.fd, sv.TCP.buffer, BUFFER_SIZE)) > 0) {
        sv.TCP.buffer[n] = '\0';
        sbuff += string(sv.TCP.buffer);
    }

    if (n == -1) {
            MSG("Something went wrong.")
            if (n == -1) STATUS("Could not receive show asset reply.")
            else STATUS("Show asset reply is empty.")
            return -1;
    }

    if (sbuff.length() >= 1)
        sbuff = sbuff.substr(0, sbuff.length()-1);
    else {
        MSG("Something went wrong.")
        STATUS("Show asset reply is empty.")
        return -1;
    }
    
    STATUS_WA("Show asset reply received: %s", sbuff.c_str());
    
    istringstream reply(sbuff);

    string opcode;
    if (!(reply >> opcode)){
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply: no opcode.")
        return -1;
    }

    if (opcode == "RSA"){
        string status;

        if (!(reply >> status)) {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply: no status.")
            return -1;
        }

        if (status=="OK"){
            string fname,fdata;
            int fsize;

            if (!(reply >> fname)){
                MSG("Something went wrong.")
                STATUS("Can't comprehend server's reply: no fname.")
                return -1;
            }

            if (!is_valid_fname(fname)){
                MSG("Something went wrong.")
                STATUS("Not valid fname.")
                return -1;
            }

            if (!(reply >> fsize)){
                MSG("Something went wrong.")
                STATUS("Can't comprehend server's reply: no fsize or fsize is not an int")
                return -1;
            }

            if (!(is_valid_fsize(fsize))){
                MSG("Something went wrong.")
                STATUS("Not valid fsize.")
                return -1;
            }

            if (!(reply >> fdata)){
                MSG("Something went wrong.")
                STATUS("Can't comprehend server's reply: no fdata.")
                return -1;
            }

            ofstream outputFile(fname);

            if (!outputFile.is_open()) {
                MSG("Something went wrong.")
                STATUS("Error opening the file.")
                return 1;
            }

            if(!(outputFile << fdata)){
                MSG("Something went wrong.")
                STATUS("Error writing to file.")
            }

            outputFile.close();
        }
        else if (status=="NOK") MSG("There is no file or some other problem.")
        else {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply.")
            return -1;
        }
    }
    else if (opcode == "ERR") MSG("Wrong syntax")
    else{
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }

    MSG("Asset file was successfully saved.")
    
    return 0;
}

int cmd_bid(istringstream &cmdstream){
    string AID;
    int value;


    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        return -1;
    }

    if (!(cmdstream>>AID)){
        MSG("AID is not specified.")
        return -1;
    }

    if (!is_valid_AID(AID)){
        MSG_WA("%s is not valid",AID.c_str())
        return -1;
    }

    if (!(cmdstream>>value)){
        MSG("Value is not a valid integer.")
        return -1;
    }

    if (value > MAX_BID_VALUE) {
        MSG("Value is too big (> 999999).")
        return -1;
    }

    if (!cmdstream.eof()) {
        MSG("Too many arguments.")
        return -1;
    }

    string sbuff = "BID " + sv.UID + " " + sv.pass + " " + AID + " " + 
                    to_string(value) + "\n";

    size_t n = write(sv.TCP.fd, sbuff.c_str(), sbuff.length());
    if (n != sbuff.length()) {
        MSG("Something went wrong.")
        STATUS("Could not send bid request")
        return -1;
    }

    STATUS("Bid message sent.")

    n = read(sv.TCP.fd, sv.TCP.buffer, BUFFER_SIZE);

    if (n <= 0){
        MSG("Something went wrong.")
        if (n == -1) STATUS("Could not receive bid reply.")
        else STATUS("Bid reply message is empty.")
        return -1;
    }

    sv.TCP.buffer[n-1] = '\0';

    STATUS_WA("Close reply received: %s", sv.TCP.buffer);

    istringstream reply(string(sv.TCP.buffer));

    string opcode;
    if (!(reply >> opcode)){
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply: no opcode.")
        return -1;
    }

    if ( opcode == "RBD"){
        string status;

        if (!(reply >> status)) {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply: no status.")
            return -1;
        }

        if (status == "NOK") MSG("Auction is not active.")
        else if (status == "NLG") MSG("You are not logged in.")
        else if (status == "ACC") MSG("Your bid was accepted.")
        else if (status == "REF") MSG("Your bid is smaller than the current bid value.")
        else if (status == "ILG") MSG("You can't bid on your own auction.")

    }
    else if (opcode == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }

    return 0;
    
}

int cmd_show_records(istringstream &cmdstream){
    string AID;

    if (sv.UID == NO_USER) {
        MSG("You are not logged in.")
        return -1;
    }

    if (!(cmdstream>>AID)){
        MSG("AID is not specified.")
        return -1;
    }

    if (!is_valid_AID(AID)){
        MSG_WA("%s is not valid",AID.c_str())
        return -1;
    }

    if (!cmdstream.eof()) {
        MSG("Too many arguments.")
        return -1;
    }

    string sbuff = "SRC " + AID + "\n";

    if(sendto(sv.UDP.fd, sbuff.c_str(), sbuff.length(), 0, sv.UDP.res->ai_addr, 
                sv.UDP.res->ai_addrlen) == -1) {
        MSG("Something went wrong.")
        STATUS("Could not send show records message")
        return -1;
    }
    STATUS("Show records message sent.")

    size_t n = recvfrom(sv.UDP.fd,sv.UDP.buffer,BUFFER_SIZE,0,(struct sockaddr*) &sv.UDP.addr,&sv.UDP.addrlen);
    if(n <= 0) {
        MSG("Something went wrong.")
        if (n == -1) STATUS("Could not receive show records reply.")
        else STATUS("Show records reply is empty.")
        return -1;
    }

    sv.UDP.buffer[n-1]='\0';

    STATUS_WA("Show records reply received: %s", sv.UDP.buffer);

    istringstream reply(string(sv.UDP.buffer));

    string opcode;
    if (!(reply >> opcode)) {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply: no opcode.")
        return -1;
    }

    if (opcode == "RRC"){
        string status;
        if (!(reply >> status)) {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply: no status.")
            return -1;
        }
        if (status == "OK"){
            string host_UID,auction_name,asset_fname,start_date_time;
            int start_value, timeactive;

            if (!(reply >> host_UID)) {
                MSG("Something went wrong.")
                STATUS("Can't comprehend server's reply: no host UID.")
                return -1;
            }

            if (host_UID.length() != UID_LEN){
                MSG("Something went wrong.")
                STATUS("Host UID is not a valid UID.")
                return -1;
            }

            if (!(reply >> auction_name)) {
                MSG("Something went wrong.")
                STATUS("Can't comprehend server's reply: no auction name.")
                return -1;
            }

            if (auction_name.length() > NAME_MAX_LEN){
                MSG("Something went wrong.")
                STATUS("Auction name is not valid.")
                return -1;
            }

            if (!(reply >> asset_fname)) {
                MSG("Something went wrong.")
                STATUS("Can't comprehend server's reply: no asset file name.")
                return -1;
            }

            if (!is_valid_fname(asset_fname)){
                MSG("Something went wrong.")
                STATUS("Auction name is not valid.")
                return -1;
            }

            if (!(reply >> start_value)) {
                MSG("Something went wrong.")
                STATUS("Can't comprehend server's reply: no start value or start \\
                        value is not an int.")
                return -1;
            }

            if (start_value > MAX_START_VALUE){
                MSG("Something went wrong.")
                STATUS("Auction name is not valid.")
                return -1;
            }
            
            if (!(reply >> start_date_time)){
                MSG("Something went wrong.")
                STATUS("Can't comprehend server's reply: no start date and time")
                return -1;
            }

            if (!is_valid_date_time(start_date_time)){
                MSG("Something went wrong.")
                STATUS("Start date_time is not valid.")
                return -1;
            }

            if (!(reply >> timeactive)) {
                MSG("Something went wrong.")
                STATUS("Can't comprehend server's reply: no start value or start \\
                        value is not an int.")
                return -1;
            }

            

            

            if (!cmdstream.eof()) {
                MSG("Too many arguments.")
                return -1;
            }

            
            
        }
        else if (status == "NOK") MSG_WA("Auction %s does not exist.", AID.c_str())
        else {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply.")
            return -1;
        }
    }
    else if (opcode == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }

    return 0;
    
}

int processCommand(string full_cmd) {
    
    istringstream cmdstream(full_cmd);
    string word, cmd;
    
    if (!(cmdstream >> cmd)){
        STATUS("invalid command")
        return -1;
    }

    if (cmd=="login") {
        start_udp();

        if (cmd_login(cmdstream) == -1){
            STATUS("invalid login.")
        }

        end_udp();
    }
    else if (cmd == "logout") {
        start_udp();

        if (!cmdstream.eof()) {
            MSG("Too many arguments.")
            return -1;
        }
        if (cmd_logout()==-1){
            STATUS("invalid logout")
        }

        end_udp();
    }

    else if (cmd == "unregister") {
        start_udp();
        
        if (!cmdstream.eof()) {
            MSG("Too many arguments.")
            return -1;
        }
        if (cmd_unregister()==-1){
            STATUS("invalid unregister")
        }

        end_udp();
    }

    else if (cmd == "exit") {
        if (!cmdstream.eof()) {
            MSG("Too many arguments.")
            return -1;
        }
        if (cmd_exit()==-1){
            STATUS("invalid exit")
        }
    }

    else if (cmd == "open") {
        start_tcp();
        
        if (cmd_open(cmdstream)==-1){
            STATUS("invalid open")
        }

        end_tcp();
    }

    else if (cmd == "close") {
        start_tcp();

        if (cmd_close(cmdstream)==-1){
            STATUS("invalid close")
        }

        end_tcp();
    }
    else if (cmd == "myauctions" || cmd == "ma") {
        start_udp();

        if (!cmdstream.eof()) {
            MSG("Too many arguments.")
            return -1;
        }
        
        if (cmd_myauctions()==-1){
            STATUS("invalid myauctions")
            return -1;
        }

        end_udp();
    }
    else if (cmd == "mybids" || cmd == "mb") {
        start_udp();

        if (!cmdstream.eof()) {
            MSG("Too many arguments.")
            return -1;
        }
        if (cmd_mybids()==-1){
            STATUS("invalid mybids")
            return -1;
        }

        end_udp();
    }
    else if (cmd == "list" || cmd == "l") {
        start_udp();

        if (!cmdstream.eof()) {
            MSG("Too many arguments.")
            return -1;
        }
        if (cmd_list()==-1){
            STATUS("invalid list")
            return -1;
        }

        end_udp();
    }
    else if (cmd == "show_asset" || cmd == "sa") {
        start_tcp();

        if (cmd_show_asset(cmdstream)==-1){
            STATUS("invalid show_asset")
        }

        end_tcp();
    }
    else if (cmd == "bid" || cmd == "b") {
        start_tcp();

        if (cmd_bid(cmdstream) == -1) {
            STATUS("invalid bid")
        }

        end_tcp();
    }
    // else if (cmd == "show_records" || cmd == "sr") {
    //     cmd_show_records(cmdstream);
    // }
    else {
        MSG("Invalid command.")
    }

    
    return 0;
}
