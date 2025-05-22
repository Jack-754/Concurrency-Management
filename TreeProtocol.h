#ifndef TREEPROTOCOL_H
#define TREEPROTOCOL_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <pthread.h>

class TreeProtocol {
public:
    TreeProtocol();
    void setTree(const std::unordered_map<std::string, std::unordered_set<std::string>>& tree);
    bool canLock(const std::string& txnId, const std::string& node);
    void recordLock(const std::string& txnId, const std::string& node);

private:
    std::unordered_map<std::string, std::unordered_set<std::string>> tree;
    std::unordered_map<std::string, std::unordered_set<std::string>> txnLocks;
    pthread_mutex_t treeMutex;

    std::string getParent(const std::string& node);
};

#endif