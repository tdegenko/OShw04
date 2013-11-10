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

./testmain $marrypid $janepid $sampid $jimpid $patpid

echo "Killing Marry's proccess (and starting an new one)."
su Marry
./test&
killall -9 test
$!>>marry_pid
exit

export marrypid=$(cat marry_pid)
./testmain $marrypid $janepid $sampid $jimpid $patpid
