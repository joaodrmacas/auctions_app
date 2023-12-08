#include "server_defs.hpp"
#include "requests.hpp"
#include "../validations.hpp"

extern sys_var sv;


string convert_file_to_txt(string file){
    return file + ".txt";
}


// string get_auctions_file_path(string AID){
//     return AUCTIONS_DIR_PATH + "/" + AID + ".txt"
// }

int req_login(istringstream &reqstream){

    string UID, pass;

    if (!(reqstream >> UID)){
        STATUS("Login request doesn't have UID")
        return -1;
    }

    if (!is_valid_UID(UID)){
        STATUS("UID is not correctly formatted.")
        return -1;
    }

    if (!(reqstream >> pass)){
        STATUS("Login request doesn't have password.")
        return -1;
    }

    if (!is_valid_pass(pass)) {
        STATUS("Password is not correctly formatted.")
        return -1;
    }

    if (!reqstream.eof()){
        STATUS("Login request format is incorrect.")
        return -1;
    }

    //Começar a checkar o request.

    string reply = "RLI ";

    fs::path user_dir = USERS_DIR_PATH / UID;
    fs::path pass_path = user_dir/ (pass + "_pass.txt");


    //Registar user;
    //O registo é feito se não existir a diretoria ou se existir mas não tem password.
    if(!(fs::exists(user_dir)) || !(fs::exist(pass_path))){
        fs::path login_path = user_dir/ (UID + "_login.txt");
        ofstream login_stream(login_path);
        ofstream pass_stream(pass_path);


        if (!login_stream.is_open()){
            STATUS("Couldn't create a login file.")
            login_stream.close();
            return -1;
        }

        login_stream.close()

        if (!(pass_stream.is_open())){
            STATUS("Couldn't create password file.")
            pass_stream.close();
            return -1;
        }

        if (!(pass_stream << pass)){
            STATUS("Error writing to password file.")
            pass_stream.close()
            return -1;
        }

        pass_stream.close()

        reply += "REG\n";
    }
    else {
        ifstream pass_stream(pass_path);

        if (!(pass_stream.is_open())){
            STATUS("Couldn't open password file.")
            pass_stream.close();
            return -1;
        }

        string password
        getline(pass_stream,password);

        if (pass_stream.fail()){
            STATUS("Error reading password file.")
            pass_stream.close();
            return -1;
        }

        reply += (pass == password) ? "OK\n" : "NOK\n";

    }

    //Enviar resposta;
    


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


    else{
        MSG("Invalid request.")
    }

    return 0;
}