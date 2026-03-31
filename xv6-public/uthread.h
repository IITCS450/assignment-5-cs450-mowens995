#pragma once
#include "types.h"

#define MAX_THREADS 8
#define STACK_SIZE 4096

enum thread_state {
    UNUSED,
    RUNNABLE,
    RUNNING,
    EXITED
};

struct context {
    uint eip;
    uint esp;
};

struct uthread {
    int id;
    int state;
    void *stack;
    struct context ctx;
};

typedef int tid_t;
void thread_init(void);
tid_t thread_create(void (*fn)(void*), void *arg);
void thread_yield(void);
int thread_join(tid_t tid);
