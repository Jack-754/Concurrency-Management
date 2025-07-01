#ifndef RAG_H
#define RAG_H

#include <string>
#include <map>
#include <set>
#include <pthread.h>

class RAG {
public:
    RAG();
    ~RAG();

    void addRequestEdge(pthread_t thread, const std::string& resource);
    void addAllocationEdge(pthread_t thread, const std::string& resource);
    void removeAllocationEdge(pthread_t thread, const std::string& resource);
    void removeRequestEdge(pthread_t thread, const std::string& resource);

    bool hasCycle();

private:
    std::map<std::string, std::set<std::string>> adj;
    pthread_mutex_t mutex;

    bool dfs(const std::string& node, std::set<std::string>& visited, std::set<std::string>& recStack);
    std::string threadToString(pthread_t thread);
};

#endif // RAG_H
