#ifndef LOCKMANAGER_H
#define LOCKMANAGER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <pthread.h>

enum LockType { SHARED, EXCLUSIVE };

class LockManager {
public:
    static void init();
    void acquireLock(const std::string& txnId, const std::string& resource, LockType type);
    void releaseLock(const std::string& txnId, const std::string& resource);

private:
    struct LockInfo {
        LockType type;
        std::unordered_set<std::string> holders;
        std::queue<std::pair<std::string, LockType>> waitingQueue;
    };

    static pthread_mutex_t mutex;
    static pthread_cond_t cond;
    std::unordered_map<std::string, LockInfo> lockTable;
};

#endif