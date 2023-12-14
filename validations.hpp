#include "definitions.hpp"

#ifndef __VALIDATIONS_HPP__
#define __VALIDATIONS_HPP__


bool is_valid_port(string port);
bool is_valid_ip(string ip);

bool is_valid_date_time(string input);
bool is_valid_AID(string AID);
bool is_valid_state(string state);
bool is_valid_fname(string fname);
bool is_valid_fsize(int fsize);
bool is_valid_date_time(string dateTimeString);
bool is_valid_UID(string UID);
bool is_valid_timeactive(int timeactive);
bool is_valid_time_seconds(int time, int timeactive);
bool is_valid_pass(string pass);
bool is_valid_auction_name(string name);
bool is_valid_bid_value(int value);
bool is_valid_bid_time(string time);
bool is_valid_start_time(int start_time);


#endif
