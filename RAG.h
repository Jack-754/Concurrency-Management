#ifndef RAG_H
#define RAG_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <pthread.h>

class RAG {
public:
    RAG();
    void addEdge(const std::string& from, const std::string& to);
    void removeEdge(const std::string& from, const std::string& to);
    bool detectDeadlock();

private:
    bool dfs(const std::string& node, std::unordered_set<std::string>& visited, std::unordered_set<std::string>& stack);
    std::unordered_map<std::string, std::unordered_set<std::string>> graph;
    pthread_mutex_t graphMutex;
};

#endif