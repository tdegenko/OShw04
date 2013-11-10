#include <stdio.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <sys/syscall.h>
#include <sched.h>

#define _CS2456_SETGROUPWEIGHT_ 327
#define _CS2456_GETGROUPWEIGHT_ 328

//REMOVE THIS ONCE WE HAVE THINGS IMPLEMENTED

#define SCHED_GWRR 4

//GID's  change once we know what they will be.

#define MJGID 1003
#define SJGID 1004
#define PGID 1005


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

    // do something to ensure that no non GWRR tasks are running

    //get pids of other proccessses.
    int marrypid = atoi(argv[0]);
    int janepid = atoi(argv[1]); 
    int sampid  = atoi(argv[2]);
    int jimpid  = atoi(argv[3]);
    int patpid  = atoi(argv[4]);
    
    setgroupweight(MJGID,30);
    setgroupweight(SJGID,10);
    setgroupweight(PGID,10);
    printf("Group weights set to:\nM/J:\t%d\nS/J:\t%d\nP:\t%d\n", getgroupweight(MJGID),getgroupweight(SJGID),getgroupweight(PGID));

    //Get unitial runtimes.
    int mitime,jaitime,sitime,jiitime,pitime;
    mitime  = task_sched_runtime(find_task_by_pid(marrypid));
    jaitime = task_sched_runtime(find_task_by_pid(janepid));
    sitime  = task_sched_runtime(find_task_by_pid(sampid));
    jiitime = task_sched_runtime(find_task_by_pid(jimpid));
    pitime  = task_sched_runtime(find_task_by_pid(patpid));
    //sleep to let scheduler do it's thing and schedule
    sleep(1);
    //upon wakeup get new times.
    int metime,jaetime,setime,jietime,petime;
    metime  = task_sched_runtime(find_task_by_pid(marrypid));
    jaetime = task_sched_runtime(find_task_by_pid(janepid));
    setime  = task_sched_runtime(find_task_by_pid(sampid));
    jietime = task_sched_runtime(find_task_by_pid(jimpid));
    petime  = task_sched_runtime(find_task_by_pid(patpid));
    //calculate time spent in each group:
    int mjtime=(metime-mitime)+(jaetime-jaitime);
    int sjtime=(setime-sitime)+(jietime-jiitime);
    int ptime=(ptime-ptime);

    printf("Marry/Jane:\t%d\nSam/Jim:\t%d\nPat:\t%d",mjtime,sjtime,ptime);
    int cpid=fork();
    int wait=1000;
    switch(cpid){
        case 0:
            //child stuff.
            //sched_setscheduler(getpid(),SCHED_NORMAL,NULL);
            while(wait>0){
                wait--;
            }
        case -1:
            //Error Stuff
            return 1;
        default:
            //Parrent stuff
            //Get unitial runtimes.
            mitime  = task_sched_runtime(find_task_by_pid(marrypid));
            jaitime = task_sched_runtime(find_task_by_pidd(janepid));
            sitime  = task_sched_runtime(find_task_by_pid(sampid));
            jiitime = task_sched_runtime(find_task_by_pid(jimpid));
            pitime  = task_sched_runtime(find_task_by_pid(patpid));
            //wait to let scheduler do it's thing and schedule
            waitpid(cpid,NULL,1);
            //Get times run.
            metime  = task_sched_runtime(find_task_by_pid(marrypid));
            jaetime = task_sched_runtime(find_task_by_pid(janepid));
            setime  = task_sched_runtime(find_task_by_pid(sampid));
            jietime = task_sched_runtime(find_task_by_pid(jimpid));
            petime  = task_sched_runtime(find_task_by_pid(patpid));
            //calculate time spent in each group:
            mjtime=(metime-mitime)+(jaetime-jaitime);
            sjtime=(setime-sitime)+(jietime-jiitime);
            ptime=(ptime-ptime);

            printf("Marry/Jane:\t%d\nSam/Jim:\t%d\nPat:\t%d",mjtime,sjtime,ptime);

 
    }


    return 0;
}

int setgroupweight(int gid,int weight){
    syscall(_CS2456_SETGROUPWEIGHT_ , gid, weight);
}
int getgroupweight(int gid){
    syscall(_CS2456_GETGROUPWEIGHT_ , gid);
}
