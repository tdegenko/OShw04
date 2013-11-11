#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <sched.h>
#include "libsched_gwrr.h"

int main(int argc, char *argv[]){
    printf("\nDiving to kernel level\n\n");
    //    syscall(_CS2456_TEST_ , 2456);
    //    printf("\nRising to user level\n\n");

    // Fork of childeren
    //    switch(fork()){
    //        case 0:
    //            //do Child stuff
    //        case -1:
    //            //fork error
    //            return -1
    //        default:
    //            //do parrent stuff
    //    }
    struct sched_param sp = { .sched_priority = 50 };
    int ret=sched_setscheduler(0,SCHED_GWRR,&sp);
    if(ret==-1){
        perror("Error:");
        return -1;
    }
    printf("scheduler changed!\n");
    while(1){
    }
    return 0;
}

