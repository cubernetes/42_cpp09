#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

int dateToInt(std::string &dateStr) {
    dateStr[4] = '0';
    dateStr[7] = '0';
    char *end;
    strtol(dateStr.c_str(), &end, 10);
}

int main() {
    std::ifstream dataRaw("data.csv");
    std::string line;
    std::vector<std::pair<int, float> > data;
    while (std::getline(dataRaw, line)) {
        std::istringstream iss(line);
        std::string dateStr, rateStr;
        std::getline(iss, dateStr, ',');
        std::getline(iss, rateStr, ',');
        int epoch = dateToInt(dateStr);
        int rate = 1;
        data.push_back(std::make_pair(epoch, rate));
    }
    return 0;
}
