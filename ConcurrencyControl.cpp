#include "ConcurrencyControl.h"
#include "Logger.h"
#include "LockManager.h"

void initializeConcurrency() {
    Logger::init();
    LockManager::init();
}