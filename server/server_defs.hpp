#ifndef __SERVER_DEFS_HPP__
#define __SERVER_DEFS_HPP__

#include "requests.hpp"
#include "../definitions.hpp"
#include "../validations.hpp"

#define USERS_DIR_PATH ("DB/USERS")
#define AUCTIONS_DIR_PATH ("DB/AUCTIONS")
#define HOSTED_DIR_PATH ("HOSTED")
#define BIDDED_DIR_PATH ("BIDDED")
#define BIDS_DIR_PATH ("BIDS")
#define ASSETS_DIR_PATH ("ASSETS")

typedef struct __sys_var__ {
    string ASport=PORT;
    bool verbose = false;
    protocol UDP, TCP;
    int next_AID = 0;
} sys_var;

#endif