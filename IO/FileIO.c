#include "FileIO.h"


static const char* ROWS[CHESS_N_ROWS] = {
        "row_1", "row_2", "row_3", "row_4",
        "row_5", "row_6", "row_7", "row_8"
};



bool loadFile(ChessGame* game, const char filePath[]) {

    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
		printMsgForSettingsState(MSG_SETTINGS_CMD_LOADFILE_FILEERR, __func__);
    	return false;
    }

    skipLine(file); //skip intro XML line
    skipLine(file); //skip <game> line

    game->currPlayer = (Color) parseDigit(file); //TODO use Color for this
    game->settings.gameModeUsersNum = parseDigit(file);

    if (game->settings.gameModeUsersNum == PLAYER_VS_COMP) {
    	game->settings.difLevel= parseDigit(file);
        game->settings.user1Color = (Color) parseDigit(file);
        skipLine(file); // skip <board> line
    }
    else { // difficulty & user_color are irrelevant and we need to skip them
        char tag[MAX_TAG] = {'\0'};
        do {
            getTag(file, tag);
            //skipLine(file); // cancel to prevent failing in test 11
        } while (strcmp(tag, T_BOARD) != 0);
    }

    char table[CHESS_N_COLUMNS][CHESS_N_ROWS];

    for (int i = CHESS_N_ROWS-1; i >= 0; i--) { // the board is from row8 to row1
        parseRow(file, table[i], CHESS_N_COLUMNS);
    }

    loadTable(game, table);

    setKings(game);

    fclose(file);
    
    ArrayListClear(game->history);

    return true;
}



bool saveFile(const ChessGame* game, const char filePath[]) {

    FILE *file = fopen(filePath, "w");
    if (file == NULL) {
		printMsgForSettingsState(MSG_SETTINGS_CMD_LOADFILE_FILEERR, __func__);
    	return false;
    }

    char current = digitToChar(game->currPlayer);
	char game_mode = digitToChar(game->settings.gameModeUsersNum);


    printLine(file, T_FIRST_XML_LINE, false, NULL_CHAR, 0);

    printLine(file, T_GAME, false, NULL_CHAR, 0);

    printLine(file, T_TURN, true, current, 1);
    printLine(file, T_GAME_MODE, true, game_mode, 1);

    if (game->settings.gameModeUsersNum == PLAYER_VS_COMP) {
    	char difficulty = digitToChar(game->settings.difLevel);
    	char userColor = digitToChar(game->settings.user1Color);
        printLine(file, T_DIFFICULTY, true, difficulty, 1);
        printLine(file, T_USER_COLOR, true, userColor, 1);
    }

    printLine(file, T_BOARD, false, NULL_CHAR, 1);

    for (int i = CHESS_N_ROWS-1; i >= 0; i--) { // the board is from row_8 to row_1
        parserPutChar(TAB_CHAR, file); parserPutChar(TAB_CHAR, file); // twice tabs
        printTag(file, ROWS[i], false);
        for (int j = 0; j < CHESS_N_COLUMNS; j++) {
        	Piece* piece = game->gameBoard.board[j][i].piece;
            parserPutChar(getPieceChar(piece),file);
		}
        printTag(file, ROWS[i], CLOSING);
        parserPutChar(END_LINE, file);
    }

    parserPutChar(TAB_CHAR, file); printTag(file, T_BOARD, CLOSING); parserPutChar(END_LINE, file);

    printTag(file, T_GAME, CLOSING); parserPutChar(END_LINE, file);

    fclose(file);

    return true;
}


// --------------- GUI HANDLING ---------------------

static int validSlots = 0;
static int mostRecentSlot = 0;
static const char* valuesPath = "saves/values.txt";
static const char* slotsForGUI[NUM_OF_SLOTS] = {
        "saves/0.xml", "saves/1.xml", "saves/2.xml", "saves/3.xml" ,"saves/4.xml"
};

int getValidSlotsNum() {return validSlots;}

void initializeValidAndRecentSlot() {
    FILE* file = fopen(valuesPath, "r");
    if (file == NULL) {
        printf("ERROR: couldn't load previous slots\n");
        return;
    }
    validSlots = charToDigit(parserGetChar(file));
    mostRecentSlot = charToDigit(parserGetChar(file));

    fclose(file);
}

void saveValidAndRecentSlot() {
    FILE* file = fopen(valuesPath, "w");
    if (file == NULL) {
        printf("ERROR: couldn't save slots to next games\n");
        return;
    }
    parserPutChar(digitToChar(validSlots),file);
    parserPutChar(digitToChar(mostRecentSlot),file);

    fclose(file);

}


bool loadFileFromSlot(ChessGame* game, int slotNum) {

    if (slotNum < 0 || slotNum >= validSlots) return false;

    int actualSlot = (mostRecentSlot - slotNum) % NUM_OF_SLOTS;


    return loadFile(game, slotsForGUI[actualSlot]);
}


bool saveFileToSlot(ChessGame* game) {

    int actualSlot = (mostRecentSlot + 1) % NUM_OF_SLOTS;

    mostRecentSlot = actualSlot;

    if (validSlots < NUM_OF_SLOTS) validSlots++;


    return saveFile(game, slotsForGUI[actualSlot]);
}
