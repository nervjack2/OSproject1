#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include <sys/syscall.h>
#include <errno.h>
#include "scheduler.h"
#define MAXINT 2147483647

int time;
int exit_proc;
int running;
int prerunning;
// last context switch
int last_cs;
int RRqueue[1000];
int head = 0, tail = 0;

int qsortcmp(const void *a, const void *b){
    Process *p1 = (Process *)a;
    Process *p2 = (Process *)b;
    return p1->ready > p2->ready;
}

void inqueue(int idx){
    RRqueue[tail] = idx;
    tail = (tail+1)%1000;
    return;
}
int dequeue(void){
    int temp = head;
    head = (head+1)%1000;
    return RRqueue[temp];
}

int sched_next_FIFO(Process *proc, int procnum){
    int nextid = -1;
    if(running == -1){
        for(int i = 0; i < procnum; i++)
            if(proc[i].pid != -1 && proc[i].remain != 0){
                nextid = i;
                break;
            }
    }
    else
        nextid = running;
    return nextid;
}

int sched_next_RR(Process *proc, int procnum){
    int nextid = -1;
    if((time-last_cs)%500==0 || running == -1){
        if(head == tail)
            nextid = running;
        else
            nextid = dequeue();
    }
    else
        nextid = running;
    return nextid;
}

int sched_next_SJF(Process *proc, int procnum){
    int nextid = -1;
    if(running == -1){
        int minid = -1, min = MAXINT;
        for(int i = 0; i < procnum; i++){
            if(proc[i].pid == -1 || proc[i].remain == 0)
                continue;
            if(proc[i].remain < min){
                minid = i;
                min = proc[i].remain;
            }
        }
        nextid = minid;
    }
    else
        nextid = running;
    return nextid;
}

int sched_next_PSJF(Process *proc, int procnum){
    int nextid = -1;
    int minid = -1, min = MAXINT;
    for(int i = 0; i < procnum; i++){
        if(proc[i].pid == -1 || proc[i].remain == 0)
            continue;
        if(proc[i].remain < min){
            minid = i;
            min = proc[i].remain;
        }         
    }
    nextid = minid;
    return nextid;
}

void sched_scheduling(Process *proc, int procnum, int policy){
    // sort process by their ready time
    qsort(proc, procnum, sizeof(Process), qsortcmp);
    // assign cpu to scheduler 
    proc_cpuassign(getpid(),0);
    proc_priorup(getpid());
    time = 0, exit_proc = 0, running = -1;
    prerunning = -1;
    while(1){
        if(time % 100 == 0)
            fprintf(stderr,"%d %d\n",time,running);
        // time t~t+1
        if(running != -1){
            if(proc[running].remain == 0){
                proc_priorup(proc[running].pid);
                //fprintf(stderr,"%d %d\n",time,running);
                exit_proc += 1;
                // suspend until the child terminate
                if(waitpid(proc[running].pid,NULL,0) < 0)
                    fprintf(stderr,"Wait Error.\n");
                running = -1;
            }
        }
        if(exit_proc == procnum)
            break;
        // find t+1 executing process
        // check if any process is ready at time t+1
        for(int i = 0; i < procnum; i++)
            if(proc[i].ready == time){
                proc[i].pid = proc_exec(proc[i]);
                inqueue(i);
            }
        // find next executing process
        int nextidx;
        if(policy == FIFO)
            nextidx = sched_next_FIFO(proc,procnum);
        else if(policy == RR)
            nextidx = sched_next_RR(proc,procnum);
        else if(policy == SJF)
            nextidx = sched_next_SJF(proc,procnum);
        else if(policy == PSJF)
            nextidx = sched_next_PSJF(proc,procnum);

        if(nextidx != -1 && nextidx != running){
            last_cs = time;
            proc_priorup(proc[nextidx].pid);
            if(running != -1){
                proc_priordown(proc[running].pid);
                prerunning = running;
            }
        }
        if(running != -1 && nextidx != running && proc[running].remain != 0)
            inqueue(running);
        running = nextidx;
        Unittime();
        if(running != -1)
            proc[running].remain -= 1;
        time += 1;
    }   
    return;
}

void proc_cpuassign(pid_t pid, int cpuid){
    cpu_set_t cpumask;
    CPU_ZERO(&cpumask);
    CPU_SET(cpuid,&cpumask);
    //fprintf(stderr,"%d\n",pid);
    if(sched_setaffinity(pid,sizeof(cpumask),&cpumask)==-1){
        perror("sched_setaffinity");
        //fprintf(stderr,"Set cpu affinity error.\n");
        exit(0);
    }
    return;
}

void proc_priorup(pid_t pid){
    struct sched_param mask;
    mask.sched_priority = 0;
    if(sched_setscheduler(pid,SCHED_OTHER,&mask) < 0){
        perror("sched_setscheduler");
        //fprintf(stderr,"Prior up error.\n");
        exit(0);
    }
    return;
}
void proc_priordown(pid_t pid){
    struct sched_param mask;
    mask.sched_priority = 0;
    if(sched_setscheduler(pid,SCHED_IDLE,&mask) < 0){
        perror("sched_setscheduler");
        //fprintf(stderr,"Prior down error.\n");
        exit(0);
    }
    return;
}
pid_t proc_exec(Process P){
    int childpid = fork();
    if(childpid == 0){
        unsigned long long int start, end;
        start = syscall(334);
        for(int i = 0; i < P.remain; i++)
            Unittime();
        end = syscall(334);
        char mesg[100];
        sprintf(mesg,"%d %llu.%09llu %llu.%09llu\n", getpid(), start/1000000000, start%1000000000, end/1000000000, end%1000000000);
        //fprintf(stderr,"%d %d\n",time,getpid());
        syscall(335,mesg);
        exit(0);
    }
    proc_cpuassign(childpid,1);
    proc_priordown(childpid);
    return childpid;
}