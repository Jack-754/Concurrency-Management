#include "2PLManager.h"
#include "Logger.h"
#include <sstream>

extern Logger logger; // shared logger instance

TwoPLManager::TwoPLManager() {
    pthread_mutex_init(&mutex, nullptr);
}

uintptr_t threadToInt(pthread_t thread) {
    return reinterpret_cast<uintptr_t>(thread);
}

bool TwoPLManager::acquire(const std::string& resource, LockType type) {
    uintptr_t tid = threadToInt(pthread_self());

    pthread_mutex_lock(&mutex);
    TransactionState& state = transactionTable[tid];

    //logger.log(std::to_string(state.shrinkingPhase) + " " + std::to_string(tid) + "\n");

    if (state.shrinkingPhase) {
        logger.log("Thread " + std::to_string(tid) + " violated 2PL by acquiring " + resource);
        pthread_mutex_unlock(&mutex);
        return false;
    }

    pthread_mutex_unlock(&mutex);

    lockManager.lockResource(resource, type);

    pthread_mutex_lock(&mutex);
    state.heldResources.insert(resource);
    pthread_mutex_unlock(&mutex);

    return true;
}

void TwoPLManager::release(const std::string& resource) {
    uintptr_t tid = threadToInt(pthread_self());

    lockManager.unlockResource(resource);
    //logger.log(std::to_string(tid) + " " + resource + "\n");

    pthread_mutex_lock(&mutex);
    TransactionState& state = transactionTable[tid];
    state.heldResources.erase(resource);

    state.shrinkingPhase=true;

    pthread_mutex_unlock(&mutex);
}

