# Matrix Manipulation – Systems Programming

## Project Overview
This project focuses on developing a Matrix Manipulation System using C programming in Linux. It explores different approaches to executing matrix operations, managing processes and threads, implementing inter-process communication, and handling concurrent client-server connections.

The system dynamically generates matrices with random dimensions and numerical values, then performs matrix multiplication, transposition, and average calculations using different execution approaches.

## Features
- Dynamic matrix generation with random dimensions and values.
- Matrix multiplication.
- Matrix transposition.
- Matrix average calculation.
- Execution time measurement for individual operations.
- Sequential execution and performance measurement.
- Process-based parallel execution using `fork()`.
- Inter-process communication using pipes and named pipes.
- Parallel matrix multiplication and average calculation using multiple processes.
- Thread-based execution using POSIX Threads (Pthreads).
- Thread synchronization using mutexes.
- Concurrent client-server communication using sockets.
- Server-side transaction logging.
- Shell scripting for automation, file permissions, and log analysis.
- Automated execution of multiple client sessions.

## Technologies Used
- **Programming Language:** C
- **Operating System:** Linux
- **Thread Management:** POSIX Threads (Pthreads)
- **Process Management:** `fork()`, `wait()`
- **Inter-Process Communication:** Pipes and Named Pipes (FIFOs)
- **Networking:** Client-Server Socket Programming
- **Synchronization:** Mutex
- **Shell Scripting:** Bash
- **Compilation:** GCC

## Project Implementations

### 1. Sequential Implementation
The initial implementation executes matrix multiplication, transposition, and average calculation sequentially through separate functions.

Execution time is measured for each operation and for the overall application, establishing a performance baseline.

### 2. Multiprocessing
The system uses `fork()` to create separate child processes for matrix multiplication, transposition, and average calculation.

Each process performs its assigned operation independently, displays the result, and measures its execution time.

### 3. Inter-Process Communication (IPC)
Pipes and named pipes are used to exchange matrix dimensions, matrix values, calculation results, and execution times between processes.

The implementation further divides matrix multiplication and average calculations into smaller tasks handled by additional child processes.

The multiplication result is also transferred to the transpose process, where it is transposed and extended with an additional row of randomly generated values.

### 4. Multithreading
The system uses POSIX Threads to execute matrix operations concurrently.

Matrix multiplication and average calculations are divided into smaller tasks distributed among multiple threads.

A mutex is used to protect the shared average sum and prevent race conditions when multiple threads update it.

Additional thread management techniques, including semaphores, thread pools, and condition variables, are discussed.

### 5. Client-Server Implementation
A client-server architecture is implemented using C socket programming.

The client generates matrices and submits them to the server, which performs multiplication, transposition, and average calculations using multithreading.

The server supports multiple concurrent client connections through separate client-handling threads and maintains a server-side log of client transactions and calculation results.

### 6. Shell Scripting and Automation
A Bash script automates the execution and testing process.

The script:

- Compiles the client and server programs.
- Configures executable and log-file permissions.
- Starts the server.
- Launches 50 client sessions concurrently.
- Stores individual client outputs.
- Searches server logs using `grep` and regular expressions.
- Counts client connections and disconnections.
- Extracts calculation results from server logs.

## Performance Analysis
Execution times are measured across sequential, process-based, and thread-based implementations.

The project explores how dividing computational tasks among processes and threads affects execution time, resource usage, and synchronization.

## Project Information
- **Project Type:** Academic Project
- **Course:** Systems Programming
- **Semester:** Summer 2026- **University:** Al Hussein Technical University (HTU)

## License
© 2026 Mohammad Aldeghemat. All rights reserved.
