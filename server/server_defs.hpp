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

#define NEXT_AID_NAME ("NEXT_AID.txt")
#define DB_LOCK_NAME ("DB.lock")
#define USERS_LOCK_NAME ("USERS.lock")
#define AUCTIONS_LOCK_NAME ("AUCTIONS.lock")
#define HOSTED_LOCK_NAME ("HOSTED.lock")
#define BIDDED_LOCK_NAME ("BIDDED.lock")

#define MAX_AID 999

typedef struct __sys_var__ {
    string ASport=PORT;
    bool verbose = false;
    protocol UDP, TCP;
    
    const std::filesystem::path next_AID_file = std::filesystem::path(DB_DIR_PATH).append(NEXT_AID_NAME);
    const std::filesystem::path db_dir = std::filesystem::path(DB_DIR_PATH);
    const std::filesystem::path users_dir = std::filesystem::path(DB_DIR_PATH).append(USERS_DIR_PATH);
    const std::filesystem::path auctions_dir = std::filesystem::path(DB_DIR_PATH).append(AUCTIONS_DIR_PATH);

    
    const std::filesystem::path db_lock = std::filesystem::path(DB_LOCK_NAME);
    const std::filesystem::path users_lock = std::filesystem::path(DB_DIR_PATH).append(USERS_LOCK_NAME);
    const std::filesystem::path auctions_lock = std::filesystem::path(DB_DIR_PATH).append(AUCTIONS_LOCK_NAME);

} sys_var;

#endif