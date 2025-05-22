#include "TreeProtocol.h"

TreeProtocol::TreeProtocol() {
    pthread_mutex_init(&treeMutex, nullptr);
}

void TreeProtocol::setTree(const std::unordered_map<std::string, std::unordered_set<std::string>>& t) {
    pthread_mutex_lock(&treeMutex);
    tree = t;
    pthread_mutex_unlock(&treeMutex);
}

std::string TreeProtocol::getParent(const std::string& node) {
    pthread_mutex_lock(&treeMutex);
    for (const auto& [parent, children] : tree) {
        if (children.count(node)) {
            pthread_mutex_unlock(&treeMutex);
            return parent;
        }
    }
    pthread_mutex_unlock(&treeMutex);
    return "";
}

bool TreeProtocol::canLock(const std::string& txnId, const std::string& node) {
    pthread_mutex_lock(&treeMutex);
    std::string parent = getParent(node);
    bool result = parent.empty() || txnLocks[txnId].count(parent);
    pthread_mutex_unlock(&treeMutex);
    return result;
}

void TreeProtocol::recordLock(const std::string& txnId, const std::string& node) {
    pthread_mutex_lock(&treeMutex);
    txnLocks[txnId].insert(node);
    pthread_mutex_unlock(&treeMutex);
}