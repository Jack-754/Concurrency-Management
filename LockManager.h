#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

#include <string>
#include <map>
#include <queue>
#include <pthread.h>
#include "rag.h"
#include "Logger.h"


enum LockType { SHARED, EXCLUSIVE };

class LockManager {
public:
    LockManager();
    ~LockManager();

    void lockResource(const std::string& resName, LockType type);
    void unlockResource(const std::string& resName);

private:
    RAG rag;

    struct Waiter {
        pthread_t thread;
        LockType lockType;
        pthread_cond_t cond;
        bool granted;

        Waiter(pthread_t t, LockType lt);
    };

    struct Resource {
        pthread_mutex_t mutex;
        int sharedCount;
        bool exclusiveHeld;
        std::queue<Waiter*> waitQueue;

        Resource();
        ~Resource();
    };

    std::map<std::string, Resource*> resourceTable;
    pthread_mutex_t globalMutex;

    void tryGrantLocks(Resource* res);
};

#endif // LOCK_MANAGER_H
