Homework 1
Khoa Nguyen
14295146

How To Compile & Run
====================
gcc -o pipes pipes.c
-ls
> ex1  hello.txt  main  pipes  pipes.c
cat hello.txt
> khoa nguyen
./pipes < hello.txt
> KHOA NGUYEN


Description
===========
[Brief description of file(s)/program.]
I have 2 pipes:
p_parent (pipe): parent write to child, child recieve data from parent
p_child (pipe): child write to parent, parent recieve data back from child
In parent process, we create a while loop to read each byte from STDIN_FILENO until EOF. Then sending it to child through a p_parent pipe, then recieving it back from child through p_child, and print it to screen.
In child process, we create a while loop as long as have data go through p_parent pipes. Flip character to uppercase, and then send them back to parent through p_child pipes.