#ifndef TWO_PL_MANAGER_H
#define TWO_PL_MANAGER_H

#include "LockManager.h"
#include <map>
#include <set>
#include <pthread.h>

class TwoPLManager {
public:
    TwoPLManager();
    bool acquire(const std::string& resource, LockType type);
    void release(const std::string& resource);

private:
    LockManager lockManager;

    struct TransactionState {
        bool shrinkingPhase;
        std::set<std::string> heldResources;
    };

    std::map<uintptr_t , TransactionState> transactionTable;
    pthread_mutex_t mutex;
};

#endif //TWO_PL_MANAGER_H