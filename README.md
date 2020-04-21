# CS-111
# Operating Systems Class
#
  # Project 1a: 
   focuses on IPC using the C programming language. The running process forks a child process and then has it 
   run /bin/bash. A user can enter commands in the running terminal, then the parent process will take the input, send
   it to the child process who runs the input through a bash terminal, then sends the output back to the parent who then 
   prints it to the screen.
  # Project 1b:
   Very similar to project 1 but I added a client server communication over TCP. The client inputs data into the terminal, 
   sends it over a tcp socket to a server, and then the server does what is descriped in project 1. Instead of printing the      results to the server terminal, the server sends the /bin/bash results back to the client who then outputs it to their 
   local terminal. Compression can be enabled for the TCP communication.
  # Project 2a and 2b:
   This project focuses on concurrent programming. Each of these programs forks a specified number of threads and has them
   run over a critical section(linked list, linked list of linked list, addition) and a user can specify what type of 
   protection they want for this critical section(mutex, spin lock, test and set). All performance is recorded and can be 
   seen in the graphs that are produced. 
  # Project 3a
   This project takes in a file system image and then scans over it. After reading this file, the program will output data 
   regarding the format of the file system. For example, it prints out inode information, group information, data block
   information, relevant offsets, and any other information regarding the format of your filesystem
   (inode tables, block table, etc)
  # Project 3b
   This reads a output regarding a file system like one produced in Project 3a and says if there are any errors or corruption
   within the file system,
  # Project 4
   a: This project runs on a beaglebone. The program reads temp sensors from the board and outputs them to the screen.
   The user can send terminal commands to the program to specify things such as period of data collection, temp scale,
   start, stop, etc. 
   b: Same functionality as part a but instead of sending commands over the terminal, commands are sent over a TCP server.
   The beaglebone connects to a running TCP or TLS server and accepts input in the form of commands, and then sends
   temperature readings back.
#

0: 99
#
1a: 97
#
1b: 100
#
2a: 97
#
2b: 94.5
#
3a: 96
#
3b: 97
#
4b: 100
#
4c: 100
