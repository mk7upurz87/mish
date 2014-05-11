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

#include "mish.h"

/**
 * Global variables
 */

int verbose_mode = 0;
char* line_read = NULL;
char** cmd_tokens = NULL;

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

        cmd_tokens = str_tokenize( line_read );

        // char c = cmd_tokens[0][0];
        // if (c == EOF) {
        //      break;
        // }
    }
}

/**
 * using the delimeter passed in, separate the string into tokens
 */
char** str_tokenize( char* str ) {

    char** results;
    char* token = strtok( str, " " );
    char* long_token = "";
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
                char* new_str;
                asprintf(&new_str, "%s %s", long_token, token);
                long_token = new_str;
                between_quotes = 1;
                printf("long_token = %s\n", long_token);
            } else {
                // we're already storing a large token so stop it
                printf( "leaving quotes = %s\n", token );
                token = remove_symbol( token, delim);
                char* new_str;
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
            char* new_str;
            asprintf(&new_str, "%s %s", long_token, token);
            long_token = new_str;
            printf("long_token = %s\n", long_token);

        // this token is a plain old token that is not between quotes
        } else {
            // push the token to the results
        }
        token = strtok( NULL, " " );
    }
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
    if(argv[0] == ( char* )"on") {
        verbose_mode = 1;
        printf( "you are in verbose mode.\n" );
        return 1;
    }
    if( argv[0] == ( char* )"off" ) {
        int previous_setting = verbose_mode;

        if( previous_setting == 1 ) {
            verbose_mode = 0;
            printf( "you left verbose mode.\n" );
        } else {
            printf( "you are in verbose mode already.\n" );
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
        "Possible commands with mish!\n"
        "\tverbose - "
        "\thelp - list of all internal commands\n"
        "\thistory - list 10 most recent internal commands\n"
        "\tquit - terminate the application\n");

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