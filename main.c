#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedule.h"

int main(void){
    char _policy[10];
    int proc_num;
    Process *proc;
    scanf("%s%d", _policy, &proc_num);
    proc = (Process *)malloc(sizeof(Process)*proc_num);
    for(int i = 0; i < proc_num; i++){
        scanf("%s%d%d",proc[i].name,&proc[i].ready_time,&proc[i].execution_time);
        proc[i].pid = -1;
        proc[i].id = i;
    }
    int policy;
    if(strcmp(_policy,"FIFO")==0)
        policy = FIFO;
    else if(strcmp(_policy,"RR")==0)
        policy = RR;
    else if(strcmp(_policy,"SJF")==0)
        policy = SJF;
    else if(strcmp(_policy,"PSJF")==0)
        policy = PSJF;
    else{
        fprintf(stderr,"Policy isn't exist.");
        exit(0);
    }
    schedule(proc,proc_num,policy);
}