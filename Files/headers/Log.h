#include <vector>
#include <string>

struct Log {
    bool keepLog;
    std :: vector <std :: pair <std :: string, std :: string>> log;

    /* Adds new data to the log */
    void add(std :: string message, std :: string color) {
        if (keepLog)
            log.push_back(std :: make_pair(message, color));
    }

    /* Clears the log */
    void clear() {
        this -> log.clear();
    }

    Log(): keepLog(true) {}
};
