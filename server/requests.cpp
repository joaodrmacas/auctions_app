#include "requests.hpp"

extern sys_var sv;


string req_login(istringstream &reqstream){

    string UID, req_pass;

    if (!(reqstream >> UID)){
        STATUS("Login request doesn't have UID")
        return "ERR\n";
    }

    if (!is_valid_UID(UID)){
        STATUS("UID is not correctly formatted.")
        return "ERR\n";
    }

    if (!(reqstream >> req_pass)){
        STATUS("Login request doesn't have password.")
        return "ERR\n";
    }

    if (!is_valid_pass(req_pass)) {
        STATUS("Password is not correctly formatted.")
        return "ERR\n";
    }

    if (!reqstream.eof()){
        STATUS("Login request format is incorrect.")
        return "ERR\n";
    }

    //Começar a checkar o request.

    string reply = "RLI ";

    fs::path user_dir = fs::path(USERS_DIR_PATH).append(UID);
    fs::path pass_path = fs::path(USERS_DIR_PATH).append(UID).append(UID + "_pass.txt");
    fs::path login_path = fs::path(USERS_DIR_PATH).append(UID).append(UID + "_login.txt");

    //Registar user;
    //O registo é feito se não existir a diretoria ou se existir mas não tem password.
    if(!(fs::exists(user_dir)) || !(fs::exists(pass_path))){
        ofstream pass_stream(pass_path);
        ofstream login_stream(login_path);

        if (!login_stream.is_open()){
            STATUS("Couldn't create a login file.")
            return "BAD\n";
        }

        login_stream.close();

        if (!(pass_stream.is_open())){
            STATUS("Couldn't create password file.")
            pass_stream.close();
            return "BAD\n";
        }

        if (!(pass_stream << req_pass)){
            STATUS("Error writing to password file.")
            pass_stream.close();
            return "BAD\n";
        }

        pass_stream.close();

        reply += "REG\n";
    }
    else {
        ifstream pass_stream(pass_path);

        if (!(pass_stream.is_open())){
            STATUS("Couldn't open password file.")
            pass_stream.close();
            return "BAD\n";
        }

        string password;
        getline(pass_stream,password); // reply como a pass é alfanumerica, podemos so apanhar a primeira linha?

        if (pass_stream.fail()){
            STATUS("Error reading password file.")
            pass_stream.close();
            return "BAD\n";
        }

        pass_stream.close();

        //se o user tiver registado. Checkar se existe um login file. Se não existir criar um e se existir cagar;

        if (req_pass == password){

            //o user nao ta logged in;
            if (!(fs::exists(login_path))){

                ofstream login_stream(login_path);

                if (!login_stream.is_open()){
                    STATUS("Couldn't create a login file.")
                    login_stream.close();
                    return "BAD\n";
                }

                login_stream.close();

            }

            reply += "OK\n";
            
        }
        else reply+="NOK\n";
    }

    //Enviar resposta;
    STATUS_WA("Login message: %s",reply.c_str())
    
    return reply;
}

string req_logout(istringstream &reqstream){
    string UID, req_pass;

    if (!(reqstream >> UID)){
        STATUS("Logout request doesn't have UID")
        return "ERR\n";
    }

    if (!is_valid_UID(UID)){
        STATUS("UID is not correctly formatted.")
        return "ERR\n";
    }

    if (!(reqstream >> req_pass)){
        STATUS("Logout request doesn't have password.")
        return "ERR\n";
    }

    if (!is_valid_pass(req_pass)) {
        STATUS("Password is not correctly formatted.")
        return "ERR\n";
    }

    if (!reqstream.eof()){
        STATUS("Logout request format is incorrect.")
        return "ERR\n";
    }

    string reply = "RLO ";

    fs::path user_dir = fs::path(USERS_DIR_PATH).append(UID);
    fs::path pass_path = fs::path(USERS_DIR_PATH).append(UID).append(UID + "_pass.txt");
    fs::path login_path = fs::path(USERS_DIR_PATH).append(UID).append(UID + "_login.txt");

    //O logout é feito se a diretoria existe,se o login existe, e se a password está correta;
    //Reply no ok, se a password nao existir damos print do que??
    //OK se a diretoria existe, se o login existe, a password existe e está correta;
    //NOK se a diretoria existe, (se o login não existe || login existe e a password está errada)
    //UNR se a diretoria não existe || se a password não existe;
    if(fs::exists(user_dir)){

        //Reply
        //Não estamos a tratar do caso em pass não existe e login existe (o que fazer?)

        //UNR
        if (!(fs::exists(pass_path))) reply += "UNR\n"; 
        //OK ou NOK
        else if (fs::exists(login_path)){

            ifstream pass_stream(pass_path);

            if (!(pass_stream.is_open())){
                STATUS("Couldn't open password file.")
                pass_stream.close();
                return "BAD\n";
            }

            string password;
            getline(pass_stream,password); // reply como a pass é alfanumerica, podemos so apanhar a primeira linha?

            if (pass_stream.fail()){
                STATUS("Error reading password file.")
                pass_stream.close();
                return "BAD\n";
            }

            pass_stream.close();

            //faz logout;
            if (req_pass == password){
                
                //apaga login;
                try {fs::remove(login_path);}
                catch (const fs::filesystem_error& e) {
                    STATUS("Error deleting login file.")
                    return "BAD\n";
                }

                reply += "OK\n";

            }
            else reply += "NOK\n";  //Login existe mas pass errada
        }
        else reply += "NOK\n";
    }
    else reply += "UNR\n";

    //mandar mensagem

    STATUS_WA("Logout message: %s",reply.c_str())

    return reply;
}

