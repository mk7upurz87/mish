/**
 * filename: mish.h
 * author: sdd7427 (Shaun DeVos)
 * description:	 The Mish shell is a simple interactive shell
 *		capable of executing simple UNIX commands and some
 *		internal commands.
 *
 * date: 05/10/1214
 */

struct command_entry {
	char *name;
	char *time;
    int id;
    void **args;
};

typedef struct command_entry COMMAND;

/*************************************
 * 			INTERNAL COMMANDS
 ************************************/

/**
 * for every command in this mode show:
 *      command
 *      results of non-empty
 *      all major operations
 *
 *  if successful, return true
 */
int verbose( int argc, char *argv [] );

/**
 * list of all internal commands included with mish
 */
int help( int argc, char *argv [] );

/**
 * list of commands so far with arguments and command number
 * the 10 most recent commands are listed by default from oldest to newest
 */
int history( int argc, char *argv [] );

/**
 * clean up memory and terminate the application
 */
int quit( int argc, char *argv [] );


/*********************************
 *			HELPER FUNCTIONS
 ********************************/

/**
 * using the delimeter passed in, separate the string into tokens
 */
COMMAND str_to_cmd( char* str );

/**
 * if the word has a quotation then return true
 */
int contains( char* word, char sym);

/**
 * remove the symbol from the work
 */
char* remove_symbol( char* word, char sym );

/**
 * remove all whitespace from string and return the new string
 */
char* remove_whitespace( char* input );

/**
 * if the string contains the word, return 1
 */
int contains_word( char* str, char* word );

/**
 * add the command to the end of the list, return the last empty index
 */
void add_to_history( char* user_command, int cmd_num, char* options[] );