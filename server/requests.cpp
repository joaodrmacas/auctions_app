#include "server_defs.hpp"
#include "requests.hpp"
#include "../validations.hpp"

extern sys_var sv;



int req_login(istringstream &reqstream){

    string UID, req_pass;

    if (!(reqstream >> UID)){
        STATUS("Login request doesn't have UID")
        return -1;
    }

    if (!is_valid_UID(UID)){
        STATUS("UID is not correctly formatted.")
        return -1;
    }

    if (!(reqstream >> req_pass)){
        STATUS("Login request doesn't have password.")
        return -1;
    }

    if (!is_valid_pass(req_pass)) {
        STATUS("Password is not correctly formatted.")
        return -1;
    }

    if (!reqstream.eof()){
        STATUS("Login request format is incorrect.")
        return -1;
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
            login_stream.close();
            return -1;
        }

        login_stream.close();

        if (!(pass_stream.is_open())){
            STATUS("Couldn't create password file.")
            pass_stream.close();
            return -1;
        }

        if (!(pass_stream << req_pass)){
            STATUS("Error writing to password file.")
            pass_stream.close();
            return -1;
        }

        pass_stream.close();

        reply += "REG\n";
    }
    else {
        ifstream pass_stream(pass_path);

        if (!(pass_stream.is_open())){
            STATUS("Couldn't open password file.")
            pass_stream.close();
            return -1;
        }

        string password;
        getline(pass_stream,password); // reply como a pass é alfanumerica, podemos so apanhar a primeira linha?

        if (pass_stream.fail()){
            STATUS("Error reading password file.")
            pass_stream.close();
            return -1;
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
                    return -1;
                }

                login_stream.close();

            }

            reply += "OK\n";
            
        }
        else reply+="NOK\n";
    }

    //Enviar resposta;
    STATUS_WA("Login message: %s",reply.c_str())
    
    return 0;

}

int req_logout(istringstream &reqstream){
    string UID, req_pass;

    if (!(reqstream >> UID)){
        STATUS("Logout request doesn't have UID")
        return -1;
    }

    if (!is_valid_UID(UID)){
        STATUS("UID is not correctly formatted.")
        return -1;
    }

    if (!(reqstream >> req_pass)){
        STATUS("Logout request doesn't have password.")
        return -1;
    }

    if (!is_valid_pass(req_pass)) {
        STATUS("Password is not correctly formatted.")
        return -1;
    }

    if (!reqstream.eof()){
        STATUS("Logout request format is incorrect.")
        return -1;
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
                return -1;
            }

            string password;
            getline(pass_stream,password); // reply como a pass é alfanumerica, podemos so apanhar a primeira linha?

            if (pass_stream.fail()){
                STATUS("Error reading password file.")
                pass_stream.close();
                return -1;
            }

            pass_stream.close();

            //faz logout;
            if (req_pass == password){
                
                //apaga login;
                try {fs::remove(login_path);}
                catch (const fs::filesystem_error& e) {
                    STATUS("Error deleting login file.")
                    return -1;
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

    return 0;
}

int req_unregister(istringstream &reqstream){
    string UID, req_pass;

    if (!(reqstream >> UID)){
        STATUS("Unregister request doesn't have UID")
        return -1;
    }

    if (!is_valid_UID(UID)){
        STATUS("UID is not correctly formatted.")
        return -1;
    }

    if (!(reqstream >> req_pass)){
        STATUS("Unregister request doesn't have password.")
        return -1;
    }

    if (!is_valid_pass(req_pass)) {
        STATUS("Password is not correctly formatted.")
        return -1;
    }

    if (!reqstream.eof()){
        STATUS("Unregister request format is incorrect.")
        return -1;
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
                return -1;
            }

            string password;
            getline(pass_stream,password); // reply como a pass é alfanumerica, podemos so apanhar a primeira linha?

            if (pass_stream.fail()){
                STATUS("Error reading password file.")
                pass_stream.close();
                return -1;
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
                    return -1;
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

    return 0;
}

int req_myauctions(istringstream &reqstream){

    string UID;

    if (!(reqstream >> UID)){
        STATUS("My auctions request doesn't have UID")
        return -1;
    }

    if (!is_valid_UID(UID)){
        STATUS("UID is not correctly formatted.")
        return -1;
    }

    if (!reqstream.eof()){
        STATUS("My auctions request format is incorrect.")
        return -1;
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
                    if (fs::is_regular_file(entry.path())) {
                        string AID;
                        AID = entry.path().stem().string();

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
            } catch (const std::filesystem::filesystem_error& e) {
                STATUS("Error accessing directory")
                return -1;
            }
        }
    }
    //Reply o que acontece quando não existe a pasta desse user?
    //consideramos como se não tivesse logged in? Pq so o proprio user pode pedir este
    else reply += "NLG\n";

    //Enviar mensagem;

    return 0;
}

int req_mybids(istringstream &reqstream){
    string UID;

    if (!(reqstream >> UID)){
        STATUS("My auctions request doesn't have UID")
        return -1;
    }

    if (!is_valid_UID(UID)){
        STATUS("UID is not correctly formatted.")
        return -1;
    }

    if (!reqstream.eof()){
        STATUS("My auctions request format is incorrect.")
        return -1;
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
                    if (fs::is_regular_file(entry.path())) {
                        string AID;
                        AID = entry.path().stem().string();

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
            } catch (const std::filesystem::filesystem_error& e) {
                STATUS("Error accessing directory")
                return -1;
            }
        }
    }
    else reply += "NLG\n";

    //Enviar mensagem;

    return 0;

}

int handleRequest(string req){

    istringstream reqstream(req);
    string request_type;

    if (!(reqstream>>request_type)){
        STATUS("Invalid command")
        return -1;
    }

    if (request_type == "LIN"){
        if (req_login(reqstream) == -1){
            STATUS("Error during login.")
        }
    }

    else if (request_type == "LOU"){
        if (req_logout(reqstream) == -1){
            STATUS("Error during logout.")
        }
    }

    else if (request_type == "UNR"){
        if (req_logout(reqstream) == -1){
            STATUS("Error during unregister.")
        }
    }

    else if (request_type == "LMA"){
        if (req_logout(reqstream) == -1){
            STATUS("Error during unregister.")
        }
    }

    else{
        //TO DO Mandar ERR
        STATUS("Invalid request.")
    }

    return 0;
}