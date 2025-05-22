#ifndef LOGGER_H
#define LOGGER_H
#include <fstream>
#include <string>
#include <pthread.h>

class Logger {
private:
    static pthread_mutex_t logMutex;
public:
    static void log(const std::string& message);
    static void init();
};

#endif