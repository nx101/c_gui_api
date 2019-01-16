


/**
 * PARSES USER INPUT AND FILE INPUT
 */



#ifndef PARSER_H_
#define PARSER_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "BasicErrorsHandling.h"

//#include "MainAux.h"

// specify the maximum line length
#define MAX_LINE_LENGTH 1024
#define NUMS_OF_LETTERS 26

// user settings commands Text
#define T_GAME_MODE "game_mode"
#define T_DIFFICULTY "difficulty"
#define T_USER_COLOR "user_color"
#define T_LOAD "load"
#define T_DEFAULT "default"
#define T_PRINT_SETTING "print_setting"
#define T_QUIT "quit"
#define T_START "start"

// user play commands Text
#define T_MOVE "move"
#define T_SAVE "save"
#define T_UNDO "undo"
#define T_RESET "reset"

// read and save files
#define START_TAG '<'
#define END_TAG '>'
#define CLOSING_TAG '/'
#define END_LINE '\n'
#define TAB_CHAR '\t'
#define CLOSING true
#define NULL_CHAR '\0'

//a type used to represent a command
typedef enum {
	COMMAND_GAME_MODE,
    COMMAND_DIFFICULTY,
    COMMAND_USER_COLOR,
    COMMAND_LOAD,
    COMMAND_DEFAULT,
    COMMAND_PRINT_SETTING,
	COMMAND_QUIT,
	COMMAND_START,
    COMMAND_MOVE,
    COMMAND_SAVE,
    COMMAND_UNDO,
    COMMAND_RESET,
	COMMAND_INVALID_LINE,
	NO_CMD
} COMMAND;

//a new type that is used to encapsulate a parsed line
typedef struct command_t {
	COMMAND cmd;
	bool isValidArg; //is set to true if the line contains a valid argument
	int arg;
    char path[MAX_LINE_LENGTH]; // for loading or saving files
    int currPos[2];
    int nexPos[2];
} Command;

/**
 * Checks if a specified string represents a valid integer. It is recommended
 * to use this function prior to calling the standard library function atoi.
 *
 * @return
 * true if the string represents a valid integer, and false otherwise.
 */
bool ParseIsInt(const char *str);


//--------------- Interact with User ---------------------


/**
 * Parses a specified line. If the line is a command which has an integer
 * argument then the argument is parsed and is saved in the field arg and the
 * field validArg is set to true. In any other case then 'validArg' is set to
 * false and the value 'arg' is undefined
 *
 * settingTime to parse this command as setting command or as play command
 *
 * @return
 * A parsed line such that:
 *   cmd - contains the command type, if the line is invalid then this field is
 *         set to INVALID_LINE
 *   validArg - is set to true if the command is add_disc and the integer argument
 *              is valid
 *   arg      - the integer argument in case validArg is set to true
 *   path	  - the string argument for save or loading files
 *   currPos & nextPos - the coord for movements
 */
Command ParseUserInputLine(const char *str, bool settingsTime);


/**
 * parse settings commands
 */
Command parseSettingsCommands(Command cmd, const char *type, const char *arg, const char *remainder);

/**
 * parse play commands
 */
Command parsePlayCommands(Command cmd, const char *type, const char *arg, const char *arg2, const char *arg3, const char *remainder);


/**
 * check if pos is at form "<x,y>"
 */
bool isValidPosForm(const char* pos);


/**
 * check if the move command is at form of "move <x,y> to <i,j>"
 */
bool isValidMoveCommand(const char* currPos, const char* to, const char* nextPos);

/**
 * save the arg in <x,y> in form to position on the board to pos[2]
 */
void parsePosArgs(int pos[2], const char* arg);



// --------- Loading files -----------------

/**
 * return first digit from in line after <tag>.
 * also, continue reading until end of line.
 * works for current, game_mode, difficulty, & color.
 */
int parseDigit(FILE *file);


/**
 * read in file, the tag after START_TAG and before END_TAG.
 * the tag put in arg tag[].
 * i.e. if line is <tag> ...., after func tag[] = "tag"
 * also, continue reading until end of line.
 */
void getTag(FILE* file, char tag[]);

/**
 * read given length of characters in Row from file to given array (row)
 */
void parseRow(FILE *file, char row[], int length);


/**
 * promote file until end of line
 */
void skipLine(FILE *file);



// -------- Saving File -----------------------

/**
 * file - The file to print
 * tagName - the tag to print in <tagName>
 * doubling - if we print after the opening tag
 * middle - string to print between open tag to closing tag
 * tabs - how many tabs to print in the beginning of the line
 */
void printLine(FILE *file, const char *tagName, bool doubling, char middle, int tabs);

/**
 * print to file tag name, surround by <>
 * use </> if it's closing tag
 */
void printTag(FILE* file, const char* tagName, bool closing);

/**
 * put '\n' in the file to mark it's end of the line
 */
void endLine(FILE* file);

/**
 * return value of char digit given.
 * i.e. for '0' func return 0
 */
int charToDigit(char c);


// ========== HELPERS ============


/**
 * return char that represent given digit.
 * i.e. for 0 func return '0'
 */
char digitToChar(int digit);

/**
 * use fputc to put char in file, and print error if it failed.
 */
void parserPutChar(int ch, FILE* file);

/**
 * use fputc to put string in file, and print error if it failed.
 */

void parsePutStr(const char* str, FILE* file);

/**
 * use fgetc to get char from file, and print error if it failed.
 */

char parserGetChar(FILE* file);


#endif
