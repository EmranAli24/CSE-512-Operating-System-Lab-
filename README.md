# CSE 512 ( Operating System Lab)

## Overview

This repository contains the implementation of 20 practical experiments from the Computer Operating System curriculum (CSE 512: Operating System Lab). These experiments focus on critical aspects of operating systems, ranging from UNIX/Linux fundamentals to advanced topics such as process control, IPC (Inter-Process Communication), synchronization, CPU scheduling, memory management, and disk scheduling. This repository aligns with university OS lab manuals and provides comprehensive coverage of practical topics in operating systems.

---

## Table of Contents

1. **UNIX Permission and umask Calculator**  
2. **POSIX File Copy with open/read/write**  
3. **Directory Listing and Metadata Report (`ls` + `stat` subset)**  
4. **grep-lite: Deterministic Text Pattern Search**  
5. **Process Spawner and Exit-Status Reporter (`fork/exec/wait`)**  
6. **Signal-Based Timeout Supervisor (`sigaction + alarm + kill`)**  
7. **Pipe-Based Filter Chain (`pipe + dup2`)**  
8. **Shared Memory Counter IPC (`shm_open + mmap + sem_open`)**  
9. **Threaded Deterministic Reducer (`pthread + mutex`)**  
10. **Bounded Buffer Producer-Consumer with Semaphores (deterministic summary)**  
11. **CPU Scheduling Simulator I (FCFS and Non-preemptive SJF)**  
12. **CPU Scheduling Simulator II (Round Robin)**  
13. **Priority Scheduling Simulator (Non-preemptive with Aging)**  
14. **Deadlock Avoidance using Banker's Algorithm**  
15. **Deadlock Detection via Wait-For Graph Cycle**  
16. **Contiguous Memory Allocation Simulator (First/Best/Worst Fit)**  
17. **Paging Address Translation with Optimal TLB**  
18. **Page Replacement Simulator (FIFO, LRU, OPT)**  
19. **File Allocation Strategy Simulator (Contiguous, Linked, Indexed)**  
20. **Disk Scheduling Simulator (FCFS, SSTF, SCAN, C-SCAN)**  

---

## Description

### Highlights:
- **UNIX/Linux Interfaces**
    - Understanding file permissions, directories, system calls, and umask.
    - Creating and copying files using POSIX standards (`open/read/write`).
- **Process Control**
    - Spawning processes and managing exit statuses using `fork/exec/wait`.
    - Supervising processes with signals (`sigaction`).
    - Creating inter-process communications pipelines.
- **Synchronization**
    - Using shared memory (`shm_open`), semaphores, mutexes, and threads (`pthread`).
    - Implementing deterministic Reduce workflows and classic producer-consumer patterns.
- **CPU Scheduling**
    - Simulating FCFS, SJF, Round Robin, and Priority scheduling algorithms.
- **Deadlock**
    - Understanding deadlock detection (Wait-For Graph) and avoidance (Banker’s Algorithm).
- **Memory Management**
    - Exploring contiguous and paging allocation strategies.
    - Implementing TLB for address translations and page replacement algorithms.
- **Disk Scheduling**
    - Simulating disk operations like FCFS, SSTF, SCAN, and C-SCAN.

---

## Resources

- **Code:** The source code for all experiments is written in **C language**.
- **Documentation:** Detailed explanations and observations are provided across individual experiment files.
- **University Manuals:** Conforms to standard OS lab syllabi.

---

## How to Use

1. Clone this repository:
   ```bash
   git clone https://github.com/EmranAli24/CSE-512-Operating-System-Lab.git
   ```
2. Navigate to the desired experiment directory.
3. Compile the C program:
   ```bash
   gcc -o output_file experiment_file.c
   ```
4. Run the executable:
   ```bash
   ./output_file
   ```

---

## License

This repository is licensed under [MIT License](LICENSE).

## Acknowledgments

- CSE 512 Laboratory Syllabus
- Operating System Lab Manuals
