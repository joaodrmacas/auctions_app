#ifndef __REQUESTS__HPP__
#define __REQUESTS__HPP__

string req_login(istringstream &reqstream);
int req_logout(istringstream &reqstream);
int req_unregister(istringstream &reqstream);
int req_myauctions(istringstream &reqstream);
int req_mybids(istringstream &reqstream);
string req_list();
int handleRequest(string req);

#endif