#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

bool isValidDateFormat(const std::string& input) {
    std::istringstream ss(input);
    int year, month, day, hour, minute, second;
    char dash1, dash2, space1, colon1, colon2;

    // Attempt to read the values with the specified format
    if (!(ss >> year >> dash1 >> month >> dash2 >> day >> hour >> colon1 >> minute >> colon2 >> second) ||
        (dash1 != '-' || dash2 != '-' || colon1 != ':' || colon2 != ':') ||
        (month < 1 || month > 12) ||
        (day < 1 || day > 31) ||
        (hour < 0 || hour > 23) ||
        (minute < 0 || minute > 59) ||
        (second < 0 || second > 59) ||
        !ss.eof()) {

        return false;  // Invalid format or invalid values
    }

    // Check if the day is valid for the given month
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
        return false;
    } else if (month == 2) {
        // Check for leap year in February
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            if (day > 29) {
                return false;
            }
        } else {
            if (day > 28) {
                return false;
            }
        }
    }

    return true;  // Valid format and values
}

int main() {
    // Test the function with a sample string
    std::string input = "2024-02-29 12:45:30";
    if (isValidDateFormat(input)) {
        std::cout << "The string has a valid format." << std::endl;
    } else {
        std::cout << "The string does not have a valid format." << std::endl;
    }

    return 0;
}
