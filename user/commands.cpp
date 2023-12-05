#include "user_defs.hpp"
#include "commands.hpp"

extern sys_var sv;

int replaceEvenSpacesWithNewline(string& inputString,int isEvenSpace) {
    int spaces=0;
    for (size_t i = 0; i < inputString.length(); ++i) {
        if (inputString[i] == ' ') {
            spaces+=1;
            if (isEvenSpace) {
                inputString[i] = '\n';
            }
            isEvenSpace = !isEvenSpace;
        }
    }
    return spaces%2;
}

bool is_valid_AID(string AID){
     for (char c : AID) {
        if (!isdigit(c)) {
            return false;  // If any character is not a digit, the string is not numeric
        }
    }
    return AID.length() == 3;
}

bool is_valid_state(string AID){
    return AID.length()==1 && (AID[0] == '0' or AID[1] == '1') ;
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
            return -1;
        }
    }
    
    if (pass.length() != 8){
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

    if (name.length() > 10) {
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

    if (asset_fname.length() > FILE_NAME_MAX_SIZE) {
        MSG("Asset filename is too long.")
        return -1;
    }

    if ( !(cmdstream >> start_value) ){
        MSG("Start_value is not a valid integer.")
        return -1;
    }

    if (start_value > 999999) {
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

    if (timeactive > 99999) {
        MSG("Timeactive is too big (> 99999).")
        return -1;
    }

    if (timeactive < 0) {
        MSG("Timeactive can't be negative.")
        return -1;
    }

    if (cmdstream.eof()) {
        MSG("Too many arguments.")
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
    if (n == -1){
        MSG("Something went wrong.")
        STATUS("Could not receive open reply.")
        return -1;
    }

    if (n <= 3) {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }

    sv.TCP.buffer[n] = '\0';
    sbuff = string(sv.TCP.buffer);
    sbuff = sbuff.substr(0, sbuff.length()-1 );

    STATUS_WA("Open reply received: %s", sbuff.c_str());
    
    string opcode = sbuff.substr(0, 4);
    if (sbuff == "ERR") MSG("Wrong syntax.")
    else if (sbuff.length() <= 3 || opcode != "ROA " || sbuff.length() == 4) {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }

    string args = sbuff.substr(4);
    if (args == "NOK") MSG("Auction can't be started.")
    else if (args == "NGL") {
        MSG("Not logged in.")
        STATUS("NOT SUPPOSED TO HAPPEN!!!")
    }
    else if (args.length() == 6 && args.substr(0, 3) == "OK ") {
        string AID = args.substr(3);
        for (char c : AID) {
            if (!isdigit(c)) {
                MSG("AID is not numeric.")
                return -1;
            }
        }
    }
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
        if (n == -1) STATUS("Could not receive login reply.")
        else STATUS("Could not receive close reply.")
        return -1;
    }

    sv.UDP.buffer[n-1] = '\0';

    STATUS_WA("Close reply received: %s", sv.UDP.buffer);

    istringstream reply(string(sv.UDP.buffer));

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
        else if (status == "END") MSG_WA("Auction %s has already finised.", AID.c_str())
        else {
            MSG("Something went wrong.")
            STATUS("Can't comprehend server's reply.")
            return -1;
        }
    }
    else if (sbuff == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }
    

    return 0;

}

int cmd_myauctions(){
    string opcode, status;
    if (sv.UID != NO_USER){
        MSG("You are already logged in.")
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
        if (n == -1) STATUS("Could not receive login reply")
        else STATUS("Could not receive my auctions reply.")
        return -1;
    }

    sv.UDP.buffer[n-1] = '\0';

    STATUS_WA("Myauctions reply received: %s", sv.UDP.buffer)
    istringstream reply(string(sv.UDP.buffer));

    if (!(reply >> opcode)){
        MSG("Something went wrong.")
        STATUS("Myauctions reply is empty")
        return -1;
    }

    if (opcode != "RMA") {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }

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
                    STATUS("Auction reply is wrongly formatted")
                    return -1;
            }
            MSG_WA("%s %s", AID.c_str(), AID.c_str())
        }
    }
    else if (status == "NOK") MSG("You have no ongoing auctions.")
    else if (status == "NLG") MSG("You are not logged in.")
    else if (status == "ERR") MSG("Wrong syntax.")
    else {
        MSG("Something went wrong.")
        STATUS("Can't comprehend server's reply.")
        return -1;
    }
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
        if (!cmdstream.eof()) {
            MSG("Too many arguments.")
            return -1;
        }
        if (cmd_logout()==-1){
            STATUS("invalid logout")
        }
    }

    else if (cmd == "unregister") {
        if (!cmdstream.eof()) {
            MSG("Too many arguments.")
            return -1;
        }
        if (cmd_unregister()==-1){
            STATUS("invalid unregister")
        }
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
        // Connect to TCP
        if (connect(sv.TCP.fd, sv.TCP.res->ai_addr, sv.TCP.res->ai_addrlen) == -1) {
            MSG("Something went wrong.")
            STATUS("Could not connect [TCP]")
            return -1;
        }

        if (cmd_open(cmdstream)==-1){
            STATUS("invalid open")
        };

        // Close to TCP
        if (close(sv.TCP.fd) == -1) {
            MSG("Something went wrong.")
            STATUS("Could not close [TCP]")
            return -1;
        }
        
    }
    else if (cmd == "close") {
        if (cmd_close(cmdstream)==-1){
            STATUS("invalid close")
        }
    }
    else if (cmd == "myauctions" || cmd == "ma") {
        if (cmdstream.eof()) {
            MSG("Too many arguments.")
            return -1;
        }
        
        if (cmd_myauctions()==-1){
            STATUS("invalid myauctions")
            return -1;
        }
    }
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
        MSG("Invalid command.")
    }

    // while (iss >> word) {
    //         std::cout << "Word: " << word << std::endl;
    // }
    return 0;
}
