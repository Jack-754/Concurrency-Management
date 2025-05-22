#include "LockManager.h"
#include "TransactionManager.h"
#include "RAG.h"
#include "TreeProtocol.h"
#include "Logger.h"

int main() {
    LockManager lm;
    RAG rag;
    Transaction t1("TXN1");
    Transaction t2("TXN2");

    t1.acquire("A", SHARED, lm);
    t2.acquire("A", EXCLUSIVE, lm); // Should wait
    t1.release("A", lm);
    t2.acquire("A", EXCLUSIVE, lm); // Should succeed now
    t2.release("A", lm);

    if (rag.detectDeadlock()) {
        Logger::log("Deadlock detected!");
    }

    return 0;
}
