#!/bin/bash
./test&
echo $!>$(whoami)_pid
