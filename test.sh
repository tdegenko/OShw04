#!/bin/bash
su Marry
./test&
$!>>marry_pid
exit
su Jane
./test&
$!>>jane_pid
exit
su Sam
./test&
$!>>sam_pid
exit
su Jim
./test&
$!>>jim_pid
exit
su Pat
./test&
$!>>pat_pid
exit

export marrypid=$(cat marry_pid)
export janepid=$(cat jane_pid)
export sampid=$(cat sam_pid)
export jimpid=$(cat jim_pid)
export patpid=$(cat pat_pid)

echo "===================================================\n"
     "=             testing time allocation             ="
     "==================================================="
echo "BEFORE SLEEP"

echo "MARRY:"
cat /proc/$marry_pid/sched
echo "JANE:"
cat /proc/$jane_pid/sched
echo "SAM:"
cat /proc/$sam_pid/sched
echo "JIM:"
cat /proc/$jim_pid/sched
echo "PAT:"
cat /proc/$pat_pid/sched
echo "SLEEPING"
sleep 2

echo "AFTER SLEEP"

echo "MARRY:"
cat /proc/$marry_pid/sched
echo "JANE:"
cat /proc/$jane_pid/sched
echo "SAM:"
cat /proc/$sam_pid/sched
echo "JIM:"
cat /proc/$jim_pid/sched
echo "PAT:"
cat /proc/$pat_pid/sched

echo "===================================================\n"
     "=         removing and adding proccess            ="
     "==================================================="
echo "Killing Marry's proccess (and starting an new one)."
su Marry
./test&
killall -9 test
$!>>marry_pid
exit

echo "===================================================\n"
     "=                 Testing preemption              ="
     "==================================================="
echo "BEFORE BUSSY WAIT"

echo "MARRY:"
cat /proc/$marry_pid/sched
echo "JANE:"
cat /proc/$jane_pid/sched
echo "SAM:"
cat /proc/$sam_pid/sched
echo "JIM:"
cat /proc/$jim_pid/sched
echo "PAT:"
cat /proc/$pat_pid/sched
echo "Running"
TIME=$(date +%S)
while [[ $TIME -eq $(date +%S) ]]; do
done

echo "After running"

echo "MARRY:"
cat /proc/$marry_pid/sched
echo "JANE:"
cat /proc/$jane_pid/sched
echo "SAM:"
cat /proc/$sam_pid/sched
echo "JIM:"
cat /proc/$jim_pid/sched
echo "PAT:"
cat /proc/$pat_pid/sched
echo "SLEEPING"
