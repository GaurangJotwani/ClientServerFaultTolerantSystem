#!/bin/bash

./fget MD hello
./fget PUT output.txt
./fget PUT test1.txt output.txt

mv disk2 disk2-out

./fget RM test1.txt
./fget PUT newfile.txt output.txt
./fget MD newfolder
./fget RM output.txt

mv disk2-out disk2
./fget MD newfolder/test
