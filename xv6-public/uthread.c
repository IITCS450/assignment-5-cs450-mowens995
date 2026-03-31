#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"



static struct uthread threads[MAX_THREADS];
static int current_tid;

void thread_init(void) {
    for (int i = 0; i < MAX_THREADS; i++) {
        threads[i].id = -1;
        threads[i].state = UNUSED;
        threads[i].stack = 0;
    }

    threads[0].id = 0;
    threads[0].state = RUNNING;

    current_tid = 0;
}

void thread_exit(void) {
    threads[current_tid].state = EXITED;
    thread_yield();
}

void thread_stub(void (*fn)(void*), void *arg) {
    fn(arg);
    thread_exit();
}

tid_t thread_create(void (*fn)(void*), void *arg) {
    tid_t tid = -1;
    for (int i = 1; i < MAX_THREADS; i++) {
        if(threads[i].state == UNUSED) {
            tid = i;
            break;
        }
    }
    if(tid < 0) return -1;

    void *stack = malloc(STACK_SIZE);
    if(!stack) return -1;

    uint *sp = (uint*)(stack + STACK_SIZE);
    *--sp = 0;
    *--sp = (uint)fn;
    *--sp = (uint)arg;

    threads[tid].ctx.eip = (uint)thread_stub;
    threads[tid].ctx.esp = (uint)sp;

    threads[tid].id = tid;
    threads[tid].stack = stack;
    threads[tid].state = RUNNABLE;

    return tid;
}

void thread_yield(void) {
    int old = current_tid;
    int next = -1;
    for (int i = 1; i < MAX_THREADS; i++) {
        int tid = (old + i) % MAX_THREADS;
        if(threads[tid].state == RUNNABLE) {
            next = tid;
            break;
        }
    }
    if(next < 0) return;

    if(threads[old].state == RUNNING)
        threads[old].state = RUNNABLE;
    threads[next].state = RUNNING;
    current_tid = next;

    struct context *oldctx = &threads[old].ctx;
    struct context *newctx = &threads[next].ctx;

    asm volatile(
        "movl %%esp, 4(%0)\n"
        "movl $1f, (%0)\n"
        "movl 4(%1), %%esp\n"
        "jmp *(%1)\n"
        "1:\n"
        :
        : "r"(oldctx), "r"(newctx)
        : "memory"
    );
}
int thread_join(tid_t tid) {
    if(tid < 0 || tid >= MAX_THREADS)
        return -1;
    
    while (threads[tid].state != EXITED) {
        thread_yield();
    }

    if(tid != 0 && threads[tid].stack) {
        free(threads[tid].stack);
        threads[tid].stack = 0;
    }

    return 0;
}
