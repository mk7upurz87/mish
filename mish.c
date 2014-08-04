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

#include "mish.h"

/**
 * Global variables
 */

int verbose_mode = 0;
char* line_read = NULL;
char** cmd_tokens = NULL;
COMMAND hist[10];
time_t raw_time;

/**
 * Take in the user input and verify.  Then execute the corresponding command
 */
int main( int argc, char* argv[] ) {

    int cmd_num = 1;
    size_t nbytes = 0;
    int bytes_read;

    while( 1 ) {
        printf ( "mish[%d]> ", cmd_num );
        line_read = ( char* )malloc( sizeof(char) );
        bytes_read = getline( &line_read, &nbytes, stdin );

        if( bytes_read == 1 ) {
            continue;
        } else {
            cmd_num++;
        }

        char* token = strtok( line_read, " " );
        char* option;

        // if(token[0] == 'EOF') {
        //     token = "quit\n";
        //     printf("quitting...");
        // }
        if(strcmp(token, "verbose\n") == 0 || strcmp(token, "verbose") == 0) {

            option = strtok( NULL, " " );
            if(option != NULL) {
                
                char* cmd = malloc(sizeof(char) * 8);
                cmd = "verbose\0";

                char* options[] = { cmd, option };
                
                printf("adding command... or trying too!");
                
                add_to_history( cmd, cmd_num, options );
                verbose(2, options);
            } else {
                printf("usage: verbose [on|off]\n");
            }
        }
        else if(strcmp(token, "help\n") == 0) {
            char* options[] = {"help"};
            help(1, options);
        }
        else if(strcmp(token, "history\n") == 0) {
            char* options[] = {"history"};
            history(1, options);
        }
        else if(strcmp(token, "quit\n") == 0) {
            char* options[] = {"quit"};

            // only close if all memory cleared properly
            if(quit(1, options) == 1) {
                return 0;
            }
        } else {
            // cmd_tokens = str_tokenize( line_read );
        }

        // free( line_read );
    }
}

/**
 * add the command to the end of the list, return the last empty index
 */
int add_to_history( char* user_command, int cmd_num, char* options[] ) {
    printf("adding command...");
    // printf("adding command: {%s, %s, %d}", (char *)localtime(&raw_time), user_command, cmd_num);
    char *curr_time = (char *)localtime(&raw_time);

    // COMMAND entry = malloc(sizeof(user_command) + sizeof(cmd_num) + sizeof(options) + sizeof(curr_time));
    // entry->name = command;
    // entry->time = curr_time;
    // entry->id = cmd_num;
    // entry->args = options;
    COMMAND entry = { user_command, curr_time, cmd_num, (void **)options };

    // printf("adding co: {%s, %s, %d}", entry.name, entry.time, entry.id);

    int i = 0;
    if(hist[10].name[0] != '\0') {
        printf("here");
        for(int j = 0; j < 9; j++) {
            hist[j] = hist[j + 1];
        }
        hist[10] = entry;
        return 10;
    } else {
        // find the next empty index
        while(i < 10 && hist[i++].name[0] != '\0') {}
        hist[i] = entry;
        return i + 1;
    }
}

/**
 * using the delimeter passed in, separate the string into tokens
 */
char** str_tokenize( char* str ) {

    char** results;
    char* token = strtok( str, " " );
    char* long_token;
    char* new_str;
    int between_quotes = 0;
    char delim = '\"';

    // walk through other tokens
    while( token != NULL ) {

        // the token has quotes so begin storing what follows as a single token
        if(contains(token, delim)) {
            if(!between_quotes) {
                // we're now between quotes so begin storing as a large token
                printf("entering quotes = %s\n", token);
                token = remove_symbol( token, delim);
                printf("removed quotes = %s\n", token);
                long_token = malloc(sizeof(char) * (strlen(long_token) + strlen(token)));
                long_token = token;
                between_quotes = 1;

                printf("long_token = %s\n", long_token);
            } else {
                // we're already storing a large token so stop it
                printf( "leaving quotes = %s\n", token );
                token = remove_symbol( token, delim);
                new_str = malloc(sizeof(char) * (strlen(long_token) + strlen(token)));
                asprintf(&new_str, "%s %s", long_token, token);
                long_token = new_str;
                between_quotes = 0;
                printf("long_token = %s\n", long_token);
                // push the long token on the results, free and re-malloc
                free(long_token);
                long_token = "";
            }
        }

        // a token before this started a quotes argument, keep appending to the long token
        if(between_quotes) {
            token = remove_symbol( token, delim);
            new_str = malloc(sizeof(char) * (strlen(long_token) + strlen(token)));
            asprintf(&new_str, "%s %s", long_token, token);
            long_token = new_str;
            printf("long_token = %s\n", long_token);

        // this token is a plain old token that is not between quotes
        } else {
            // push the token to the results
        }
        free(new_str);
        token = strtok( NULL, " " );
    }
    free(token);
    return results;
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
 * for every command in this mode show:
 *      command
 *      results of non-empty
 *      all major operations
 *
 *  if successful, return true
 */
int verbose( int argc, char *argv [] ) {

    if( strcmp(argv[1], "on\n") == 0 ) {
        verbose_mode = 1;
        printf( "entered verbose mode.\n" );
        return 1;
    }
    if( strcmp(argv[1], "off\n") == 0 ) {
        int previous_setting = verbose_mode;

        if( previous_setting == 1 ) {
            verbose_mode = 0;
            printf( "exited verbose mode.\n" );
        } else {
            printf( "already in verbose mode.\n" );
        }
        return 1;
    }
    else {
        printf( "usage: verbose [on|off]" );
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
    return 1;
}

/**
 * clean up memory and terminate the application
 */
int quit( int argc, char *argv [] ) {
    free( line_read );
    free( cmd_tokens );
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