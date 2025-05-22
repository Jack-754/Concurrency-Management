#include "TransactionManager.h"
#include "Logger.h"
#include <pthread.h>
#include <cstdio>

Transaction::Transaction(const std::string& id) : txnId(id), phase("growing") {
    threadId = pthread_self();
}

std::string Transaction::getCombinedId() const {
    char buffer[65];
    snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)threadId);
    return txnId + "_" + buffer;
}

bool Transaction::acquire(const std::string& resource, LockType type, LockManager& lm) {
    if (phase == "shrinking") {
        Logger::log("[" + getCombinedId() + "] 2PL VIOLATION: cannot acquire in shrinking phase.");
        return false;
    }
    lm.acquireLock(getCombinedId(), resource, type);
    heldLocks.insert(resource);
    return true;
}

void Transaction::release(const std::string& resource, LockManager& lm) {
    lm.releaseLock(getCombinedId(), resource);
    heldLocks.erase(resource);
    if (phase == "growing") {
        phase = "shrinking";
    }
}

std::string Transaction::getId() const {
    return txnId;
}