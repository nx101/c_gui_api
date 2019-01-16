

#include "ChessGameViewOutput.h"



// ==0== GENERAL PRINTING UTILITY HELPERS


static const char LABELS_0[CHESS_N_ROWS] = {'1', '2', '3', '4', '5', '6', '7', '8'};
static const char LABELS_1[CHESS_N_COLUMNS] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

static const char* typeNames[] = {"pawn", "bishop", "knight", "rook", "queen", "king"};


const char* getTypeNamesFromTypeId(char nameId) {
    switch (nameId) {
        case SYMBOL_PAWN:
            return typeNames[0];

        case SYMBOL_BISHOP:
            return typeNames[1];

        case SYMBOL_KNIGHT:
            return typeNames[2];

        case SYMBOL_ROOK:
            return typeNames[3];

        case SYMBOL_QUEEN:
            return typeNames[4];

        case SYMBOL_KING:
            return typeNames[5];


        default:
            printErr(ERR_FUNC_FAILED, __func__);
            return NULL;
    }
}

char* getColorText(const Color color){

    switch (color) {

        case BLACK :
            return "black";

        case WHITE :
            return "white";

        default :
            printErr(ERR_FUNC_FAILED, __func__);
            return NULL;
    }
}

char* getCapitalColorText(const Color color) {
    switch (color) {

        case BLACK :
            return "BLACK";

        case WHITE :
            return "WHITE";

        default :
            printErr(ERR_FUNC_FAILED, __func__);
            return NULL;
    }

}


char getPieceChar(const Piece* piece) {
	if (piece == NULL) return EMPTY_CELL;

    if (piece->color == WHITE)
        return piece->type->nameId;
    else
        return toUppercase(piece->type->nameId);
}


// ==2== MAIN MSGs Printing


// 2.1 Settings state:

void printMsgForSettingsState(GAME_CONSOLE_MSG_SETTINGS msg, const char* func){

    switch (msg) {

        case MSG_SETTINGS_QUIT :
            printf("Exiting...\n");
            break;

        case MSG_SETTINGS_INP_REQ_SETTING_OR_START :
            printf("Specify game setting or type 'start' to begin a game with the current setting:\n");
            break;

        case MSG_SETTINGS_CMD_GAMEMODE_SET_1 :
            printf("Game mode is set to 1 player\n");
            break;

        case MSG_SETTINGS_CMD_GAMEMODE_SET_2 :
            printf("Game mode is set to 2 players\n");
            break;

        case MSG_SETTINGS_CMD_GAMEMODE_BAD_ARG:
            printf("Wrong game mode\n");
            break;

        case MSG_SETTINGS_CMD_DIFFICULTY_EXPERT_NOT_SUPPORTED :
            printf("Expert level not supported, please choose a value between 1 to 4:\n");
            break;

        case MSG_SETTINGS_CMD_DIFFICULTY_VAL_NOT_IN_RANGE :
            printf("Wrong difficulty level. The value should be between 1 to 5\n");
            break;

        case MSG_SETTINGS_CMD_DIFFICULTY_INVALID_CMD : // ERR on 2-player mode
            printErr(ERR_INV_CMD, func);
            break;

        case MSG_SETTINGS_CMD_USERCOLOR_INVALID_CMD : // ERR on 2-player mode
            printErr(ERR_INV_CMD, func);
            break;

        case MSG_SETTINGS_CMD_LOADFILE_FILEERR : // ERR on file opening
            printErr(ERR_FILE_CANT_OPEN, func);
            break;

        default :
            printErr(ERR_FUNC_FAILED, __func__);


    }

    fflush(stdout);

}





void printSetting(ChessGame* game){
    ChessSettings sets = game->settings;
    printf("SETTINGS:\n");
    printf("GAME_MODE: %d\n", sets.gameModeUsersNum);

    if (sets.gameModeUsersNum==PLAYER_VS_COMP){
        printf("DIFFICULTY_LVL: %d\n",sets.difLevel);
        printf("USER_CLR: %s\n",getCapitalColorText(sets.user1Color));
    }


}



// 2.2 Game playing state - USER TURN


