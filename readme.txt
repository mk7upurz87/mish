mish
====

The mish shell is a simple interactive shell capable of executing simple UNIX commands and some internal commands specified later in this document.

Mish gets each line of input from the terminal console, formulates that input into a user command, and creates a child process to execute that command. The mish shell continues to process each line entered until specific events occur.

====

### Example

```c
# the user starts mish and issues commands following its prompt.
$ mish
mish[1]> date
Wed Jul 31 14:44:07 EDT 2013
mish[2]> echo hello there universe
hello there universe
mish[3]> cat header-c99.mak
CXXFLAGS =	-ggdb -Wall
CFLAGS =	-ggdb -Wall -std=c99
CLIBFLAGS =	-lm
CCLIBFLAGS =	
mish[4]> which grep
/bin/grep
mish[5]> 
$ 
# At prompt number 5 the user entered CONTROL-D.
The program terminated and returned the user to the terminal shell.
# This shows what happens when a user enters an unknown command.
$ mish
mish[1]> bogus
bogus: No such file or directory
command status: 1
mish[2]> 
$ 
# The attempt to execute bogus produced an error code.
# The shell detected and printed the error.
# Since the execution status was non-zero from the child process,
the shell printed the status it received.
# This example shows starting the shell in diagnostic, verbose mode.
$ mish 1
mish[1]> ls MissingFile
	command: ls MissingFile

	input command tokens:
	0: ls
	1: MissingFile
	wait for pid 1244: ls
	execvp: ls
ls: cannot access MissingFile: No such file or directory
command status: 2
mish[2]> ^D
$
```

### Processing External Commands

1. the mish shell runs an infinite loop waiting for user commands; the user may issue a CONTROL-D to tell the shell that there is no more input.
2. the prompt for each command contains its sequence number, 1, 2, and so on.
3. the shell gets a line of text containing commands and their arguments.
4. the shell parses the line into tokens.
5. parsing handles unnested, quoted arguments containing single or double quotes. (The input will not have unbalanced or unmatched quotation characters; the input of quotation marks will be balanced and matching in all tests.)
6. the shell detects and handles internal mish commands { eg: help } before the fork point.
7. the shell forks and execs the command entered by the user.
8. the shell waits for command completion. Note that there is more to do than simply call wait. The child may have exited normally or not. The specifics on return codes that wait returns are in the page man -s 2 wait, where you will find some additional functions that process the returned status based on whether the child exited or was terminated by a signal. These functions post-process the raw exit value to extract the most typical error codes.
9. the shell reports errors in execution (see Errors).
10. the shell shall provide a command line option for a verbose mode of operation. When invoked with a non-zero command line argument, the shell will display the command, the results of parsing a non-empty command, and the major operations that occur in processing the command. See the Examples.
11. the shell shall maintain a set of internal commands that it does not fork. When the mish shell recognizes an internal command name, it will execute a function that fulfills the functionality associated with that name.

### Included Internal Commands

* <strong>verbose on/off</strong>: turn the shell verbose on or off based on the argument passed to the verbose command. When verbose is on, the shell should print a table/list as shown in the examples. After the command executes, the verbose mode should print the execution status value returned to the shell by the command. This is also shown in the examples.
The output format should be a reasonable design of your own.
If the user enters something other than 'on' or 'off', the shell should print a usage message like this:
```
		usage: verbose [on|off]
```	
* <strong>help</strong>: print a list of internal commands. The output format should be a reasonable design of your own.
* <strong>history</strong>: print a list of commands executed so far, including their arguments. The history list should be numbered starting from the value 1. The output format should be a reasonable design of your own.
The history command should display only the last 10 commands that were entered. The numbering may be either always 1 to 10 or values such as the pattern (N-10), (N-9), ... (N), where N is the number of the history command just entered.
* <strong>quit</strong>: clean up memory and gracefully terminate mish.

### Known Bugs

* Ctrl+D was still throwing Segmentation Fault, currently not handled and is commented out of code
* Verbose off command when run repeatedly will trigger a memory corruption
* Will record history, however the results for verbose off are not as expected
* Does not properly tokenize before segmentation fault
* Because tokenizing the command line does not work the custom commands could not be implemented
* The application supports all of the main functions and has ground work in place to get custom commands in
* Does not fork separate processes
* Does not yet act as a multithreaded application
* Does quit gracefully!

### Known improvements

* Strip the newline character from all commands rather than handling them as is
* Tokenize the strings earlier on and place into a Command to be handled universally
* Enumerate the command names
* Utilize constants
* Make better use of perror and respective error outputs