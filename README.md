mish
====

The mish shell is a simple interactive shell capable of executing simple UNIX commands and some internal commands specified later in this document.

Mish gets each line of input from the terminal console, formulates that input into a user command, and creates a child process to execute that command. The mish shell continues to process each line entered until any one of these events occur:

the shell encounters an error reading input;

the shell encounters the end of input (e.g. file input);

the user issues the command to terminate the shell; or

the user signals the shell that there is no more input.