string req_unregister(istringstream &reqstream){
    string UID, req_pass;

    if (!(reqstream >> UID)){
        STATUS("Unregister request doesn't have UID")
        return "ERR\n";
    }

    if (!is_valid_UID(UID)){
        STATUS("UID is not correctly formatted.")
        return "ERR\n";
    }

    if (!(reqstream >> req_pass)){
        STATUS("Unregister request doesn't have password.")
        return "ERR\n";
    }

    if (!is_valid_pass(req_pass)) {
        STATUS("Password is not correctly formatted.")
        return "ERR\n";
    }

    if (!reqstream.eof()){
        STATUS("Unregister request format is incorrect.")
        return "ERR\n";
    }

    string reply = "RUR ";

    fs::path user_dir = fs::path(USERS_DIR_PATH).append(UID);
    fs::path pass_path = fs::path(USERS_DIR_PATH).append(UID).append(UID + "_pass.txt");
    fs::path login_path = fs::path(USERS_DIR_PATH).append(UID).append(UID + "_login.txt");

    //OK - user_dir existe, login existe e pass está correta
    //NOK - user_dir existe, (se o login não existe || login existe e password está errada) 
    //UNR - user_dir não existe ou password não existe.
    if(fs::exists(user_dir)){

        //Reply
        //Não estamos a tratar do caso em pass não existe e login existe (o que fazer?)
        //ou seja nao tou a checkar se a passe e o login existem em simultaneo
        //assumo que caso o login exista entao a passe tambem.

        //UNR
        if (!(fs::exists(pass_path))) reply += "UNR\n"; 
        //OK ou NOK
        else if (fs::exists(login_path)){

            ifstream pass_stream(pass_path);

            if (!(pass_stream.is_open())){
                STATUS("Couldn't open password file.")
                pass_stream.close();
                return "BAD\n";
            }

            string password;
            getline(pass_stream,password); // reply como a pass é alfanumerica, podemos so apanhar a primeira linha?

            if (pass_stream.fail()){
                STATUS("Error reading password file.")
                pass_stream.close();
                return "BAD\n";
            }

            pass_stream.close();

            //faz logout;
            if (req_pass == password){

                try {
                    fs::remove(login_path);
                    fs::remove(pass_path);
                }
                catch (const fs::filesystem_error& e) {
                    STATUS("Error deleting file.")
                    return "BAD\n";
                }
                reply += "OK\n";

            }
            else reply += "NOK\n";  //Login existe mas pass errada
        }
        else reply += "NOK\n";
    }
    else reply += "UNR\n"; //o user não está registado 
    
    STATUS_WA("Unregister message: %s",reply.c_str())

    //Enviar mensagem

    return reply;
}

