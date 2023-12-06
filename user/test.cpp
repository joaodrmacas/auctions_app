#include <iostream>
#include <string>
#include <regex>
#include <ctime>

bool isValidDateTimeFormat(const std::string& dateTimeString) {
    // Define the regular expression pattern for the specified format
    std::regex dateTimeRegex("^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}$");

    // Check if the string matches the pattern
    if (std::regex_match(dateTimeString, dateTimeRegex)) {
        std::tm timeStruct = {};
        std::istringstream ss(dateTimeString);

        // Parse the string into a std::tm structure
        ss >> std::get_time(&timeStruct, "%Y-%m-%d %H:%M:%S");

        // Check for parsing errors and the validity of individual components
        if (ss.fail() == false &&
            timeStruct.tm_year >= 0 && timeStruct.tm_mon >= 0 && timeStruct.tm_mon < 12 &&
            timeStruct.tm_mday >= 1 && timeStruct.tm_mday <= 31 &&
            timeStruct.tm_hour >= 0 && timeStruct.tm_hour <= 23 &&
            timeStruct.tm_min >= 0 && timeStruct.tm_min <= 59 &&
            timeStruct.tm_sec >= 0 && timeStruct.tm_sec <= 59) {
            
            // Check if the day is valid for the given month (taking into account leap years)
            int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if (timeStruct.tm_mon == 2 && ((timeStruct.tm_year % 4 == 0 && timeStruct.tm_year % 100 != 0) || (timeStruct.tm_year % 400 == 0))) {
                // Adjust days in February for leap years
                daysInMonth[2] = 29;
            }

            if (timeStruct.tm_mday <= daysInMonth[timeStruct.tm_mon]) {
                return true; // Valid format and valid components
            }
        }
    }

    return false; // Invalid format or invalid components
}

int main() {
    std::string testDateTime = "2023-02-29 15:30:45";

    if (isValidDateTimeFormat(testDateTime)) {
        std::cout << "The string follows the specified format and time rules." << std::endl;
    } else {
        std::cout << "Invalid format or time rules." << std::endl;
    }

    return 0;
}
