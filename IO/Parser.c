//#include <pwd.h>
#include "Parser.h"

/**
 * Checks str to be all numbers (leading zeros allowed by atoi)
 */
bool ParseIsInt(const char *str) {
	if (str == NULL) return  false;
	const char* c = str;
    if (*c == '-') c++;
	while (*c != '\0') {
		if (*c < '0' || '9' < *c)
            return false;
		c++;
	}
	return true;
}


//TODO commands like this in both func
/**
 * Commands:
 * TOKEN1			TOKEN2
 * -------			--------
 * suggest_move
 * undo_move
 * add_disc			<col_num>
 * quit
 * restart_game
 */

Command ParseUserInputLine(const char *str, bool settingsTime) {
    // Copy str
    char str2[MAX_LINE_LENGTH];
    strcpy(str2, str); //TODO security

    const char *delimiter = " \t\r\n"; // Including ace - POSIX whiteaces
    const char *token1 = strtok(str2, delimiter);
    const char *token2 = strtok(NULL, delimiter);
    const char *token3 = strtok(NULL, delimiter);
    const char *token4 = strtok(NULL, delimiter);
    const char *token5 = strtok(NULL, delimiter);


    // Local, Static-Allocation
    Command cmd;
    cmd.cmd = COMMAND_INVALID_LINE;
    cmd.isValidArg = false;
    cmd.arg = -1;

    if (settingsTime)
    	return parseSettingsCommands(cmd, token1, token2, token3);
    else
    	return parsePlayCommands(cmd, token1, token2, token3, token4, token5);
}


Command parseSettingsCommands(Command cmd, const char *type, const char *arg, const char *remainder) {
    // CMD: game_mode x
    if (!strcmp(type, T_GAME_MODE) && remainder == NULL) {
        cmd.cmd = COMMAND_GAME_MODE;
        if (ParseIsInt(arg)) {
            cmd.arg = atoi(arg);
            cmd.isValidArg = true;
        }
    }

    // CMD: difficulty x
    else if (!strcmp(type, T_DIFFICULTY) && remainder == NULL) {
        cmd.cmd = COMMAND_DIFFICULTY; //TODO ask what to return when x isn't int, like "difficulty abc"
        if (ParseIsInt(arg)) {
            cmd.arg = atoi(arg);
            cmd.isValidArg = true;
        }
    }

    // CMD: user_color x
    else if (!strcmp(type, T_USER_COLOR) && remainder == NULL) {
        cmd.cmd = COMMAND_USER_COLOR; //TODO ask what to return when colors args aren't valid
        if (ParseIsInt(arg)) {
            cmd.arg = atoi(arg);
            cmd.isValidArg = true;
        }
    }

    // CMD: load x
    else if (!strcmp(type, T_LOAD) && remainder == NULL) {
        // we assumed as instructed that file path doesn't contain spaces
        cmd.cmd = COMMAND_LOAD;
        strcpy(cmd.path, arg);
    }

    // CMD: default
    else if (!strcmp(type, T_DEFAULT) && arg == NULL) {
        cmd.cmd = COMMAND_DEFAULT;
    }

    // CMD: print_settings
    else if (!strcmp(type, T_PRINT_SETTING) && arg == NULL) {
        cmd.cmd = COMMAND_PRINT_SETTING;
    }


    // CMD: start
    else if (!strcmp(type, T_START) && arg == NULL){
        cmd.cmd = COMMAND_START;
    }

    // CMD: quit
    if (!strcmp(type, T_QUIT) && arg == NULL){
        cmd.cmd = COMMAND_QUIT;
    }

    return cmd;
}


Command parsePlayCommands(Command cmd, const char *type, const char *arg, const char *arg2, const char *arg3, const char *remainder) {

    // CMD: move <x.y> to <i,j>
    if (!strcmp(type, T_MOVE) && remainder == NULL) {
        cmd.cmd = COMMAND_MOVE;
        cmd.isValidArg = isValidMoveCommand(arg, arg2, arg3); // TODO ask what to do when isn't valid form
        if (cmd.isValidArg) {
            parsePosArgs(cmd.currPos, arg); // arg is currentPos <x,y>
            parsePosArgs(cmd.nexPos, arg3); // arg3 is nextPos <i.j>
        }
    }

    // CMD: save x
    else if (!strcmp(type, T_SAVE) && arg2 == NULL) {
        cmd.cmd = COMMAND_SAVE;
        strcpy(cmd.path, arg);
    }

    // CMD: undo
    else if (!strcmp(type, T_UNDO) && arg == NULL) {
        cmd.cmd = COMMAND_UNDO;
    }

    // CMD: reset
    else if (!strcmp(type, T_RESET) && arg == NULL) {
        cmd.cmd = COMMAND_RESET;
    }

    // CMD: quit
    else if (!strcmp(type, T_QUIT) && arg == NULL){
        cmd.cmd = COMMAND_QUIT;
    }

    return cmd;
}

