#include "LockManager.h"
#include "Logger.h"

pthread_mutex_t LockManager::mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t LockManager::cond = PTHREAD_COND_INITIALIZER;

void LockManager::init() {}

void LockManager::acquireLock(const std::string& txnId, const std::string& resource, LockType type) {
    pthread_mutex_lock(&mutex);
    auto& lockInfo = lockTable[resource];

    while (true) {
        bool canAcquire = false;
        if (lockInfo.holders.empty()) {
            canAcquire = true;
        } else if (lockInfo.type == SHARED && type == SHARED) {
            canAcquire = true;
        }

        if (canAcquire) {
            lockInfo.holders.insert(txnId);
            lockInfo.type = type;
            Logger::log("[" + txnId + "] ACQUIRE LOCK ON " + resource);
            break;
        } else {
            lockInfo.waitingQueue.push({txnId, type});
            Logger::log("[" + txnId + "] WAITING FOR LOCK ON " + resource);
            pthread_cond_wait(&cond, &mutex);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void LockManager::releaseLock(const std::string& txnId, const std::string& resource) {
    pthread_mutex_lock(&mutex);
    auto& lockInfo = lockTable[resource];
    lockInfo.holders.erase(txnId);
    Logger::log("[" + txnId + "] RELEASE LOCK ON " + resource);

    if (lockInfo.holders.empty()) {
        while (!lockInfo.waitingQueue.empty()) {
            auto next = lockInfo.waitingQueue.front();
            auto [nextTxn, nextType] = next;

            if (nextType == SHARED) {
                while (!lockInfo.waitingQueue.empty() && lockInfo.waitingQueue.front().second == SHARED) {
                    auto [txn, type] = lockInfo.waitingQueue.front();
                    lockInfo.holders.insert(txn);
                    lockInfo.waitingQueue.pop();
                }
                lockInfo.type = SHARED;
            } else {
                if (lockInfo.holders.empty()) {
                    lockInfo.holders.insert(nextTxn);
                    lockInfo.type = EXCLUSIVE;
                    lockInfo.waitingQueue.pop();
                }
            }
            pthread_cond_broadcast(&cond);
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
}