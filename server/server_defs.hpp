#ifndef __SERVER_DEFS_HPP__
#define __SERVER_DEFS_HPP__

#include <filesystem>
#include "../definitions.hpp"
#include "../validations.hpp"

#define USERS_DIR_PATH ("USERS")
#define AUCTIONS_DIR_PATH ("AUCTIONS")
#define HOSTED_DIR_PATH ("HOSTED")
#define BIDDED_DIR_PATH ("BIDDED")

namespace fs = filesystem;


typedef struct __sys_var__ {
    string ASport=PORT;
    bool verbose_mode = false;
    //protocol UDP, TCP;
} sys_var;

#endif