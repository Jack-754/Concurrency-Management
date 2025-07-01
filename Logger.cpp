#include "Logger.h"
#include <iostream>
#include <ctime>

pthread_mutex_t Logger::logMutex = PTHREAD_MUTEX_INITIALIZER;

void Logger::init() {}

void Logger::log(const std::string& message) {
    pthread_mutex_lock(&logMutex);
    std::ofstream logfile("logs/operations.log", std::ios::app);
    std::time_t now = std::time(nullptr);
    std::string time_str = std::ctime(&now);
    time_str.pop_back(); 
    logfile << "[" << time_str << "] " << message << "\n";
    std::cout << message << std::endl;
    logfile.close();
    pthread_mutex_unlock(&logMutex);
}