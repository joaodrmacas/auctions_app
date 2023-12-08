#ifndef __SERVER_DEFS_HPP__
#define __SERVER_DEFS_HPP__

#include <filesystem>
#include "../definitions.hpp"
#include "../validations.hpp"

#define USERS_DIR_PATH "USERS";
#define AUCTIONS_DIR_PATH "AUCTIONS"

namespace fs = filesystem;


typedef struct __sys_var__ {
    string ASport=PORT;
    bool verbose_mode = false;
    string users_path = USERS_DIR_PATH;
    string auctions_path = AUCTIONS_DIR_PATH;
    //protocol UDP, TCP;
} sys_var;

#endif