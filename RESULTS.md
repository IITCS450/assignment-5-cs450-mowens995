Context switching approach:
    My uthread.c has a global "threads" array that contains MAX_THREADS (8) uthread structs. Each struct consists of the thread id, the state, the stack and a structure containing the instruction and stack pointers.
    Upon initialization, all threads (save 0) are set to the UNUSED state while thread 0 is set as the active thread.
    thread_create finds the next unused thread and allocates its values. It initializes the stack with a dummy address, the function and the argument addresses. Finally it utilizes a wrapper function "thread_stub" to save as the instruction pointer and returns the thread id.
    thread_stub runs the function on the argument(s) then calls thread_exit which sets the thread state to EXITED then yields.
    thread_yield finds the next runnable state, saves off the old instruction/stack pointers and loads the new ones. If no runnable state is found it returns.
    thread_join utilizes a while loop to yield until the referenced thread has exited.
Limitations:
    maximum threads: 8
    stack size: 4096