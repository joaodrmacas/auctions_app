#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

int connect_tcp();
int close_tcp();

int cmd_login(istringstream &cmdstream);
int cmd_logout();
int cmd_unregister();
int cmd_exit();
int cmd_open(istringstream &cmdstream);
int cmd_close(istringstream &cmdstream);
int cmd_myauctions();
int processCommand(string full_cmd);


#endif