/**
 * filename: mish.c
 * author: sdd7427 (Shaun DeVos)
 * description:	 The Mish shell is a simple interactive shell
 *		capable of executing simple UNIX commands and some
 *		internal commands.
 *
 * date: 05/10/1214
 */

#define _GNU_SOURCE

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "mish.h"


/**
 * Global variables
 */

int verbose_mode = 0;
char* line_read = NULL;
COMMAND cmd_tokens;
COMMAND hist[10];
int history_length;
time_t raw_time;

/**
 * Take in the user input and verify.  Then execute the corresponding command
 */
int main( int argc, char* argv[] ) {

    int cmd_num = 1;
    size_t nbytes = 0;
    int bytes_read;
    history_length = 0;

    while( 1 ) {
        printf ( "mish[%d]> ", cmd_num );

        // If the user enters a 1 as a command line argument
        // start in verbose mode
        if( argc == 2 && atoi(argv[1]) == 1) {
            verbose_mode = 1;
        }

        line_read = ( char* )malloc( sizeof(char) );
        bytes_read = getline( &line_read, &nbytes, stdin );

        if( bytes_read == 1 ) {
            continue;
        } else {
            cmd_num++;
        }

        char* token = strtok( line_read, " " );
        char* option;

        // If the user enters CTRL+D command, quit the application gracefully
        // if(token[0] == EOF ) {
        //     token = "quit\n";
        //     printf("quitting...");
        // }

        if(strcmp(token, "verbose\n") == 0 || strcmp(token, "verbose") == 0) {

            option = strtok( NULL, " " );
            if(option != NULL) {
                
                char* cmd = "verbose\0";
                add_to_history( cmd, cmd_num, (char *[]){ option });

                char* opts[] = { cmd, option };
                verbose(2, opts);

            } else {
                printf("usage: verbose [on|off]\n");
            }
        }
        else if(strcmp(token, "help\n") == 0) {
            char* cmd = "help\0";
            add_to_history( cmd, cmd_num, NULL );

            char* opts[] = { cmd };
            help(1, opts);
        }
        else if(strcmp(token, "history\n") == 0) {
            char* cmd = "history\0";
            add_to_history( cmd, cmd_num, NULL );

            char* opts[] = { cmd };
            history(1, opts);
        }
        else if(strcmp(token, "quit\n") == 0) {
            char* opts[] = { "quit" };

            // only close if all memory cleared properly
            if(quit(1, opts) == 1) {
                return 0;
            }
        } else {
            cmd_tokens = str_to_cmd( line_read );
            
            char* cmd = cmd_tokens.name;
            // char* options[] = { cmd_tokens.args[0] };

            // Output the command and arguments being executed in a new thread
            printf("\tcommand: %s", line_read ? line_read : "\n");
            printf("\tinput command tokens:\n");
            printf("\t[0]: %s", cmd ? cmd : "\n");
            // if(options != NULL && options[0] != NULL) {
            //     for(int i = 0; options[i]; i++) {
            //         // Output the arguments, i + 1 because the cmd was the first arg
            //         printf("\t[%d]: %s\n", i + 1, options[i]);
            //     }
            // }
            // Spawn new thread here and execute corresponding command
            // int result = customCommand(cmd_tokens);
            // if(result == 0) {
            //     perror("%d")
            // }
            // else {
            //     add_to_history( cmd, cmd_num, options );
            // }   
        }
    }
}

/**
 * using the quotation character as a marker for the start of arguments,
 * create an array of arguments.
 */
COMMAND str_to_cmd( char* str ) {

    // char **arg_tokens = NULL;
    char *curr_token = strtok( str, " " );
    char *curr_time = (char *)localtime(&raw_time);

    //     switch(curr_token[0]) { // the real value
    //         case '\0'
    //             // code for arrow up
    //             break;
    //     }

    COMMAND cmd = { curr_token, curr_time, 0, (void *)&("\0") };
    printf("%s: No such file or directory\n", cmd.name);
    char* long_token = NULL;
    char* new_str = NULL;
    int between_quotes = 0;
    char delim = '\"';


    // walk through other tokens
    while( curr_token != NULL ) {
        // the token has quotes so begin storing what follows as a single token

        if(contains(curr_token, delim)) {
            // results
            // printf("token: %s has quotes...\n", token);

            if(!between_quotes) {
                // we're now between quotes so begin storing as a large token
                // printf("entering quotes = %s\n", token);
                curr_token = remove_symbol( curr_token, delim);
                // printf("removed quotes = %s\n", token);

                long_token = malloc(sizeof(char) *
                    (strlen(long_token) + strlen(curr_token)));
                long_token = curr_token;
                between_quotes = 1;

                // printf("long_token = %s\n", long_token);
            } else {
                // we're already storing a large token so stop it
                // printf( "leaving quotes = %s\n", token );
                curr_token = remove_symbol( curr_token, delim);
                // printf("removed quotes = %s\n", token);

                new_str = malloc(sizeof(char) *
                    (strlen(long_token) + strlen(curr_token)));
                asprintf(&new_str, "%s %s", long_token, curr_token);
                long_token = new_str;
                between_quotes = 0;

                // printf("long_token = %s\n", long_token);
                // push the long token on the results, free and re-malloc
                free(long_token);
                long_token = "";
            }
        }

        // a token before this started a quotes argument,
        // keep appending to the long token
        if(between_quotes) {
            curr_token = remove_symbol( curr_token, delim);
            new_str = malloc(sizeof(char) *
                (strlen(long_token) + strlen(curr_token)));
            asprintf(&new_str, "%s %s", long_token, curr_token);
            long_token = new_str;
            printf("long_token = %s\n", long_token);

        // this token is a plain old token that is not between quotes
        } else {
            // push the token to the results
        }
        free(new_str);
        curr_token = strtok( NULL, " " );
    }
    return cmd;
}

