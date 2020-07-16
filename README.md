# shell-implementation
For Unix/Linux

What it does:
1)Built-in Commands
The shell inteprets the following built-in commands:
 cd: changes the current working directory
 pwd: prints the current working directory
 echo: prints a message and the values of environment variables
 exit: terminates the shell
 env: prints the current values of the environment variables
 setenv: sets an environment variable


2)Processes
The shell takes a command, checks if it is not a built-in command, forks a process, loads the program from the file system, passes the arguments, and executes it.

3)Background Processes
Processes can be run on the background by appending a '&' at the end of a command line.

4)Signal Handling
When depressing ctrl-C, the shell returns to the prompt.