void printMsgForPlayingStateAndUserTurn(GAME_CONSOLE_MSG_PLAYING_STATE_USER_TURN msg, const char *func,
                                        const Color color, const Vector *v1, const Vector *v2) {

    switch (msg) {

        case MSG_GAME_USER_QUIT :
            printf("Exiting...\n");
            break;

        case MSG_GAME_USER_RESET :
            printf("Restarting...\n");
            break;

        case MSG_GAME_USER_INP_REQ_ENTER_MOVE :
            printf("%s", getColorText(color));
            printf(" player - enter your move:\n");
            break;

        case MSG_GAME_USER_CMD_MOVE_XY_TO_IJ_1_INVALID_POS :
            printf("Invalid position on the board\n");
            break;

        case MSG_GAME_USER_CMD_MOVE_XY_TO_IJ_2_XY_NOT_PLAYER_PIECE :
            printf("The specified position does not contain your piece\n");
            break;

        case MSG_GAME_USER_CMD_MOVE_XY_TO_IJ_3_MOVE_ILLEGAL_FOR_PLAYER_PIECE :
            printf("Illegal move\n");
            break;

        case MSG_GAME_USER_CMD_GET_MOVES_1_INVALID_POS :
            printf("Invalid position on the board\n");
            break;

        case MSG_GAME_USER_CMD_GET_MOVES_2_INVALID_POS :  // USES ARGS: COLOR
            printf("The specified position does not contain ");
            printf("%s", getColorText(color));
            printf(" player piece\n");
            break;

        case MSG_GAME_USER_CMD_UNDO_1_NOT_AVAIL_IN_2PLAYER_MODE :
            printf("Undo command not available in 2 players mode\n");
            break;

        case MSG_GAME_USER_CMD_UNDO_2_NO_HISTORY :
            printf("Empty history, move cannot be undone\n");
            break;

        case MSG_GAME_USER_CMD_UNDO_3_OK_REPORT_CHANGE :
            if (0 > printf("Undo move for player %s : <%c,%c> -> <%c,%c>\n",
                           getColorText(color),
                           LABELS_0[v1->v[1]], LABELS_1[v1->v[0]],
                           LABELS_0[v2->v[1]], LABELS_1[v2->v[0]])  )
                printErr(ERR_FUNC_FAILED, __func__);
            break;


            // On board update (after a valid move) feedback:
        case MSG_GAME_USER_ON_BOARD_UPDATED_KING_IN_CHECK : // USES ARGS: COLOR
            printf("Check: ");
            printf("%s", getColorText(color));
            printf(" King is threatened!\n"); //TODO Typo in pdf
            break;


        case MSG_GAME_USER_ON_BOARD_UPDATED_GAME_ENDS_CHECKMATE : // ARGS: COLOR
            printf("Checkmate! ");
            printf("%s", getColorText(color));
            printf(" player wins the game\n");
            break;

        case MSG_GAME_USER_ON_BOARD_UPDATED_GAME_ENDS_TIE :
            printf("The game is tied\n");
            break;

            // Pawn Promotion
        case MSG_GAME_USER_ON_BOARD_UPDATED_USER_PAWN_PROMOTE_ASK :
            printf("Pawn promotion- please replace the pawn by queen, rook, knight, bishop or pawn:\n");
            break;

        case MSG_GAME_USER_ON_BOARD_UPDATED_USER_PAWN_PROMOTE_INVALID_TYPE :
            printf("Invalid Type\n");
            break;

        case MSG_GAME_USER_CMD_SAVEFILE_FILEERR:
            printErr(ERR_FILE_CANT_OPEN, func);
            break;

            // Function call up Error
        default :
            printErr(ERR_FUNC_FAILED, __func__);


    }

    fflush(stdout);

}


// 2.3 Game playing state - AI TURN

void printMsgForPlayingStateAndAITurn(GAME_CONSOLE_MSG_PLAYING_STATE_AI_TURN msg,
                                      const Piece *piece, const Vector *v1, const Vector *v2) {

    switch (msg) {

        case MSG_GAME_AI_QUIT :
            printf("Exiting...\n");
            break;


            // On board update feedback (after Computer's move):
        case MSG_GAME_AI_ON_BOARD_UPDATED_KING_IN_CHECK :
            printf("Check!\n");
            break;


        case MSG_GAME_AI_ON_BOARD_UPDATED_GAME_ENDS_CHECKMATE : // ARGS: COLOR
            printf("Checkmate! ");
            printf("%s", getColorText(piece->color));
            printf(" player wins the game\n");
            break;

        case MSG_GAME_AI_ON_BOARD_UPDATED_GAME_ENDS_TIE :
            printf("The game ends in a tie\n");
            break;


            // Pawn Promotion
        case MSG_GAME_AI_ON_BOARD_UPDATED_AI_REPORT_REGULAR_NO_PAWN_PROMOTE : // USES ARGS
            if (0 > printf("Computer: move %s at <%c,%c> to <%c,%c>\n",
                           getTypeNamesFromTypeId(piece->type->nameId),
                           LABELS_0[v1->v[1]], LABELS_1[v1->v[0]],
                           LABELS_0[v2->v[1]], LABELS_1[v2->v[0]]))
                printErr(ERR_FUNC_FAILED, __func__);
            break;


        case MSG_GAME_AI_ON_BOARD_UPDATED_REPORT_WITH_PAWN_PROMOTE : // USES ARGS
            if (0 > printf("Computer: move pawn <%c,%c> to <%c,%c> and promote to %c",
                           LABELS_0[v1->v[1]], LABELS_1[v1->v[0]],
                           LABELS_0[v2->v[1]], LABELS_1[v2->v[0]],
                           piece->type->nameId))
                printErr(ERR_FUNC_FAILED, __func__);
            break;


            // Function call up Error
        default :
            printErr(ERR_FUNC_FAILED, __func__);


    }

    fflush(stdout);

}






void printBoard(GameBoard* gameBoard) {

    for (int row = gameBoard->dims[1]-1; row >= 0; row--) {
        printf("%d| ", row+1);
        for (int col = 0; col < gameBoard->dims[0]; col++) {
            printf("%c ", getPieceChar(gameBoard->board[col][row].piece));
        }
        printf("|\n");
    }

    printf("  -----------------\n");
    printf("   A B C D E F G H\n");

}






// ==3== GUI
