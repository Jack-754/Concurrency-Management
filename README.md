# Concurrency Manager

This project implements a concurrency control system in C++, providing various locking mechanisms to manage shared resources in a multi-threaded environment. It includes a Two-Phase Locking (2PL) manager, a Tree Locking manager, a generic Lock Manager, a Resource Allocation Graph (RAG) for deadlock detection, and a logging utility.

## Features

* **Two-Phase Locking (2PL) Manager**: Ensures serializability of transactions by enforcing the 2PL protocol (growing and shrinking phases).

* **Tree Locking Manager**: Implements a hierarchical locking scheme, allowing more granular control over resources organized in a tree structure. It prevents cycles in the lock hierarchy.

* **Generic Lock Manager**: Provides basic shared and exclusive locking capabilities for individual resources, handling waiting queues and granting locks.

* **Resource Allocation Graph (RAG)**: Dynamically tracks resource requests and allocations to detect deadlocks using cycle detection.

* **Logging**: A simple logging utility to record operations and events, useful for debugging and monitoring concurrency behavior.

## Project Structure

The project is organized into several modular components:

* `2PLManager.h`/`2PLManager.cpp`: Implements the Two-Phase Locking protocol.

* `TreeLockManager.h`/`TreeLockManager.cpp`: Implements the Tree Locking protocol.

* `LockManager.h`/`LockManager.cpp`: Provides the core shared/exclusive lock management for resources.

* `rag.h`/`rag.cpp`: Manages the Resource Allocation Graph for deadlock detection.

* `Logger.h`/`Logger.cpp`: A utility for logging messages to a file and console.

## How It Works

### Two-Phase Locking (2PL)

The `TwoPLManager` ensures that transactions (represented by threads) acquire all necessary locks during a "growing phase" and release them during a "shrinking phase." Once a transaction releases any lock, it enters the shrinking phase and cannot acquire any new locks. This helps maintain serializability.

### Tree Locking

The `TreeLockManager` allows you to define a hierarchy of resources (e.g., a database, tables within a database, rows within a table). To acquire a lock on a resource, a thread must first hold a lock on its parent resource in the tree. This approach can prevent deadlocks in certain scenarios.

### Lock Manager

The `LockManager` is the foundational component. It handles requests for shared (`SHARED`) or exclusive (`EXCLUSIVE`) locks on named resources.

* **Shared Lock**: Multiple threads can hold a shared lock concurrently.

* **Exclusive Lock**: Only one thread can hold an exclusive lock at a time.
    It manages a wait queue for resources and uses condition variables to signal waiting threads when a lock becomes available.

### Resource Allocation Graph (RAG)

The `RAG` class maintains a directed graph where nodes are threads and resources. Edges represent requests (thread to resource) and allocations (resource to thread). A cycle in this graph indicates a deadlock. The `LockManager` uses the RAG to detect deadlocks before granting a lock.

### Logger

The `Logger` class provides a thread-safe mechanism to write log messages to a file (`logs/operations.log`) and the console, timestamping each entry.

## Building the Project

This project uses standard C++ libraries and `pthread` for threading.

1.  **Prerequisites**:

    * A C++ compiler (e.g., g++)

    * `pthread` library (usually available by default on Linux/macOS, or via MinGW/Cygwin on Windows).

2.  **Compilation**:
    Navigate to the project's root directory in your terminal and compile the source files. You'll need to link with the `pthread` library.

    ```
    g++ -std=c++17 -pthread -o concurrency_manager 2PLManager.cpp LockManager.cpp Logger.cpp rag.cpp TreeLockManager.cpp main.cpp # Assuming you have a main.cpp
    ```

    *(Note: You might need to create a `main.cpp` file to demonstrate the usage of these managers.)*

## Usage

To use these managers, you would typically:

1.  Include the necessary header files (`2PLManager.h`, `TreeLockManager.h`, `LockManager.h`, `Logger.h`).

2.  Instantiate the desired manager (e.g., `TwoPLManager`, `TreeLockManager`).

3.  For `TreeLockManager`, define the resource hierarchy using `addEdge`.

4.  Call `acquire` to request a lock and `release` to release it.

5.  Ensure proper thread management and synchronization in your application logic.
