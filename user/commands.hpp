#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

#include "user_defs.hpp"
#include "../validations.hpp"

void sig_handler(int signo);
int cmd_login(istringstream &cmdstream);
int cmd_logout();
int cmd_unregister();
int cmd_exit();
int cmd_open(istringstream &cmdstream);
int cmd_close(istringstream &cmdstream);
int cmd_myauctions();
int processCommand(string full_cmd);


#endif