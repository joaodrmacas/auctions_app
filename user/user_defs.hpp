#ifndef __USER_DEFS_HPP__
#define __USER_DEFS_HPP__

#include "../definitions.hpp"
#include "../validations.hpp"
#include "commands.hpp"

#define STATUS_ON (1)

#define STATUS(msg)                                                     \
    {if (STATUS_ON) {printf("[STATUS]: %s  [Line %d] [File %s]\n", msg, __LINE__, __FILE__);}}

#define STATUS_WA(format, ...)                                          \
    {if (STATUS_ON) {printf("[STATUS]: " format "  [Line %d] [File %s]\n", __VA_ARGS__, __LINE__, __FILE__);}}

// FIXME: NO NEED TO SEE THE LINE AND FILE
#define MSG(msg)                    \
    {printf("%s", msg); if (STATUS_ON) printf(" [Line %d] [File %s]", __LINE__, __FILE__); printf("\n");}               \

#define MSG_WA(format, ...)         \
    {printf(format, __VA_ARGS__ ); if (STATUS_ON) printf(" [Line %d] [File %s]", __LINE__, __FILE__); printf("\n");} \

#define PUBLIC_PORT ("58011")
#define PUBLIC_IP ("tejo.tecnico.ulisboa.pt")
#define MY_IP ("localhost")

#define NO_USER ("")
#define NO_PASS ("")

#define ASSETS_DIR ("user/assets/")

typedef struct __sys_var__ {
    string ASIP=PUBLIC_IP, ASport=PUBLIC_PORT;
    protocol UDP, TCP;
    string UID = NO_USER;
    string pass = NO_PASS;
    int to_exit = 0;
} sys_var;

#endif
