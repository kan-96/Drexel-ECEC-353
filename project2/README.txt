Project 1
Khoa Nguyen
14295146

How To Compile & Run
====================
[kn574@xunil-03 ~]$ make
gcc -g -Wall -c builtin.c -o builtin.o
gcc -g -Wall -c pssh.c -o pssh.o
gcc  builtin.o  pssh.o  parse.o -Wall -lreadline -o pssh
[kn574@xunil-03 ~]$ ./pssh
                    ________
_________________________  /_
___  __ \_  ___/_  ___/_  __ \
__  /_/ /(__  )_(__  )_  / / /
_  .___//____/ /____/ /_/ /_/
/_/ Type 'exit' or ctrl+c to quit
/home/kn574@drexel.edu$ which ls
/usr/bin/ls
/home/kn574@drexel.edu$ which man
/usr/bin/man
/home/kn574@drexel.edu$ which man > test17.txt
/home/kn574@drexel.edu$ cat test17.txt
/usr/bin/man


Description
===========
[Brief description of file(s)/program.]
