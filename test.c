#include <stdio.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <sched.h>

#define _CS2456_SETGROUPWEIGHT_ 327
#define _CS2456_GETGROUPWEIGHT_ 328

//REMOVE THIS ONCE WE HAVE THINGS IMPLEMENTED

#define SCHED_GWRR 4

int main(int argc, char *argv[]){
//    printf("\nDiving to kernel level\n\n");
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
    //Set policy to GWRR
    sched_setscheduler(getpid(),SCHED_GWRR,NULL);
    while(true){
        //bussy wait to get measurements.
    }
      
       
    return 0;
}
