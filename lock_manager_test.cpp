#include "LockManager.h"
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

LockManager lm;

void* sharedTask(void* arg) {
    string res = *(string*)arg;
    lm.lockResource(res, SHARED);
    sleep(1); // Simulate work
    lm.unlockResource(res);
    return nullptr;
}

void* exclusiveTask(void* arg) {
    string res = *(string*)arg;
    lm.lockResource(res, EXCLUSIVE);
    sleep(2); // Simulate work
    lm.unlockResource(res);
    return nullptr;
}

int main() {
    pthread_t t1, t2, t3, t4, t5, t6;
    string res1 = "resource1";
    string res2 = "resource2";

    // 2 shared locks on resource1
    pthread_create(&t1, nullptr, sharedTask, &res1);
    pthread_create(&t2, nullptr, sharedTask, &res1);

    sleep(0.5);

    // Exclusive lock requested on resource1, will wait
    pthread_create(&t3, nullptr, exclusiveTask, &res1);

    sleep(0.5);

    // Another shared lock on resource1, will also wait
    pthread_create(&t4, nullptr, sharedTask, &res1);

    // Independent resource2 usage
    pthread_create(&t5, nullptr, exclusiveTask, &res2);
    pthread_create(&t6, nullptr, sharedTask, &res2);

    // Join all threads
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);
    pthread_join(t4, nullptr);
    pthread_join(t5, nullptr);
    pthread_join(t6, nullptr);

    cout << "Test completed. Check lock_manager.log for details." << endl;

    return 0;
}
