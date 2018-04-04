# coolshell
a very minimal unix shell written in C


     ----- coolshell v1.0 user manual -----

Thanks for trying coolshell! You can start coolshell from any other shell by simply 
typing in "coolshell". It includes many built-in functions found in other shells, 
listed below:
   * cd       - change directory. Usage: "cd \<directory>"
   * clear    - prints newlines to clear the screen of previous commands and output.
   * help     - prints this dialog
   * environ  - prints the list of environment variables
   * echo     - prints to the screen. Usage: "echo \<statement>"
   * pause    - suspends the prompt until the Enter key is pressed
   * euphoric - prints a sarcastic motivational quotation
   * thanks   - give coolshell some love
   * quit     - exit the shell
   * exit     - exit the shell

An example prompt, command, and output are as follows:

  ```
  ~tuf08776 is coolguy~/home/TU/tuf08776>thanks
  cs~>No problem, buddy.
  ```

It also supports UNIX commands such as ls, mkdir, rm, cp, et cetera. Any programs 
on the system can also be run in the same manner, such as nano, emacs, or vim.

I/O redirection is supported in the following forms:
   * \<  - redirect input to a function from a file
   * \>  - redirect output from a function to a file
   * \>\> - append output from a function to a file
   * |  - pipe output of one function as the input of another function

For example, the following command uses input and output redirection:

  `~tuf08776 is coolguy~/home/TU/tuf08776>wc -l < input.txt > output.txt`

The command used here is wc, wordcount, and it will count the number of words in the
file called input.txt (located in the /tuf08776 directory). The output will be saved 
in the file called output.txt (which, if it does not exist, will be created).
Similarly, the append redirection will save the output without overwriting the data
that is already there. A pipe works in a similar fashion, however it links two 
commands by "piping" the output of the first command into the input of the second
command. 

coolshell also supports batch invocation. You can call it from the prompt like so:

  `~tuf08776 is coolguy~/home/TU/tuf08776>coolshell mybatch`

The batch file is a text file consisting of each line to be executed by the shell, 
separated by a single newline character (i.e. pressing Enter once at the end of the 
line). This is important because the batch interpreter is quite delicate. Superfluous 
newline characters will result in the batch ending early, much to your dissatisfaction.

Also included for statistical reasons is the source for a timer. Compile it like so:

  `~tuf08776 is coolguy~/home/TU/tuf08776>gcc -o Timer timer.c -lm`

Then it can be used to time coolshell's uptime. Call it when starting coolshell:

  `~tuf08776 is coolguy~/home/TU/tuf08776>Timer coolshell`

And it will tell you the total time coolshell was awake when you exit.
