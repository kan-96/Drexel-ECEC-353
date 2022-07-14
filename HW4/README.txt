Homework 1
Khoa Nguyen
14295146

How To Compile & Run
====================
gcc -o signal signal.c
>./signal -l 
 1: SIGHangup (Hangup).
 2: SIGInterrupt (Interrupt).
 3: SIGQuit (Quit).
 4: SIGIllegal instruction (Illegal instruction).
 5: SIGTrace/breakpoint trap (Trace/breakpoint trap).
 6: SIGAborted (Aborted).
 7: SIGBus error (Bus error).
 8: SIGFloating point exception (Floating point exception).
 9: SIGKilled (Killed).
10: SIGUser defined signal 1 (User defined signal 1).
11: SIGSegmentation fault (Segmentation fault).
12: SIGUser defined signal 2 (User defined signal 2).
13: SIGBroken pipe (Broken pipe).
14: SIGAlarm clock (Alarm clock).
15: SIGTerminated (Terminated).
16: SIGStack fault (Stack fault).
17: SIGChild exited (Child exited).
18: SIGContinued (Continued).
19: SIGStopped (signal) (Stopped (signal)).
20: SIGStopped (Stopped).
21: SIGStopped (tty input) (Stopped (tty input)).
22: SIGStopped (tty output) (Stopped (tty output)).
23: SIGUrgent I/O condition (Urgent I/O condition).
24: SIGCPU time limit exceeded (CPU time limit exceeded).
25: SIGFile size limit exceeded (File size limit exceeded).
26: SIGVirtual timer expired (Virtual timer expired).
27: SIGProfiling timer expired (Profiling timer expired).
28: SIGWindow changed (Window changed).
29: SIGI/O possible (I/O possible).
30: SIGPower failure (Power failure).
31: SIGBad system call (Bad system call).
32: SIG(null) (Unknown signal 32).
33: SIG(null) (Unknown signal 33).
>ps -a
    PID TTY          TIME CMD
     85 pts/0    00:00:00 ps
./signal -s 0 86
PID 86 exists and is able to recieve signals
./signal -s 9 86


Description
===========
[Brief description of file(s)/program.]
First,we check option od user's input through argv[1]
if input 's' convert string to integer, then use int kill() function
if input 'l', use for loop to print all signal name by sys_siglist, and strsignal
Last, we use errno to check valid of PID