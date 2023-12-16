#ifndef __REQUESTS__HPP__
#define __REQUESTS__HPP__

#include "server_defs.hpp"
#include "../validations.hpp"

int end_tcp_socket();

//UDP
string req_login(istringstream &reqstream);
string req_logout(istringstream &reqstream);
string req_unregister(istringstream &reqstream);
string req_myauctions(istringstream &reqstream);
string req_mybids(istringstream &reqstream);
string req_list();
string req_showrecord(istringstream &reqstream);

//TCP
string req_open(istringstream &reqstream);
string req_close(istringstream &reqstream);
string req_bid(istringstream &reqstream);
void req_showasset(istringstream &reqstream);

int handle_TCP_req();
int handle_UDP_req(string req);

#endif