#ifndef TREE_LOCK_MANAGER_H
#define TREE_LOCK_MANAGER_H

#include "LockManager.h"
#include <map>
#include <set>
#include <string>
#include <pthread.h>

class TreeLockManager {
public:
    TreeLockManager();
    bool addEdge(const std::string& parent, const std::string& child);
    bool acquire(const std::string& resource);
    void release(const std::string& resource);

private:
    LockManager lockManager;

    std::map<std::string, std::string> parentMap; // child -> parent
    std::map<pthread_t, std::set<std::string>> heldResources;
    std::map<pthread_t, std::set<std::string>> releasedResources;

    bool violatesTreeStructure(const std::string& child);
    bool isParentHeld(pthread_t tid, const std::string& resource);
    pthread_mutex_t mutex;
};

#endif // TREE_LOCK_MANAGER_H
