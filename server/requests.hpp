#ifndef __REQUESTS__HPP__
#define __REQUESTS__HPP__

#include "server_defs.hpp"
#include "../validations.hpp"

int end_tcp_socket();

string req_login(istringstream &reqstream);
string req_logout(istringstream &reqstream);
string req_unregister(istringstream &reqstream);
string req_myauctions(istringstream &reqstream);
string req_mybids(istringstream &reqstream);
string req_list();
int handleRequest(string req);

#endif