string req_myauctions(istringstream &reqstream){

    string UID;

    if (!(reqstream >> UID)){
        STATUS("My auctions request doesn't have UID")
        return "ERR\n";
    }

    if (!is_valid_UID(UID)){
        STATUS("UID is not correctly formatted.")
        return "ERR\n";
    }

    if (!reqstream.eof()){
        STATUS("My auctions request format is incorrect.")
        return "ERR\n";
    }

    string reply = "RMA ";

    fs::path user_dir = fs::path(USERS_DIR_PATH).append(UID);
    fs::path login_path = fs::path(USERS_DIR_PATH).append(UID).append((UID + "_login.txt"));
    fs::path uid_hosted_dir = fs::path(USERS_DIR_PATH).append(UID).append(HOSTED_DIR_PATH);

    if (fs::exists(user_dir)){
        //Checkar login primeiro
        if (!(fs::exists(login_path))) reply += "NLG\n";
        else if (!(fs::exists(uid_hosted_dir)) || fs::is_empty(uid_hosted_dir)) {
            reply += "NOK\n";
        }
        else {
            reply += "OK";
            try {
                for (const auto& entry : fs::directory_iterator(uid_hosted_dir)) {
                    if (fs::is_regular_file(entry.path())) { // Reply este if é preciso?
                        string AID;
                        AID = entry.path().stem().string();

                        if (!is_valid_AID(AID)){
                            STATUS("Auction file name is not a valid AID.")
                            return "BAD\n";
                        }

                        reply += " " + AID;


                        fs::path curr_auction_dir = fs::path(AUCTIONS_DIR_PATH).append(AID);
                        fs::path end_auction_file = fs::path(AUCTIONS_DIR_PATH).append(AID).append("END_" + AID + ".txt");
                        
                        if (!(fs::exists(end_auction_file))){
                            reply += " 1";
                        }
                        else reply += " 0";
                    }
                }
                reply += "\n";
            } catch (const filesystem::filesystem_error& e) {
                STATUS("Error accessing directory")
                return "BAD\n";
            }
        }
    }
    //Reply o que acontece quando não existe a pasta desse user?
    //consideramos como se não tivesse logged in? Pq so o proprio user pode pedir este
    else reply += "NLG\n";
    
    STATUS_WA("My auctions server reply: %s", reply)

    return reply;
}

string req_mybids(istringstream &reqstream){
    string UID;

    if (!(reqstream >> UID)){
        STATUS("My auctions request doesn't have UID")
        return "ERR\n";
    }

    if (!is_valid_UID(UID)){
        STATUS("UID is not correctly formatted.")
        return "ERR\n";
    }

    if (!reqstream.eof()){
        STATUS("My auctions request format is incorrect.")
        return "ERR\n";
    }

    string reply = "RMB ";

    fs::path user_dir = fs::path(USERS_DIR_PATH).append(UID);
    fs::path login_path = fs::path(USERS_DIR_PATH).append(UID).append(UID + "_login.txt");
    fs::path uid_bidded_dir = fs::path(USERS_DIR_PATH).append(UID).append(BIDDED_DIR_PATH);

    if (fs::exists(user_dir)){
        //checkar login
        if (!(fs::exists(login_path))) reply += "NLG\n";
        else if (!(fs::exists(uid_bidded_dir)) || fs::is_empty(uid_bidded_dir)){
            reply += "NOK\n";
        }
        else{
            reply += "OK";
            try {
                for (const auto& entry : fs::directory_iterator(uid_bidded_dir)) {
                    if (fs::is_regular_file(entry.path())) { // Reply este if é preciso?
                        string AID;
                        AID = entry.path().stem().string();

                        if (!is_valid_AID(AID)){
                            STATUS("Auction file name is not a valid AID.")
                            return "BAD\n";
                        }

                        reply += " " + AID;

                        fs::path curr_auction_dir = fs::path(AUCTIONS_DIR_PATH).append(AID);
                        fs::path end_auction_file = fs::path(AUCTIONS_DIR_PATH).append(AID).append("END_" + AID + ".txt");
                        
                        if (!(fs::exists(end_auction_file))){
                            reply += " 1";
                        }
                        else reply += " 0";
                    }
                }
                reply += "\n";
            } catch (const filesystem::filesystem_error& e) {
                STATUS("Error accessing directory")
                return "BAD\n";
            }
        }
    }
    else reply += "NLG\n";

    STATUS_WA("My bids server reply: %s", reply)

    return reply;
}

string req_list(){

    string reply = "RLS ";

    fs::path auctions_dir = fs::path(AUCTIONS_DIR_PATH);

    if (!fs::exists(auctions_dir)) {
        STATUS("AUCTIONS directory does not exist")
        return "BAD\n";
    } //a pasta auction assume se que ja está criada antes de correr
    else if (fs::is_empty(auctions_dir)) reply = "NOK\n";
    else {
        reply += "OK";
            try {
                for (const auto& entry : fs::directory_iterator(auctions_dir)) {
                    if (fs::is_regular_file(entry.path())) { // Reply este if é preciso?
                        string AID;
                        AID = entry.path().stem().string();

                        if (!is_valid_AID(AID)){
                                STATUS("Auction file name is not a valid AID.")
                                return "BAD\n";
                        }

                        reply += " " + AID;

                        fs::path curr_auction_dir = fs::path(AUCTIONS_DIR_PATH).append(AID);
                        fs::path end_auction_file = fs::path(AUCTIONS_DIR_PATH).append(AID).append("END_" + AID + ".txt");
                        
                        if (!(fs::exists(end_auction_file))){
                            reply += " 1";
                        }
                        else reply += " 0";
                    }
                }
                reply += "\n";
            } catch (const filesystem::filesystem_error& e) {
                STATUS("Error accessing directory")
                return "BAD\n";
            }
    }

    STATUS_WA("My bids server reply: %s", reply)

    return reply;
}


