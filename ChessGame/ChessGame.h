


/*
 * REPRESENTS A CHESS GAME
 * Handles all specifics of the game and its logic
 */



#ifndef CHESSPROG_CHESSGAME_H
#define CHESSPROG_CHESSGAME_H

#include "../IO/FileIO.h"
#include "../IO/Parser.h"
#include "ChessGameViewOutput.h"
#include "ChessGameBoard.h"

#define SETTINGS_TIME true
#define GAME_TIME false


typedef struct t_choice {
    Vector prevPos;
    Vector nextPos;
} Choice;


typedef struct t_history {
    Choice choice;
    PieceType* type;
} History;


/**
 * create game with board, pieceTypes, History
 * DO NOT load pieces to the board
 * @return the game
 */
ChessGame* createGame();

/**
 * destroy @param game
 */
void destroyGame(ChessGame* game);

/**
 * load pieces to board and set game settings to default
 *
 */
void initializeGame(ChessGame* game);

/**
 * loop on getting settings commands from user, until start or quit command
 * @return start or quit command
 */
COMMAND doSettings(ChessGame* game);

/**
 * handle play commands
 * @return command
 */
COMMAND doPlayerTurn(ChessGame* game);

/**
 *
 * make move according to @param cmd
 * @return if move succeeded
 */
bool move(ChessGame* game, Command cmd);

/**
 * @param game  - our game
 * @param prevPos - starting point coord for move
 * @param nextPos - destination coord for move
 * @param print - if print messages
 * @return if move succeeded
 */
bool moveAdvanced(ChessGame* game, const Vector* prevPos, const Vector* nextPos, bool print);

/**
 *
 * @param game  - our game
 * @param currPos - starting point coord for move
 * @param nextPos - destination coord for move
 * @return if move valid
 * NOTE: unlike possibleMove, isMoveValid make sure move don't put player's king at risk
 */
bool isMoveValid(ChessGame* game, Vector* currPos, Vector* nextPos);

/**
 * get all possible valid moves for piece
 * @param game - our game
 * @param coord - starting point coord
 * @return pointer to Array List of coordinations to possible moves
 */
ArrayList* getValidMoves(ChessGame* game, Vector* coord);

/**
 * undo last round in @param game
 * NOTE: func undo move of both player and computer
 */
void undo(ChessGame* game);

/**
 *
 * undo last move in @param game
 * @param print - if print messages
 */
void undoingMove(ChessGame* game, bool print);

/**
 * @return if @param game is finished
 */
bool isGameFinished(ChessGame *game);

/**
 * @return genereic invalid choice
 */
Choice getInvalidChoice();

/**
 *
 * @return if @param choice is on @param board
 */
bool isChoiceValid(const GameBoard* board, const Choice* choice);

/**
 * @return if king in @param kingColor is threaten
 */
bool isKingThreatend(ChessGame* game, Color kingColor);

/**
 * load saving at @param filePath to @param game
 */
void doLoad(ChessGame* game, const char* filePath);

/**
 * @return if player in @param color can move.
 */
bool isPlayerCanMove(ChessGame* game, Color color);

/**
 *
 * update game state
 */
void updateGameState(ChessGame *game);

/**
 * @return if user can undo
 */
bool canUndo(ChessGame* game);


#endif //CHESSPROG_CHESSGAME_H
