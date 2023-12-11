#ifndef __SERVER_DEFS_HPP__
#define __SERVER_DEFS_HPP__

#include "requests.hpp"
#include "../definitions.hpp"
#include "../validations.hpp"

#define USERS_DIR_PATH ("USERS")
#define AUCTIONS_DIR_PATH ("AUCTIONS")
#define HOSTED_DIR_PATH ("HOSTED")
#define BIDDED_DIR_PATH ("BIDDED")

typedef struct __sys_var__ {
    string ASport=PORT;
    bool verbose = false;
    protocol UDP, TCP;
} sys_var;

#endif