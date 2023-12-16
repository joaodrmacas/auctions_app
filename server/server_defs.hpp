#ifndef __SERVER_DEFS_HPP__
#define __SERVER_DEFS_HPP__

#include "requests.hpp"
#include "../definitions.hpp"
#include "../validations.hpp"

#define DB_DIR_PATH ("server/DB")
#define USERS_DIR_PATH ("USERS")
#define AUCTIONS_DIR_PATH ("AUCTIONS")
#define HOSTED_DIR_PATH ("HOSTED")
#define BIDDED_DIR_PATH ("BIDDED")
#define BIDS_DIR_PATH ("BIDS")
#define ASSETS_DIR_PATH ("ASSETS")
#define ASSET_DIR_PATH ("ASSET")

#define USERS_LOCK_NAME ("USERS.lock")
#define AUCTIONS_LOCK_NAME ("AUCTIONS.lock")
#define HOSTED_LOCK_NAME ("HOSTED.lock")
#define BIDDED_LOCK_NAME ("BIDDED.lock")
#define AID_LOCK_NAME ("AID.lock")
#define UID_LOCK_NAME ("UID.lock")

#define MAX_AID 999

typedef struct __sys_var__ {
    string ASport=PORT;
    bool verbose = false;
    protocol UDP, TCP;
    int next_AID = 0;
} sys_var;

#endif