//falta atualizar o timeactive sempre que se usa isto;
string req_showrecord(istringstream &reqstream){
    string AID;
    if (!(reqstream >> AID)){
        STATUS("My auctions request doesn't have UID")
        return "ERR\n";
    }

    if (!is_valid_AID(AID)){
        STATUS("AID is not correctly formatted.")
        return "ERR\n";
    }

    if (!reqstream.eof()){
        STATUS("Show record request format is incorrect.")
        return "ERR\n";
    }

    string reply = "RRC ";

    fs::path auction_dir = fs::path(AUCTIONS_DIR_PATH).append(AID);
    fs::path start_auction_file = fs::path(AUCTIONS_DIR_PATH).append(AID).append("START_" + AID + ".txt");

    if (fs::exists(auction_dir)){
        if (fs::exists(start_auction_file)){
            reply += "OK ";

            ifstream start_stream(start_auction_file);

            if (!(start_stream.is_open())){
                STATUS("Couldn't open start auction file.")
                start_stream.close();
                return "BAD\n";
            }


            string host_uid,name,asset_fname,start_date;
            int start_value, timeactive, start_date_secs;

            if (!(reqstream >> host_uid)){
                STATUS("Start file doesn't have Host_UID")
                return "BAD\n";
            }

            if (!is_valid_UID(host_uid)){
                STATUS("Start file has a incorrectly formatted host_uid")
                return "BAD\n";
            }

            if (!(reqstream >> name)){
                STATUS("Start file doesn't have auction name")
                return "BAD\n";
            }

            if (!is_valid_auction_name(name)){
                STATUS("Start file has a incorrectly formatted auction name")
                return "BAD\n";
            }

            if (!(reqstream >> asset_fname)){
                STATUS("Start file doesn't have asset name")
                return "BAD\n";
            }

            if (!is_valid_fname(asset_fname)){
                STATUS("Start file has a incorrectly formatted asset name")
                return "BAD\n";
            }

            if (!(reqstream >> start_value)){
                STATUS("Start file doesn't have start value")
                return "BAD\n";
            }

            if (!is_valid_bid_value(start_value)){
                STATUS("Start file has a incorrectly formatted start value")
                return "BAD\n";
            }

            if (!(reqstream >> timeactive)){
                STATUS("Start file doesn't have time active")
                return "BAD\n";
            }

            if (!is_valid_timeactive(timeactive)){
                STATUS("Start file has a incorrectly formatted time active")
                return "BAD\n";
            }

            if (!(reqstream >> start_date)){
                STATUS("Start file doesn't have start date")
                return "BAD\n";
            }

            if (!is_valid_date_time(start_date)){
                STATUS("Start file has a incorrectly formatted start date")
                return "BAD\n";
            }

            if (!(reqstream >> start_date_secs)){
                STATUS("Start file doesn't have a start date in seconds")
                return "BAD\n";
            }

            start_stream.close();

            reply += host_uid + " " + name + " " + asset_fname + " " + to_string(start_value) \
            + " " + start_date + " " + to_string(timeactive);

            //checkar se alguem biddou;

            fs::path bids_directory = fs::path(AUCTIONS_DIR_PATH).append(AID).append(BIDS_DIR_PATH);

            if (fs::exists(bids_directory) && !(fs::is_empty(bids_directory))){
                try {

                    for (const auto& entry : fs::directory_iterator(bids_directory)) {
                        if (fs::is_regular_file(entry.path())) { // Reply este if é preciso
                            string bid_value_str, bid_filename = entry.path();

                            fs::path curr_bid_file = fs::path(AUCTIONS_DIR_PATH).append(AID).append(BIDS_DIR_PATH).append(bid_filename);
                            
                            ifstream bid_stream(curr_bid_file);

                            string uid,bid_date;
                            int bid_value, bid_date_secs;

                            if (!(bid_stream.is_open())){
                                STATUS("Couldn't open bid file.")
                                bid_stream.close();
                                return "BAD\n";
                            }

                            if (!(bid_stream >> uid)){
                                STATUS("Bid file doesn't have uid")
                                return "BAD\n";
                            }

                            if (!is_valid_UID(uid)){
                                STATUS("Bid file has a incorrectly formatted host_uid")
                                return "BAD\n";
                            }

                            if (!(bid_stream >> bid_value)){
                                STATUS("Bid file doesn't have start value")
                                return "BAD\n";
                            }

                            if (!is_valid_bid_value(bid_value)){
                                STATUS("Bid file has a incorrectly formatted start value")
                                return "BAD\n";
                            }

                            if (!(bid_stream >> bid_date)){
                                STATUS("Bid file doesn't have start date")
                                return "BAD\n";
                            }

                            if (!is_valid_date_time(bid_date)){
                                STATUS("Bid file has a incorrectly formatted date")
                                return "BAD\n";
                            }

                            if (!(bid_stream >> bid_date_secs)){
                                STATUS("Bid file doesn't have a date in seconds")
                                return "BAD\n";
                            }

                            reply += " B " + uid + " " + to_string(start_value) + " "
                                        + start_date + " " + to_string(bid_date_secs);

                        }
                    }
                } catch (const exception& e) {
                    STATUS("Error accessing directory");
                    return "BAD\n";
                }
                


            }
            
            //checkar se a auction ja acabou e se já escrever o [E ...]
            fs::path end_auction_file = fs::path(AUCTIONS_DIR_PATH).append(AID).append("END_" + AID + ".txt");
            if (fs::exists(end_auction_file)){
                ifstream end_stream(end_auction_file);

                if (!(end_stream.is_open())){
                    STATUS("Couldn't open end auction file.")
                    end_stream.close();
                    return "BAD\n";
                }

                string end_datetime, end_sec_time;

                if (!(end_stream >> end_datetime)){
                    STATUS("End file doesn't have a date")
                    return "BAD\n";
                }

                if (!is_valid_date_time(end_datetime)){
                    STATUS("End file date is incorrectly formatted")
                    return "BAD\n";
                }

                if (!(end_stream >> end_sec_time)){
                    STATUS("End file doesn't have a end date in seconds")
                    return "BAD\n";
                }
            }
            else reply += "\n";



        }
        else return "BAD\n"; //A auction existe mas nao tem start file
    }
    else{
        reply += "NOK\n";
    }

    return reply;
}

