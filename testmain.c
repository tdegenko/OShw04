#include <stdio.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <sched.h>
#include "libsched_gwrr.h"


//GID's  change once we know what they will be.

#define MJGID 1008
#define SJGID 1007
#define PGID 1005


int main(int argc, char *argv[]){
    setgroupweight(MJGID,30);
    setgroupweight(SJGID,10);
    setgroupweight(PGID,10);
    printf("Group weights set to:\nM/J:\t%d\nS/J:\t%d\nP:\t%d\n", getgroupweight(MJGID),getgroupweight(SJGID),getgroupweight(PGID));
    return 0;
}

