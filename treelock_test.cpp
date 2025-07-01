#include "TreeLockManager.h"
#include <pthread.h>
#include <unistd.h>
#include <iostream>

using namespace std;

TreeLockManager treeLock;

void* validTransaction(void* arg) {
    cout << "Thread " << pthread_self() << " starting valid tree lock transaction." << endl;

    if (!treeLock.acquire("A")) return nullptr;
    sleep(1);

    if (!treeLock.acquire("B")) return nullptr;
    sleep(1);

    if (!treeLock.acquire("D")) return nullptr;
    sleep(1);

    treeLock.release("B");
    treeLock.release("D");
    treeLock.release("A");

    cout << "Thread " << pthread_self() << " completed valid tree lock transaction." << endl;
    return nullptr;
}

void* invalidParentViolation(void* arg) {
    cout << "Thread " << pthread_self() << " starting transaction with parent violation." << endl;

    if (!treeLock.acquire("C")) return nullptr;
    sleep(1);

    // Try to lock D without holding its parent B
    if (!treeLock.acquire("D")) {
        cout << "Correctly blocked parent violation on D." << endl;
    }

    treeLock.release("C");

    return nullptr;
}

void* invalidRelockViolation(void* arg) {
    cout << "Thread " << pthread_self() << " starting transaction with relock violation." << endl;

    if (!treeLock.acquire("A")) return nullptr;
    sleep(1);

    treeLock.release("A");
    sleep(1);

    if (!treeLock.acquire("A")) {
        cout << "Correctly blocked relock violation on A." << endl;
    }

    return nullptr;
}

int main() {
    // Tree structure: A -> B, A -> C, B -> D
    treeLock.addEdge("A", "B");
    treeLock.addEdge("A", "C");
    treeLock.addEdge("B", "D");

    pthread_t t1, t2, t3;

    pthread_create(&t1, nullptr, validTransaction, nullptr);
    sleep(1);
    pthread_create(&t2, nullptr, invalidParentViolation, nullptr);
    sleep(1);
    pthread_create(&t3, nullptr, invalidRelockViolation, nullptr);

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);

    cout << "Tree Lock Manager test completed. Check lock_manager.log for details." << endl;
    return 0;
}
