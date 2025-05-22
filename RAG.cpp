#include "RAG.h"

RAG::RAG() {
    pthread_mutex_init(&graphMutex, nullptr);
}

void RAG::addEdge(const std::string& from, const std::string& to) {
    pthread_mutex_lock(&graphMutex);
    graph[from].insert(to);
    pthread_mutex_unlock(&graphMutex);
}

void RAG::removeEdge(const std::string& from, const std::string& to) {
    pthread_mutex_lock(&graphMutex);
    graph[from].erase(to);
    pthread_mutex_unlock(&graphMutex);
}

bool RAG::dfs(const std::string& node, std::unordered_set<std::string>& visited, std::unordered_set<std::string>& stack) {
    if (stack.count(node)) return true;
    if (visited.count(node)) return false;

    visited.insert(node);
    stack.insert(node);

    for (const auto& neighbor : graph[node]) {
        if (dfs(neighbor, visited, stack)) return true;
    }

    stack.erase(node);
    return false;
}

bool RAG::detectDeadlock() {
    pthread_mutex_lock(&graphMutex);
    std::unordered_set<std::string> visited, stack;
    bool hasCycle = false;
    for (const auto& [node, _] : graph) {
        if (!visited.count(node)) {
            if (dfs(node, visited, stack)) {
                hasCycle = true;
                break;
            }
        }
    }
    pthread_mutex_unlock(&graphMutex);
    return hasCycle;
}