/**
 * if the word has a quotation then return true
 */
int contains( char* word, char sym) {
    for(int i = 0; i < strlen(word); i++) {
        if(word[i] == sym) {
            return 1;
        }
    }
    return 0;
}

/**
 * if the string contains the word, return 1
 */
int contains_word( char* str, char* word) {
    for(int i = 0, j = 0; i < strlen(str); i++) {

        // the current string index has the same character sequence as the word
        while(str[i] == word[j]) {
            i++;
            j++;

            // the words length has been checked and the last letter is the same
            if(j == strlen(word) && str[i] == word[j]) {
                return 1;
            } else
            if(i == strlen(str) || j == strlen(word)) {
                return 0;
            }
        }
    }
    return 0;
}
/**
 * remove the symbol from the work
 */
char* remove_symbol( char* word, char sym ) {
    char* tmp_word = malloc(sizeof(char) * strlen(word));
    strcpy(tmp_word, word);

    for(int i = 0, j = 0; i < strlen(word); i++) {
        if(word[i] == sym) {
            continue;
        }
        tmp_word[j] = word[i];
        j++;
    }
    word = tmp_word;
    free(tmp_word);
    return word;
}

/**
 * add the command to the end of the list
 */
void add_to_history( char* user_command, int cmd_num, char* options[] ) {
    char* curr_time = (char *)localtime(&raw_time);

    if(options != NULL && options[0] != NULL &&
            strcmp(options[0], user_command) == 1) {

        // Remove the command from the beginning of the options array
        for(int i = 0; options[i] != NULL; i++) {
            printf("options[%d] = %s\n", i, options[i]);
        }
    }

    // Create a command object to package and store the command information
    COMMAND entry = { user_command, curr_time, cmd_num, (void **)options };

    if( history_length < 10 ) {
        hist[ history_length ] = entry;
        history_length++;
    } else {
        for(int j = 0; j < 9; j++) {
            hist[j] = hist[j + 1];
        }
        hist[history_length] = entry;
        history_length = 10;
    }
}

/**
 * for every command in this mode show:
 *      command
 *      results of non-empty
 *      all major operations
 *
 *  if successful, return true
 */
int verbose( int argc, char *argv [] ) {

    if( strcmp(argv[1], "on\n") == 0) {
        if( verbose_mode == 1 ) {
            printf( "already in verbose mode.\n" );
        } else {
            verbose_mode = 1;
            printf( "entered verbose mode.\n" );
        }
        return 1;
    }
    if( strcmp(argv[1], "off\n") == 0 ) {
        if( verbose_mode == 0 ) {
            printf( "not in verbose mode.\n");
        } else {
            verbose_mode = 0;
            printf( "exited verbose mode.\n" );
        }
        return 1;
    }
    return 0;
}

/**
 * list of all internal commands included with mish
 */
int help( int argc, char *argv [] ) {
    printf(
        "\n"
        "Possible commands with mish:\n"
        "\tverbose - prints the command, results and major operations\n"
        "\thelp - list of all internal commands\n"
        "\thistory - list 10 most recent internal commands\n"
        "\tquit - terminate the application\n"
        "\n");

    return 1;
}

/**
 * list of commands so far with arguments and command number
 * the 10 most recent commands are listed by default from oldest to newest
 */
int history( int argc, char *argv [] ) {
    char** cmd_arguments;
    if( history_length == 0 ) {
        printf( "There is no history.\n");
        return 0;
    }
    for( int i = 0; i < history_length; i++ ) {

        printf( "[%d] %s", hist[i].id, hist[i].name );
        
        cmd_arguments = (char **)(hist[i].args);
        if(cmd_arguments != NULL && cmd_arguments[0] != NULL) {
            printf( " %s", cmd_arguments[0] );
        } else {
            printf( "\n" );
        }

        // Print out all of the arguments
        // for(int i = 0; cmd_arguments[i]; i++) {
        //     printf(" %s\n", cmd_arguments[i]);
        // }
        cmd_arguments = NULL;
    }
    printf("\n");
    return 1;
}

/**
 * clean up memory and terminate the application
 */
int quit( int argc, char *argv [] ) {
    free( line_read );
    return 1;
}

/**
 * remove all whitespace from string and return the new string
 */
char* remove_whitespace( char* input ) {
    char* output = input;
    int i, j;

    for (i = 0, j = 0; i < strlen( input ); i++,j++) {
        
        if( input[ i ] != ' ') {                           
            output[ j ] = input[ i ];
        } else {
            j--;
        }                                     
    }
    output[ j ] = 0;
    return output;
}