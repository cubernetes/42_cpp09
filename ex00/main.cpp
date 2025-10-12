#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <deque>
#include <stdlib.h>
#include <iostream>

int dateToInt(std::string dateStr) {
    if (dateStr.length() < 10)
        return -1;
    std::string tmpDateStr = dateStr;
    if (tmpDateStr[4] != '-' || tmpDateStr[7] != '-')
        return -1;
    tmpDateStr[4] = '\0';
    tmpDateStr[7] = '\0';
    if (!std::isdigit(tmpDateStr[0]) ||
        !std::isdigit(tmpDateStr[1]) ||
        !std::isdigit(tmpDateStr[2]) ||
        !std::isdigit(tmpDateStr[3]))
        return -1;
    int year = std::atoi(tmpDateStr.c_str());
    if (!std::isdigit(tmpDateStr[5]) ||
        !std::isdigit(tmpDateStr[6]))
        return -1;
    int month = std::atoi(tmpDateStr.c_str() + 5);
    if (!std::isdigit(tmpDateStr[8]) ||
        !std::isdigit(tmpDateStr[9]))
        return -1;
    int day = std::atoi(tmpDateStr.c_str() + 8);
    if (day > 31 || day < 1 || month > 12 || month < 1)
        return -1;
    bool isLeapYear = year % 400 == 0 || (year % 100 != 0 && year % 4 == 0);
    if (day == 31 && (month == 4 || month == 6 || month == 9 || month == 11))
        return -1;
    if (month == 2 && day > (isLeapYear ? 29 : 28))
        return -1;

    dateStr[4] = '0';
    dateStr[7] = '0';
    char *end;
    int dateAsInt = static_cast<int>(strtol(dateStr.c_str(), &end, 10));
    return *end ? -1 : dateAsInt;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream dataRaw("data.csv");
    std::string line;
    std::deque<std::pair<int, double> > data;
    (void)std::getline(dataRaw, line); // ignore header
    while (std::getline(dataRaw, line)) {
        std::istringstream iss(line);
        std::string dateStr, rateStr;
        std::getline(iss, dateStr, ',');
        std::getline(iss, rateStr, ',');
        int dateAsInt = dateToInt(dateStr);
        if (dateAsInt == -1) {
            std::cerr << "In file data.csv: Failed to parse date: '" << dateStr << "'" << std::endl;
            return EXIT_FAILURE;
        }
        char *endptr;
        double rateAsDouble = std::strtod(rateStr.c_str(), &endptr);
        if (*endptr) {
            std::cerr << "In file data.csv: For date: " << dateStr << ": Failed to parse rate: '" << rateStr << "'" << std::endl;
            return EXIT_FAILURE;
        }
        data.push_back(std::make_pair(dateAsInt, rateAsDouble));
    }

    std::ifstream inputRaw(argv[1]);
    (void)std::getline(inputRaw, line); // ignore header
    while (std::getline(inputRaw, line)) {
        std::istringstream iss(line);
        std::string dateStr, amountStr;
        std::getline(iss, dateStr, '|');
        std::getline(iss, amountStr, '\0');
        if (!dateStr.empty() && dateStr[dateStr.length() - 1] == ' ')
            dateStr[dateStr.length() - 1] = '\0';
        if (!amountStr.empty() && amountStr[amountStr.length() - 1] == ' ') {
            dateStr[dateStr.length() - 1] = '\0';
            if (amountStr.length() == 1)
                amountStr = "";
        }
        int dateAsInt = dateToInt(dateStr);
        if (dateAsInt == -1) {
            std::cerr << "In file " << argv[1] << ": Failed to parse date: '" << dateStr << "'" << std::endl;
            continue;
        }
        if (amountStr[0] == '\0') {
            std::cerr << "In file " << argv[1] << ": For date: " << dateStr << ": Amount is empty" << std::endl;
            continue;
        }
        char *endptr;
        double amountAsDouble = std::strtod(amountStr.c_str(), &endptr);
        if (*endptr) {
            std::cerr << "In file " << argv[1] << ": For date: " << dateStr << ": Failed to parse amount: '" << amountStr << "'" << std::endl;
            continue;
        } else if (amountAsDouble < 0 || amountAsDouble > 1000) {
            std::cerr << "In file " << argv[1] << ": For date: " << dateStr << ": Amount is out of range ([0, 1000]): " << amountAsDouble << std::endl;
            continue;
        }
        std::cout << "Input data: " << dateAsInt << ", " << amountAsDouble << std::endl;
    }
    return EXIT_SUCCESS;
}
