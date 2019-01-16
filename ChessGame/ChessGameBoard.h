/*
 * ChessGameBoard.h
 *
 *  Created on: 4 Sep 2017
 *      Author: eyal
 */

#ifndef SOURCE_CHESSGAME_CHESSGAMEBOARD_H_
#define SOURCE_CHESSGAME_CHESSGAMEBOARD_H_

#include "../BoardGame/BoardGame.h"
#include "../DataStructs/ArrayList.h"

#define HISTORY_SIZE 3*2
#define PIECE_TYPES_NUM 6
#define CHESS_N_ROWS 8
#define CHESS_N_COLUMNS 8
#define MAX_DIFF 5
#define PLAYER_VS_COMP 1
#define PLAYER_VS_PLAYER 2

#define NULL_PIECE_TYPE {0,NULL,0}


#define DEFAULT_GAME_MODE PLAYER_VS_COMP
#define DEFAULT_DIFFOCULTY 2
#define DEFAULT_COLOR WHITE
#define MAX_THREATS 32

#define START_BOARD {"rnbqkbnr", "mmmmmmmm","________", "________", "________", "________","MMMMMMMM", "RNBQKBNR"}

typedef enum t_game_state {
    GAME_STATE_ON,
    GAME_STATE_BLACK_WON,
    GAME_STATE_WHITE_WON,
    GAME_STATE_TIE
} GameState;

typedef enum t_symbols {
    SYMBOL_PAWN = 'm',
    SYMBOL_BISHOP = 'b',
    SYMBOL_ROOK = 'r',
    SYMBOL_KNIGHT = 'n',
    SYMBOL_QUEEN = 'q',
    SYMBOL_KING = 'k',
} Symbol;


typedef struct t_chess_settings{
    int gameModeUsersNum;   // 1 - 1 user vs. Comp , 2 - 2 users.
    int difLevel;           // 1-4 regular, 5 - bonus expert.
    Color user1Color;       // Color for user number 1.

} ChessSettings;



typedef struct t_chess_game {
    GameBoard gameBoard;
    ChessSettings settings;
    Color currPlayer;
    ArrayList* history;
    GameState gameState;
    Piece* whiteKing;
    Piece* blackKing;
} ChessGame;



/**
 *
 * @return pointer to ArrayList of all PieceTypes in chess
 */
ArrayList* initializePieceTypes();

/**
 *
 * @param game - our game
 * @param table = board to load our game from it
 */
void loadTable(ChessGame* game, char table[CHESS_N_COLUMNS][CHESS_N_ROWS]);

/**
 *
 * @param pieceTypes - Array List of piece types
 * @param symbol - char that represent piece
 * @return pointer to piece type according to its symbol
 */
PieceType* getPieceTypeFromSymbol(ArrayList* pieceTypes ,char symbol);

/**
 *
 * @param symbol - char of piece
 * @return Color: White if lower case, BLACK if upper case
 */
Color getColorFromSymbol(char symbol);

/**
 * @return opposite color of @param color
 */
Color getOppositeColor(Color color);

/**
 * switch current player in @param game
 */
void flipCurrentPlayer(ChessGame* game);

/**
 * @return color of @param piece
 */
Color getPieceColor(Piece* piece);

/**
 * @return upperCase of @param lowCase
 */
char toUppercase(char lowCase);

/**
 * set pointers in @param game to white & black kings
 */
void setKings(ChessGame* game);


// ================= creator pieceTypes ============


/**
 * @return pieceType of pawn
 */
PieceType createPawnType();

/**
 * @return pieceType of bishop
 */
PieceType createBishopType();

/**
 * @return pieceType of rook
 */
PieceType createRookType();

/**
 * @return pieceType of knight
 */
PieceType createKnightType();

/**
 * @return pieceType of queen
 */
PieceType createQueenType();

/**
 * @return pieceType of king
 */
PieceType createKingType();


// ================= step Handlers ============


/**
 * update @param stepDialog->threatType to OnlyEmptyCellBlockAble
 */
void stepHandlerOnlyEmptyCellsNonSkip(StepDialog *stepDialog, const GameBoard *board);

/**
 * update @param stepDialog->threatType to OnlyEnemyPieceBlockAble
 */
void stepHandlerOnlyEnemyPieceNonSkip(StepDialog *stepDialog, const GameBoard *board);

/**
 * update @param stepDialog->threatType to EnemyPieceOrEmptyCellBlockAble
 */
void stepHandlerEnemyPieceOrEmptyCellNonSkip(StepDialog *stepDialog, const GameBoard *board);

/**
 * update @param stepDialog->threatType to NoThreatSkippable
 */
void stepHandlerNoThreatCanSkip(StepDialog *stepDialog, const GameBoard *board);

/**
 * special handler for Pawn 2 Up Opnening
 */
void stepHandlerPawn2UpOpenning(StepDialog* stepDialog, const GameBoard* board);

#endif /* SOURCE_CHESSGAME_CHESSGAMEBOARD_H_ */
