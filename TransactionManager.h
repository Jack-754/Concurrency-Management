#ifndef TXNMANAGER_H
#define TXNMANAGER_H

#include <string>
#include <unordered_set>
#include <pthread.h>
#include "LockManager.h"

class Transaction {
public:
    Transaction(const std::string& id);
    bool acquire(const std::string& resource, LockType type, LockManager& lm);
    void release(const std::string& resource, LockManager& lm);
    std::string getId() const;
    std::string getCombinedId() const;

private:
    std::string txnId;
    pthread_t threadId; // Store thread ID
    std::string phase; // "growing", "shrinking"
    int pid;
    std::unordered_set<std::string> heldLocks;
};

#endif