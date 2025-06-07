# 345_exercises
OS obligatory course, Computer Science Department, University of Crete

Operating System Procedures and Implementation (Summary)

An Operating System (OS) manages hardware and software resources through various procedures:

    Multiprogramming enables multiple programs to reside in memory, improving CPU utilization by switching between tasks.

    Process communication uses mechanisms like synchronization and semaphores to coordinate concurrent process execution and prevent race conditions.

    Context switching saves and loads process states to switch the CPU from one process to another.

    Process scheduling decides the order in which processes access the CPU using algorithms (e.g., round-robin, priority).

    System calls allow user programs to request OS services like file access or memory allocation.

    Interrupt handlers manage signals from hardware/software, pausing and resuming processes as needed.

    Deadlock occurs when processes block each other indefinitely while waiting for resources; strategies include prevention, avoidance, and detection.

    Device-independent I/O abstracts hardware differences, allowing unified access to various devices.

    Device drivers are OS components that translate general I/O requests into device-specific operations.

    Disk scheduling algorithms (e.g., FCFS, SSTF, SCAN) optimize disk head movement to reduce latency.

    Memory management handles allocation and protection of memory spaces for processes.

    Virtual memory extends RAM using disk storage, allowing larger address spaces.

    Paging algorithms (like LRU, FIFO) manage how pages are loaded into memory to minimize page faults.

    File systems organize data into files/directories, managing storage, retrieval, and metadata.

    Distributed operating systems manage a group of networked computers as a single system, offering transparency in resource sharing and process execution.

Laboratory Exercises:
Students engage in hands-on labs that involve modifying a simple OS kernel (e.g., adding a new system call, implementing a basic scheduler, or simulating memory management), offering practical insights into OS internals and reinforcing theoretical concepts.
