

/*
 * HANDLES GAME OUTPUT TO USER
 * For both Console and Gui modes, based on active game mode
 * Hides console / gui output choice from Game modules
 * All feedback texts are defined here (msgs, errors, etc)
 */



#ifndef CHESSPROG_VIEWCONTROLLER_H
#define CHESSPROG_VIEWCONTROLLER_H

#include <stdio.h>
#include <string.h>
#include "../BoardGame/BoardGame.h"
#include "ChessGameBoard.h"
#include "../IO/BasicErrorsHandling.h"

#define EMPTY_CELL '_'






/* ==2== CONSOLE - MAIN MESSAGES (INC. FEEDBACK AND INPUT ERRORS)

    * Some for User's turn some for Comp turn

*/


// TODO - what about bad values for some cmds here?

typedef enum t_game_console_msg_settings_state {
    MSG_SETTINGS_QUIT,
    MSG_SETTINGS_INP_REQ_SETTING_OR_START,
    MSG_SETTINGS_CMD_GAMEMODE_SET_1,
    MSG_SETTINGS_CMD_GAMEMODE_SET_2,
    MSG_SETTINGS_CMD_GAMEMODE_BAD_ARG,
    MSG_SETTINGS_CMD_DIFFICULTY_EXPERT_NOT_SUPPORTED,
    MSG_SETTINGS_CMD_DIFFICULTY_VAL_NOT_IN_RANGE,
    MSG_SETTINGS_CMD_DIFFICULTY_INVALID_CMD, // ERR on 2-player mode
    MSG_SETTINGS_CMD_USERCOLOR_INVALID_CMD,  // ERR on 2-player mode
    MSG_SETTINGS_CMD_LOADFILE_FILEERR,       // ERR opening file
} GAME_CONSOLE_MSG_SETTINGS;


typedef enum t_game_console_msg_playing_state_user {
    MSG_GAME_USER_QUIT,
    MSG_GAME_USER_RESET,
    MSG_GAME_USER_INP_REQ_ENTER_MOVE,
    MSG_GAME_USER_CMD_MOVE_XY_TO_IJ_1_INVALID_POS, // Either one of location is invalid
    MSG_GAME_USER_CMD_MOVE_XY_TO_IJ_2_XY_NOT_PLAYER_PIECE,
    MSG_GAME_USER_CMD_MOVE_XY_TO_IJ_3_MOVE_ILLEGAL_FOR_PLAYER_PIECE,
    MSG_GAME_USER_CMD_GET_MOVES_1_INVALID_POS,
    MSG_GAME_USER_CMD_GET_MOVES_2_INVALID_POS,
    MSG_GAME_USER_CMD_UNDO_1_NOT_AVAIL_IN_2PLAYER_MODE,
    MSG_GAME_USER_CMD_UNDO_2_NO_HISTORY,
    MSG_GAME_USER_CMD_UNDO_3_OK_REPORT_CHANGE,
    MSG_GAME_USER_ON_BOARD_UPDATED_KING_IN_CHECK,
    MSG_GAME_USER_ON_BOARD_UPDATED_GAME_ENDS_CHECKMATE,
    MSG_GAME_USER_ON_BOARD_UPDATED_GAME_ENDS_TIE,
    MSG_GAME_USER_ON_BOARD_UPDATED_USER_PAWN_PROMOTE_ASK,
    MSG_GAME_USER_ON_BOARD_UPDATED_USER_PAWN_PROMOTE_INVALID_TYPE,
	MSG_GAME_USER_CMD_SAVEFILE_FILEERR		// ERR opening file
} GAME_CONSOLE_MSG_PLAYING_STATE_USER_TURN;


typedef enum t_game_console_msg_playing_state_ai {
    MSG_GAME_AI_QUIT,
    MSG_GAME_AI_ON_BOARD_UPDATED_KING_IN_CHECK,
    MSG_GAME_AI_ON_BOARD_UPDATED_GAME_ENDS_CHECKMATE,
    MSG_GAME_AI_ON_BOARD_UPDATED_GAME_ENDS_TIE,
    MSG_GAME_AI_ON_BOARD_UPDATED_AI_REPORT_REGULAR_NO_PAWN_PROMOTE,
    MSG_GAME_AI_ON_BOARD_UPDATED_REPORT_WITH_PAWN_PROMOTE,
} GAME_CONSOLE_MSG_PLAYING_STATE_AI_TURN;





/**
 * @return name of piece type according to @param nameId
 */
const char* getTypeNamesFromTypeId(char nameId);
/**
 * @return lower-case text if @param color
 */
char* getColorText(const Color color);

/**
 * @return upper-case text if @param color
 */
char* getCapitalColorText(const Color color);

/**
 * @return nameId of @param piece according to its color
 * i.e. white piece is lower-case and black piece is upper-case
 */
char getPieceChar(const Piece* piece);

/**
 * print appropriated messages for settings states commands, as instructed
 */
void printMsgForSettingsState(GAME_CONSOLE_MSG_SETTINGS msg, const char* func);

/**
 * print setting of @param game
 */
void printSetting(ChessGame* game);

/**
 * print appropriated messages for user play states commands, as instructed
 */
void printMsgForPlayingStateAndUserTurn(GAME_CONSOLE_MSG_PLAYING_STATE_USER_TURN msg, const char *func,
                                        Color color, const Vector *v1, const Vector *v2);

/**
 * print appropriated messages for computer moves, as instructed
 */
void printMsgForPlayingStateAndAITurn(GAME_CONSOLE_MSG_PLAYING_STATE_AI_TURN msg,
                                      const Piece *piece, const Vector *v1, const Vector *v2);

/**
 * print @param gameBoard as instructed
 */
void printBoard(GameBoard* gameBoard);



















#endif //CHESSPROG_VIEWCONTROLLER_H
