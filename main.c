#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scheduler.h"

int map_policy(char *policy){
    if(strcmp(policy,"FIFO") == 0)
        return FIFO;
    else if(strcmp(policy,"RR") == 0)
        return RR;
    else if(strcmp(policy,"SJF") == 0)
        return SJF;
    else if(strcmp(policy,"PSJF") == 0)
        return PSJF;
    else{
        fprintf(stderr,"Policy name can't be found.\n");
        exit(0);
    }
}   


int main(void){
    char sched_policy[10];
    int N;
    scanf("%s%d",sched_policy,&N);
    Process proc[N]; 
    for(int i = 0; i < N; i++){
        scanf("%s%lu%lu",proc[i].name, &proc[i].ready, &proc[i].remain);
        proc[i].pid = -1;
        proc[i].idx = i;
    }
    int policy = map_policy(sched_policy);
    sched_scheduling(proc, N, policy);
   // printf("hi\n";
    for(int i = 0; i < N; i++)
        fprintf(stdout,"%s %d\n",proc[i].name,proc[i].pid);
    fflush(stdout);
}