int handle_TCP_req(string req){
    istringstream reqstream(req);
    string request_type;
    string reply;

    if (!(reqstream>>request_type)){
        STATUS("Invalid command")
        return -1;
    }

    if (request_type == "OPA"){
        reply = req_open(reqstream);
        if (reply == ""){
            STATUS("Error during open.")
            return -1;
        }
    }

    else if (request_type == "CLS"){
        reply = req_close(reqstream);
        if (reply == ""){
            STATUS("Error during open.")
            return -1;
        }
    }

    else if (request_type == "SAS"){
        reply = req_showasset(reqstream);
        if (reply == ""){
            STATUS("Error during show asset.")
            return -1;
        }
    }
    
    else if (request_type == "BID"){
        reply = req_bid(reqstream);
        if (reply == ""){
            STATUS("Error during bid.")
            return -1;
        }
    }

    else reply = "ERR\n";

    //send reply

    return 0;
}

int handle_UDP_req(string req){

    istringstream reqstream(req);
    string request_type;
    string reply;

    if (!(reqstream>>request_type)){
        STATUS("Invalid command")
        return -1;
    }

    if (request_type == "LIN"){
        reply = req_login(reqstream);
        if (reply == ""){
            STATUS("Error during login.")
        }
    }

    else if (request_type == "LOU"){
        reply = req_logout(reqstream);
        if (reply == ""){
            STATUS("Error during logout.")
        }
    }

    else if (request_type == "UNR"){
        reply = req_unregister(reqstream);
        if (reply == ""){
            STATUS("Error during unregister.")
        }
    }

    else if (request_type == "LMA"){
        reply = req_myauctions(reqstream);
        if (reply == ""){
            STATUS("Error during my auctions.")
        }
    }

    else if (request_type == "LMB"){
        reply = req_mybids(reqstream);
        if (reply == ""){
            STATUS("Error during my auctions.")
        }
    }

    else if (request_type == "LST"){
        reply = req_list();
        if (reply == ""){
            STATUS("Error during list")
        }
    }

    else if (request_type == "SRC"){
        reply = req_showrecord(reqstream);
        if (reply == ""){
            STATUS("Error during show record.")
        }
    }

    else{
        reply = "ERR\n";
        STATUS("Invalid request.")
    }

    //mandar reply

    return 0;
}