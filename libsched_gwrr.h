/*
 * Group Weighted Round Robin Scheduler -
 *      header file wrapping system calls.
 */

#ifndef __LIBSCHED_GWRR_H
#define __LIBSCHED_GWRR_H

// We need to REdefine the syscall IDs we previously
// defined in unistd.h before compiling. Yay.
#define __NR_getgroupweight 327
#define __NR_setgroupweight 328

// Ditto for the SCHED_GWRR policy number.
// Apparently SCHED_RR, SCHED_FIFO, SCHED_OTHER are exposed
// to user space, while SCHED_NORMAL, SCHED_BATCH, SCHED_IDLE,
// and the new SCHED_GWRR aren't. Oddly SCHED_OTHER ~ SCHED_NORMAL.
// Not at all sure what's going on here... maybe something to
// do with gcc's library choices...?
#define SCHED_GWRR      6

static inline int getgroupweight(int gid) {
         return syscall(__NR_getgroupweight, gid);
}

static inline int setgroupweight(int gid, int weight) {
        return syscall(__NR_setgroupweight, gid, weight);
}

#endif

