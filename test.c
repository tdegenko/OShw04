#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <sched.h>
#include "libsched_gwrr.h"
#include <sys/times.h> 

int main(int argc, char *argv[]){
    struct sched_param * sp=malloc(sizeof(struct sched_param));
    int ret=sched_getparam(0,sp);
    if(ret==-1){
        perror("Error:");
        return -1;
    }
    
    pid_t pid=getpid();
    ret=sched_setscheduler(0,SCHED_GWRR,sp);
    if(ret==-1){
        perror("Error:");
        return -1;
    }
    printf("scheduler changed!\n");
    long count=0;
    struct tms time;
    while(1){
        times(&time);
        printf("PID: %d, 
time:%d\n",pid,time.tms_utime+time.tms_stime);
    }
                              
                                     
    return 0;
}

