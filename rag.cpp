#include "rag.h"
#include "Logger.h"
#include <iostream>
#include <sstream>

extern Logger logger; // shared logger instance

using namespace std;

RAG::RAG() {
    pthread_mutex_init(&mutex, nullptr);
}

RAG::~RAG() {
    pthread_mutex_destroy(&mutex);
}

string RAG::threadToString(pthread_t thread) {
    stringstream ss;
    ss << thread;
    return ss.str();
}

void RAG::addRequestEdge(pthread_t thread, const string& resource) {
    pthread_mutex_lock(&mutex);
    adj[threadToString(thread)].insert(resource);
    pthread_mutex_unlock(&mutex);
}

void RAG::addAllocationEdge(pthread_t thread, const string& resource) {
    pthread_mutex_lock(&mutex);
    adj[resource].insert(threadToString(thread));
    adj[threadToString(thread)].erase(resource);
    pthread_mutex_unlock(&mutex);
}

void RAG::removeAllocationEdge(pthread_t thread, const string& resource) {
    pthread_mutex_lock(&mutex);
    adj[resource].erase(threadToString(thread));
    pthread_mutex_unlock(&mutex);
}

void RAG::removeRequestEdge(pthread_t thread, const string& resource) {
    pthread_mutex_lock(&mutex);
    adj[threadToString(thread)].erase(resource);
    pthread_mutex_unlock(&mutex);
}

bool RAG::dfs(const string& node, set<string>& visited, set<string>& recStack) {
    visited.insert(node);
    recStack.insert(node);

    for (const string& neighbor : adj[node]) {
        if (recStack.count(neighbor)) return true;
        if (!visited.count(neighbor) && dfs(neighbor, visited, recStack)) return true;
    }

    recStack.erase(node);
    return false;
}

bool RAG::hasCycle() {
    pthread_mutex_lock(&mutex);

    set<string> visited;
    set<string> recStack;

    for (const auto& pair : adj) {
        if (!visited.count(pair.first)) {
            if (dfs(pair.first, visited, recStack)) {
                pthread_mutex_unlock(&mutex);
                //logger.log("No cycle detected");
                return true;
            }
        }
    }
    //logger.log("Cycle detected");
    pthread_mutex_unlock(&mutex);
    return false;
}
