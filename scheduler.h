#include <stdio.h>
#include <unistd.h>

#define FIFO 1
#define RR 2
#define SJF 3
#define PSJF 4

// using macro function to avoid syncronization problem
#define Unittime()                              \
{                                               \
    volatile unsigned long i;                   \
    for(i = 0;i < 1000000UL;i++);               \
}                                               \

typedef struct process{
    char name[40];
    unsigned long int ready;
    unsigned long int remain;
    int idx;
    pid_t pid;
}Process;

void proc_priorup(pid_t pid);
void proc_priordown(pid_t pid);
void proc_cpuassign(pid_t pid, int cpuid);
pid_t proc_exec(Process P);
void sched_scheduling(Process *proc, int procnum, int policy);