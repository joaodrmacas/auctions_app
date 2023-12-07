#include "definitions.hpp"


bool is_valid_AID(string AID){
    if (AID.length() != AID_SIZE){
        STATUS("AID is not size 3")
        return false;
    }

    for (char c : AID) {
        if (!isdigit(c)) {
            STATUS("AID is not numeric")
            return false;  // If any character is not a digit, the string is not numeric
        }
    }
    return true;
}

bool is_valid_state(string state){
    if (state == "0" || state == "1") return true;
    STATUS("State is not either 0 or 1.")
    return false;
}

bool is_valid_fname(string fname){
    if (fname.length()>FILE_NAME_MAX_SIZE) {
        STATUS("Filename is too long >24 chars.")
        return false;
    }
    for (char c: fname){
        if (!isalnum(c) && c!='-' && c!='_' && c!='.') {
            STATUS_WA("Filename has invalid characters: %c", c)
            return false;
        }
    }
    return true;
}

bool is_valid_fsize(int fsize){
    if (fsize>FILE_MAX_SIZE){
        STATUS("File is larger than 10*10^6 Bytes")
        return false;
    }
    return true;
}

bool isValidDateFormat(string input) {
    std::istringstream ss(input);
    int year, month, day, hour, minute, second;
    char dash1, dash2, space1, colon1, colon2;

    if (input.length() == DATE_TIME_LEN){
        STATUS("Date_time is not 19 characters.")
    }

    // Attempt to read the values with the specified format
    // if (!(ss >> year >> dash1 >> month >> dash2 >> day >> hour >> colon1 >> minute >> colon2 >> second) ||
    //     (dash1 != '-' || dash2 != '-' || colon1 != ':' || colon2 != ':') ||
    //     (month < 1 || month > 12) ||
    //     (day < 1 || day > 31) ||
    //     (hour < 0 || hour > 23) ||
    //     (minute < 0 || minute > 59) ||
    //     (second < 0 || second > 59) ||
    //     !ss.eof()) {

    //     return false;  // Invalid format or invalid values
    // }

    if (!(ss >> year >> dash1 >> month >> dash2 >> day >> hour >> colon1 >> minute >> colon2 >> second)){
        STATUS("Couldnt extract date in the correct form: ")
        return false;
    }

    else if (dash1 != '-' || dash2 != '-' || colon1 != ':' || colon2 != ':'){
        STATUS("Date does not have - or :")
        return false;
    }

    else if (month < 1 || month > 12){
        STATUS("Incorrect month")
        return false;
    }

    else if (day < 1 || day > 31){
        STATUS("Incorrect day")
        return false;
    }

    else if (hour < 0 || hour > 23){
        STATUS("Incorrect hour")
        return false;
    }

    else if (minute < 0 || minute > 59){
        STATUS("Incorrect minute")
        return false;
    }

    else if (second < 0 || second > 59){
        STATUS("Incorrect second")
        return false;
    }

    else if(!ss.eof()){
        STATUS("Date didnt reach eof.")
        return false;
    }

    // Check if the day is valid for the given month
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
        STATUS("The month only has 30 days.")
        return false;
    } else if (month == 2) {
        // Check for leap year in February
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            if (day > 29) {
                STATUS("Leap year.")
                return false;
            }
        } else {
            if (day > 28) {
                STATUS("February doesnt have more than 28 days in default year.")
                return false;
            }
        }
    }

    return true;  // Valid format and values
}

bool is_valid_UID(string UID){
    for (char c : UID) {
        if (!isdigit(c)) {
            STATUS("UID should be only numeric.")
            return false;
        }
    }
    if (UID.length() != UID_LEN){
        STATUS("UID should be 6 digits")
        return false;
    }

    return true;
}

bool is_valid_timeactive(int timeactive){
    if (timeactive>MAX_TIME_ACTIVE) {
        STATUS("timeactive is higher than max_time.")
        return false;
    }

    if (timeactive<0){
        STATUS("timeactive is negative.")
        return false;
    }
    return true;
}

bool is_valid_pass(string pass){
    if (pass.length() != PASSWORD_LEN){
        STATUS("Password is not 8 characters long")
        return false;
    }
    
    for (char c : pass) {
        if (!isalnum(c)) {
            STATUS("Password should be alphanumeric.")
            return false;
        }
    }

    return true;
}

bool is_valid_auction_name(string name){
    if (name.length() > NAME_MAX_LEN) {
        STATUS("Auction name is too long.")
        return false;
    }

    for (char c : name) {
        if (!isalnum(c)) {
            STATUS("Auction name should be alphanumeric.")
            return false;
        }
    }

    return true;

}

bool is_valid_bid_value(int value){
    if (value > MAX_START_VALUE) {
        STATUS("Start_value is too big (> 999999).")
        return false;
    }

    if (value < 0) {
        STATUS("Start_value can't be negative.")
        return false;
    }

    return true;
}

bool is_valid_bid_time(string time) {
    if (time.length() != END_TIME_LEN) {
        STATUS("End_time is not 5 characters long.")
        return false;
    }

    for (char c : time) {
        if (!isdigit(c)) {
            STATUS("End_time should be numeric.")
            return false;
        }
    }

    return true;
}
