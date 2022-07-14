Homework 1
Khoa Nguyen
14295146

How To Compile & Run
====================
gcc -o hw3 mmap.c
> Child: value = 1
>Parent: value = 2


Description
===========
[Brief description of file(s)/program.]
First, we open /dev/zero file with read and write
then we call mmap to mapping between process address space and the file
Then we close /dev/zero file
Next, we call fork to create a child, and process the value of *add, and pass it to parent process, then call munmap
The parent process recieve a new value of *addr, then call munmap