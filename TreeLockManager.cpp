#include "TreeLockManager.h"
#include "Logger.h"
#include <iostream>

extern Logger logger;

TreeLockManager::TreeLockManager() {
    pthread_mutex_init(&mutex, nullptr);
}

bool TreeLockManager::addEdge(const std::string& parent, const std::string& child) {
    pthread_mutex_lock(&mutex);

    if (parentMap.find(child) != parentMap.end()) {
        logger.log("Edge addition failed: " + child + " already has a parent.");
        pthread_mutex_unlock(&mutex);
        return false;
    }

    parentMap[child] = parent;

    if (violatesTreeStructure(child)) {
        parentMap.erase(child);
        logger.log("Edge addition failed: cycle detected.");
        pthread_mutex_unlock(&mutex);
        return false;
    }

    logger.log("Edge added: " + parent + " -> " + child);
    pthread_mutex_unlock(&mutex);
    return true;
}

bool TreeLockManager::violatesTreeStructure(const std::string& child) {
    std::set<std::string> visited;
    std::string current = child;

    while (parentMap.find(current) != parentMap.end()) {
        if (visited.count(current)) return true;
        visited.insert(current);
        current = parentMap[current];
    }
    return false;
}

bool TreeLockManager::isParentHeld(pthread_t tid, const std::string& resource) {
    auto it = parentMap.find(resource);
    if (it == parentMap.end()) return true; // No parent, first lock allowed
    return heldResources[tid].count(it->second) > 0;
}

bool TreeLockManager::acquire(const std::string& resource) {
    pthread_t tid = pthread_self();

    pthread_mutex_lock(&mutex);

    if (releasedResources[tid].count(resource) > 0) {
        logger.log("Thread " + std::to_string(tid) + " cannot re-lock " + resource + " after release.");
        pthread_mutex_unlock(&mutex);
        return false;
    }

    if (!heldResources[tid].empty() && !isParentHeld(tid, resource)) {
        logger.log("Thread " + std::to_string(tid) + " cannot lock " + resource + " without holding its parent.");
        pthread_mutex_unlock(&mutex);
        return false;
    }

    pthread_mutex_unlock(&mutex);

    lockManager.lockResource(resource, EXCLUSIVE);

    pthread_mutex_lock(&mutex);
    heldResources[tid].insert(resource);
    pthread_mutex_unlock(&mutex);

    return true;
}

void TreeLockManager::release(const std::string& resource) {
    pthread_t tid = pthread_self();

    lockManager.unlockResource(resource);

    pthread_mutex_lock(&mutex);
    heldResources[tid].erase(resource);
    releasedResources[tid].insert(resource);

    if (heldResources[tid].empty() && releasedResources[tid].empty()) {
        heldResources.erase(tid);
        releasedResources.erase(tid);
    }

    pthread_mutex_unlock(&mutex);
}
