Homework 1
Khoa Nguyen
14295146

How To Compile & Run
====================
$ gcc -o execlp execlp.c
$ ./execlp
-rwxr-xr-x 1 runner runner 8.4K Jan 10 20:07 execlp
-rw-r--r-- 1 runner runner  485 Jan 10 01:43 execlp.c
-rwxr-xr-x 1 runner runner 8.4K Jan 10 20:07 execvp
-rw-r--r-- 1 runner runner  532 Jan 10 01:53 execvp.c
-rwxr-xr-x 1 runner runner 8.3K Jan 10 01:17 main
Child exited with return code: 0
$ gcc -o execvp execvp.c
$ ./execvp
total 44K
-rwxr-xr-x 1 runner runner 8.4K Jan 10 01:53 execlp
-rw-r--r-- 1 runner runner  485 Jan 10 01:43 execlp.c
-rwxr-xr-x 1 runner runner 8.4K Jan 10 20:07 execvp
-rw-r--r-- 1 runner runner  532 Jan 10 01:53 execvp.c
-rwxr-xr-x 1 runner runner 8.3K Jan 10 01:17 main
Child exited with return code: 0


Description
===========
[Brief description of file(s)/program.]
I use vfork to create a child program, so child will get schedule first after work
1/ The program uses execvp to search the $PATH for a file named "ls" and passing "-l" and "-h" as argument to new program
2/ The program uses execlp functions to duplicate the action of the shell in searching for an executable file.