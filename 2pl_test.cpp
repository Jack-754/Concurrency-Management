#include "2PLManager.h"
#include <pthread.h>
#include <unistd.h>
#include <iostream>

using namespace std;

TwoPLManager twoPL;

void* validTransaction(void* arg) {
    cout << "Transaction " << pthread_self() << " starting valid 2PL transaction." << endl;

    if (!twoPL.acquire("resource1", SHARED)) return nullptr;
    sleep(1);
    if (!twoPL.acquire("resource2", EXCLUSIVE)) return nullptr;
    sleep(1);

    twoPL.release("resource1");
    twoPL.release("resource2");

    cout << "Transaction " << pthread_self() << " completed valid 2PL transaction." << endl;
    return nullptr;
}

void* invalidTransaction(void* arg) {
    cout << "Transaction " << pthread_self() << " starting invalid 2PL transaction." << endl;

    if (!twoPL.acquire("resource3", SHARED)) return nullptr;
    sleep(1);
    twoPL.release("resource3");
    sleep(1);

    // This should violate 2PL
    if (!twoPL.acquire("resource4", EXCLUSIVE)) {
        cout << "Transaction " << pthread_self() << " correctly blocked due to 2PL violation." << endl;
    }

    return nullptr;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, nullptr, validTransaction, nullptr);
    pthread_create(&t2, nullptr, invalidTransaction, nullptr);

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);

    cout << "Test completed. Check lock_manager.log for detailed logs." << endl;
    return 0;
}