bool isValidPosForm(const char* pos) {
    if (pos == NULL) return false;

    int i = 0;

    if (pos[i] != '<') return false;

    // loop at the first argument until ',' or end of string
    for (; pos[i] != ',' &&  pos[i] != '\0'; i++) {};
    if (pos[i] != ',') return false;

    // loop at the second argument until '>' or end of string
    for (; pos[i] != '>' &&  pos[i] != '\0'; i++) {};
    if (pos[i] != '>') return false;

    return true;
}

bool isValidMoveCommand(const char* currPos, const char* to, const char* nextPos) {
    if (!isValidPosForm(currPos) || !isValidPosForm(nextPos)) return false;
    if (strcmp(to, "to") != 0) return false;

    return true;
}


void parsePosArgs(int pos[2], const char* arg) {
    // parse to
    char arg_copy[MAX_LINE_LENGTH];
    strcpy(arg_copy, arg); //TODO security
    const char* delimiter = ",";
    const char* pos1 = strtok(arg_copy, delimiter);
    const char* pos2 = strtok(NULL, delimiter);

    // parse first argument
    assertAndPrintFuncErr(ParseIsInt(pos1 + 1), __func__); // +1 to jump above the opening Pos '<"
    pos[1] = atoi(pos1+1)-1;

    // parse second argument
    pos[0] = pos2[0] - 'A';
    for (int i = 1; pos2[i] != '>' ; i++) {
        pos[0] *= NUMS_OF_LETTERS;
        pos[0] += pos2[i] - 'A';
    }
}


int parseDigit(FILE* file) {
    while (parserGetChar(file) != END_TAG); // promote until row content
    int digit = charToDigit(parserGetChar(file)); // parserGetChar return char index
    skipLine(file);
    return digit;
}


void getTag(FILE* file, char tag[]) {
    while (parserGetChar(file) != START_TAG); // skip tabs and start tag
    char c;
    for (int i = 0; (c = parserGetChar(file)) != END_TAG; ++i) {
        tag[i] = c;
    }
    skipLine(file);
}


void parseRow(FILE* file, char row[], int length) {
    while (parserGetChar(file) != END_TAG); // promote until row content
    // TODO maybe validate we actually read "<row_x>"
    for (int i = 0; i < length; i++) {
        row[i] = parserGetChar(file); // read the row from file
    }
    skipLine(file);
}

void skipLine(FILE* file) {
    while (parserGetChar(file) != END_LINE);
}



void printLine(FILE *file, const char *tagName, bool doubling, char middle, int tabs) {
    for (int i = 0; i < tabs; i++) {
        parserPutChar(TAB_CHAR, file);
    }
    printTag(file, tagName, !CLOSING);
    if (doubling) {
        parserPutChar(middle, file);
        printTag(file, tagName, CLOSING);
    }
    endLine(file);
}

void printTag(FILE* file, const char* tagName, bool closing) {
    parserPutChar(START_TAG, file);
    if (closing) parserPutChar(CLOSING_TAG, file);
    parsePutStr(tagName, file);
    parserPutChar(END_TAG, file);
}

void endLine(FILE* file) {
    parserPutChar(END_LINE, file);
}

int charToDigit(char c) {
	return c-'0';
}

char digitToChar(int digit) {
    return digit+'0';
}


void parserPutChar(int ch, FILE* file) {
    if (ch != NULL_CHAR)
        assertAndPrintFuncErr(fputc(ch, file) != EOF, __func__);
}

void parsePutStr(const char* str, FILE* file) {
    assertAndPrintFuncErr(fputs(str, file) != EOF, __func__);
}

char parserGetChar(FILE* file) {
    char c = fgetc(file);
    assertAndPrintFuncErr(c != EOF, __func__); // we assume it isn't  End Of File
    return c;
}
