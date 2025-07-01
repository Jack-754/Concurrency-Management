#include "LockManager.h"
#include <unistd.h>

using namespace std;

Logger logger;

LockManager::LockManager() {
    pthread_mutex_init(&globalMutex, nullptr);
}

LockManager::~LockManager() {
    pthread_mutex_lock(&globalMutex);
    for (auto& pair : resourceTable) {
        delete pair.second;
    }
    pthread_mutex_unlock(&globalMutex);
    pthread_mutex_destroy(&globalMutex);
}

LockManager::Resource::Resource() : sharedCount(0), exclusiveHeld(false) {
    pthread_mutex_init(&mutex, nullptr);
}

LockManager::Resource::~Resource() {
    pthread_mutex_destroy(&mutex);
    while (!waitQueue.empty()) {
        delete waitQueue.front();
        waitQueue.pop();
    }
}

LockManager::Waiter::Waiter(pthread_t t, LockType lt) : thread(t), lockType(lt), granted(false) {
    pthread_cond_init(&cond, nullptr);
}

void LockManager::lockResource(const string& resName, LockType type) {
    pthread_mutex_lock(&globalMutex);

    if (resourceTable.find(resName) == resourceTable.end()) {
        resourceTable[resName] = new Resource();
    }
    Resource* res = resourceTable[resName];

    pthread_mutex_unlock(&globalMutex);

    pthread_mutex_lock(&res->mutex);

    rag.addRequestEdge(pthread_self(), resName);
    if (rag.hasCycle()) {
        rag.removeRequestEdge(pthread_self(), resName);
        pthread_mutex_unlock(&res->mutex);
        logger.log("Thread " + to_string(pthread_self()) + " detected DEADLOCK while requesting " + resName);
        return;
    }

    if (type == SHARED && !res->exclusiveHeld && res->waitQueue.empty()) {
        res->sharedCount++;
        rag.addAllocationEdge(pthread_self(), resName);
        rag.removeRequestEdge(pthread_self(), resName);
        logger.log("Thread " + to_string(pthread_self()) + " acquired SHARED lock on " + resName);
    }
    else if (type == EXCLUSIVE && res->sharedCount == 0 && !res->exclusiveHeld && res->waitQueue.empty()) {
        res->exclusiveHeld = true;
        rag.addAllocationEdge(pthread_self(), resName);
        rag.removeRequestEdge(pthread_self(), resName);
        logger.log("Thread " + to_string(pthread_self()) + " acquired EXCLUSIVE lock on " + resName);
    }
    else {
        Waiter* waiter = new Waiter(pthread_self(), type);
        res->waitQueue.push(waiter);
        logger.log("Thread " + to_string(pthread_self()) + " waiting for " + (type == SHARED ? "SHARED" : "EXCLUSIVE") + " lock on " + resName);

        do {
            pthread_cond_wait(&waiter->cond, &res->mutex);
        } while (!waiter->granted);

        if (type == SHARED) {
            res->sharedCount++;
        } else {
            res->exclusiveHeld = true;
        }

        rag.addAllocationEdge(pthread_self(), resName);
        rag.removeRequestEdge(pthread_self(), resName);
        logger.log("Thread " + to_string(pthread_self()) + " acquired " + (type == SHARED ? "SHARED" : "EXCLUSIVE") + " lock on " + resName);

        delete waiter;
    }

    pthread_mutex_unlock(&res->mutex);
}

void LockManager::unlockResource(const string& resName) {
    pthread_mutex_lock(&globalMutex);

    if (resourceTable.find(resName) == resourceTable.end()) {
        logger.log("Error: Resource not found: " + resName);
        pthread_mutex_unlock(&globalMutex);
        return;
    }
    Resource* res = resourceTable[resName];

    pthread_mutex_unlock(&globalMutex);

    pthread_mutex_lock(&res->mutex);

    if (res->exclusiveHeld) {
        res->exclusiveHeld = false;
        rag.removeAllocationEdge(pthread_self(), resName);
        logger.log("Thread " + to_string(pthread_self()) + " released EXCLUSIVE lock on " + resName);
    }
    else if (res->sharedCount > 0) {
        res->sharedCount--;
        if (res->sharedCount == 0) {
            rag.removeAllocationEdge(pthread_self(), resName);
        }
        logger.log("Thread " + to_string(pthread_self()) + " released SHARED lock on " + resName);
    }
    else {
        logger.log("Error: No lock held on " + resName);
    }

    tryGrantLocks(res);

    pthread_mutex_unlock(&res->mutex);
}

void LockManager::tryGrantLocks(Resource* res) {
    while (!res->waitQueue.empty()) {
        Waiter* next = res->waitQueue.front();

        if (next->lockType == SHARED) {
            if (!res->exclusiveHeld) {
                while (!res->waitQueue.empty()) {
                    Waiter* sharedWaiter = res->waitQueue.front();
                    if (sharedWaiter->lockType != SHARED) break;

                    sharedWaiter->granted = true;
                    pthread_cond_signal(&sharedWaiter->cond);
                    res->waitQueue.pop();
                }
            }
            break;
        }
        else { // EXCLUSIVE
            if (res->sharedCount == 0 && !res->exclusiveHeld) {
                next->granted = true;
                pthread_cond_signal(&next->cond);
                res->waitQueue.pop();
            }
            break;
        }
    }
}
