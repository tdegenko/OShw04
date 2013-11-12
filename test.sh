#!/bin/bash
echo "MARY:"
su -c "./usertest.sh" -m Mary
echo "JANE:"
su -c "./usertest.sh" -m Jane
echo "SAM:"
su -c "./usertest.sh" -m Sam
echo "JIM:"
su -c "./usertest.sh" -m Jim
echo "PAT:"
su -c "./usertest.sh" -m Pat

./changeweight

echo "CS2456:"

export mary_pid=$(cat Mary_pid)
export jane_pid=$(cat Jane_pid)
export sam_pid=$(cat Sam_pid)
export jim_pid=$(cat Jim_pid)
export pat_pid=$(cat Pat_pid)

echo "==============================="
echo "=  testing time allocation    ="
echo "==============================="
echo "BEFORE SLEEP"

echo "MARY:"
ps -p $mary_pid -o cputime=
echo "JANE:"
ps -p $jane_pid -o cputime=
echo "SAM:"
ps -p $sam_pid -o cputime=
echo "JIM:"
ps -p $jim_pid -o cputime=
echo "PAT:"
ps -p $pat_pid -o cputime=
echo "SLEEPING"
sleep 30

echo "AFTER SLEEP"

echo "MARY:"
ps -p $mary_pid -o cputime=
echo "JANE:"
ps -p $jane_pid -o cputime=
echo "SAM:"
ps -p $sam_pid -o cputime=
echo "JIM:"
ps -p $jim_pid -o cputime=
echo "PAT:"
ps -p $pat_pid -o cputime=

#echo "==========================\n"
#     "=         removing and adding proccess            ="
#     "=========================="
#echo "Killing Marry's proccess (and starting an new one)."
#su Marry
#su -c "./usertest.sh" -m Mary./test&
#killall -9 test
#$!>>mary_pid
#exit
#
#echo "==========================\n"
#echo "=                 Testing preemption              ="
#echo "=========================="
#echo "BEFORE BUSSY WAIT"
#
#echo "MARY:"
#ps -p $mary_pid -o cputime=
#echo "JANE:"
#ps -p $jane_pid -o cputime=
#echo "SAM:"
#ps -p $sam_pid -o cputime=
#echo "JIM:"
#ps -p $jim_pid -o cputime=
#echo "PAT:"
#ps -p $pat_pid -o cputime=
#echo "Running"
#TIME=$(date +%S)
#while [[ $TIME -eq $(date +%S) ]]; do
#done
#
#echo "After running"
#
#echo "MARY:"
#ps -p $mary_pid -o cputime=
#echo "JANE:"
#ps -p $jane_pid -o cputime=
#echo "SAM:"
#ps -p $sam_pid -o cputime=
#echo "JIM:"
#ps -p $jim_pid -o cputime=
#echo "PAT:"
#ps -p $pat_pid -o cputime=
#
































































































