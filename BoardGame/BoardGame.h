

/**
 * REPRESENTS A GENERAL BOARD GAME
 * A ChessGame is a BoardGame
 */



#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../DataStructs/ArrayList.h"
#include "../IO/BasicErrorsHandling.h"

#ifndef CHESSPROG_BOARDGAME_H
#define CHESSPROG_BOARDGAME_H



// === CONSTS ===

#define DIMS_NUM 2
#define XY_ORIENT_NUM 8
#define INF (-1)



// define threat type of piece on cells
// also, define if threat can be blocked by non-empty cell or can skip it
typedef enum t_threat_type{
	THREAT_TYPE_NO_THREAT_SKIPPABLE,
    THREAT_TYPE_NO_THREAT_BLOCKABLE,
	THREAT_TYPE_ONLY_ENEMY_PIECE_BLOCKABLE,
	THREAT_TYPE_ONLY_EMPTY_CELL_BLOCKABLE,
	THREAT_TYPE_ENEMY_PIECE_OR_EMPTY_CELL_BLOCKABLE,
} ThreatType;


// === GRID, AXIS AND ORIENTATION BASICS ===


// Vector - considered in absolute axis.
// A Coordinate is a vector from the axis origin.
typedef struct t_vector{
    int v[DIMS_NUM];
} Vector;


// Specific XY implementations
typedef enum t_orientationXY {
    DIR_XY_UP,          //0
    DIR_XY_DIAG_UR,     //1
    DIR_XY_RIGHT,       //2
    DIR_XY_DIAG_DR,     //3
    DIR_XY_DOWN,        //4
    DIR_XY_DIAG_DL,     //5
    DIR_XY_LEFT,        //6
    DIR_XY_DIAF_UL      //7
} OrientXY;


// Action to do with threats we check move
typedef enum t_threatAction {
    THREAT_ACTION_NOTHING,
    THREAT_ACTION_CONTINUE_PATH,
    THREAT_ACTION_DELETE_PATH,
    THREAT_ACTION_RETURN_MOVES
} ThreatAction;


typedef enum t_color{
    BLACK = 0, // for safety
    WHITE
} Color;


// === MOVES AND THEIR PROPERTIES ===

/**
 * Hierarchy Overview:
 * A PieceType can have a MoveSet, which is a set of Moves.
 * A Move is a unit of action a piece can do on a board.
 * A Move comprises of Strides.
 * Strides compose of finite or infinite number of steps.
 * Stride can end on a final destination (target) or not,
 * they can have conditionals like collisions with other units
 * or having started from certain board positions.
 * Therefore a set of strides can generate either non, single or many move targets.
 * ThreatPath constitute all piece Moves with appropriate Orientations.

 * Example:
 * 1. chess Knight move:
 *      [(0,1), len 2, isTarget=0] , [(0,1), isTarget=1]
 *          "skip over 2 cells up "..              "and 1 cell right".
 * 2. chess Pawn eating move:
 *      [(1,1), isTarget=1, targetIsEnemy]
 *          "1 cell diagonally right, only when eating enemy piece"
 * 3. chess Rook move:
 *      [(0,1), isTarget=1, Repeats=INF,
 *
 *
 *
 *
*/


// Repeats Attribute - for repeating patterns (e.g vectors, steps)
typedef int Repeats;


// Move : A Specific Sequence of Steps
typedef struct t_move{

    // Flags
    bool enabled; // Some moves are only enabled by cells or game conditions

    // Core Pattern
    ArrayList* strides;

    bool validRotationsXY[XY_ORIENT_NUM]; // additional pattern rotations, i.e. same move in different orientation
} Move;









// === PIECES AND THEIR PROPERTIES ===


// PieceType - defining the type of the piece
typedef struct t_pieceType{
    char nameId; // symbol
    ArrayList* moveSet;
	int threatPathsNum; // max num of threatPaths for pieces in this type
} PieceType;

// Piece : Actual piece instance in the game
// Use attr to add any required attributes for your specific game
typedef struct t_piece{
    struct t_pieceType* type;
    bool    onBoard;
    Color   color;
    struct t_cell* currentCell;
    ArrayList* threatPaths;
} Piece;



typedef struct t_threat_path {
    const Piece* originPiece;
    const Move* move;	// Move that creates the threat
    OrientXY orientXY;		// Move orientation


    //ArrayList* cellsThreatened;
} ThreatPath;



// === BOARD COMPONENTS ===

/**
 * Hierarchy of Board:
 * GameBoard: Cells on the board, and list of all pieceTypes.
 * Cell: PosOnBoard, pointer to piece on it (if exist) and its CellCrossing.
 * CellCrossing: a cross between threatPath to cell.
 * contains pointer to threatPath and cell location on this threat.
 * also, CellCrossing contain the type of threat upon cell.
 */


typedef int Dims[DIMS_NUM];

typedef struct t_gameBoard{     // The plain board of the game
	Dims dims;
	struct t_cell** board;
	ArrayList* pieceTypes;
} GameBoard;


typedef struct t_cell{
	Vector	posOnBoard;
	Piece*	piece;
	ArrayList* CellCrossings; // list of threats pointers
} Cell;



typedef struct t_cell_crossing {
	const ThreatPath* threatPath;
	int strideIndex;
	int inStrideStepIndex;
	ThreatType threatType;
} CellCrossing;



// === PATH REPORTS ===


// A struct to exchange stride information with the user.
// Allows user-scope restricted-handling of stride progression for piece's move.
// Uses: animation, collision handling, --move's stride conditionals--, notifications etc.
typedef struct t_stepDialog{
    const struct t_stride* stride;
    Vector origin, curCoord, nextCoord;
    ThreatType threatType;
    bool stopWalking;
    bool isStepMarkedAsTarget;
} StepDialog;




// Stride : The minimal unit of traversal in a move.
//      Considered from a piece's orientation. Can consist of several cells.
typedef struct t_stride{

    // Basics
    OrientXY direction;  // Vector Direction (unitVector)
    int strideLen;               // Vector Size. for INFINITE use const INF.

    // Flags
    bool isTarget;     // Is the ending position valid as a Final Target for the move

    // Stride Handler
    // User-provided method to check conditions, gather information etc.
    void (*stepHandler)(StepDialog* stepDialog, const GameBoard* board);

} Stride;







// === FUNCTION HEADERS ===


// ================= INITIALIZERS & DESTROYERS ======================


/**
 * x - length of Row in board
 * y - length of Column in board
 * maxThreats - maximum number of threats on single cell
 * pieceTypes - piece types of board
 * func initialized given gameBoard and allocate its cells
 * in addition, allocates Array Lists of CellCrossing and assign them to cells
 * return - pointer to the game board
 */
GameBoard* initializeBoardGame(int x, int y, int maxThreats, GameBoard* gameBoard, ArrayList* pieceTypes);


/**
 * initialize piece in given type and coord accordingly
 * update threats accordingly
 * preq: type & board aren't NULL; coord is empty.
 */
Piece* initializePiece(PieceType* type, const GameBoard* board, const Vector* coord, Color color);


/**
 * destroy all elements in board: pieces, pieceTypes, Cells and CellCrossings.
 * func DO NOT destroy board itself
 */
void destroyAllElementsInBoard(GameBoard *board);


/**
 *  destroy all pieceTypes in board.
 */
void destroyPieceTypeElements(PieceType* pieceType);

/**
 * destroy all pieces upon board.
 */
void destoryBoardPieces(GameBoard* board);

/**
 * remove piece and destroy it
 */
void destroyPiece(Piece* piece, const GameBoard* board);




// ======= MOVES ================

/**
 * put given piece in given coord, and update its attributes and cellCrossings accordingly
 */
void putPiece(Piece* piece, const GameBoard* board, const Vector* coord);


/**
 * remove given piece from board
 * and continue CellCrossings accordingly
 */
void removePiece(Piece* piece, const GameBoard* board);




// ======= HANDLING THREATS, CELL CROSSINGS AND CHECK IF MOVE POSSIBLE +=============


/**
 * action = THREAT_ACTION_CONTINUE_PATH:
 * create cellCrossings from given piece according to its threatPaths.
 * preq: piece's cellCrossings weren't created before
 *
 * action = THREAT_ACTION_DELETE_PATH:
 * delete cellCrossings from given piece according to its threatPaths.
 * preq: piece's cellCrossings already created
 */
void createOrDeletePieceThreats(Piece* piece, const GameBoard* board, ThreatAction action);


/**
 * traverse on all pieces threatPaths.
 *
 * action = THREAT_ACTION_CONTINUE_PATH:
 * create cellCrossings from given coord according to its threatPaths.
 * preq: piece's cellCrossings weren't created before
 *
 * action = THREAT_ACTION_DELETE_PATH:
 * delete cellCrossings from given coord according to its threatPaths.
 * preq: piece's cellCrossings already created
 *
 * action = THREAT_ACTION_RETURN_MOVES:
 * return ArrayList of coord of cell, that piece in origin can move to them.
 */
ArrayList* getAllPathReport(const Vector *origin, const GameBoard *board, ThreatAction status);


/**
 * traverse from origin on given threatPath,
 *
 * action = THREAT_ACTION_CONTINUE_PATH:
 * create cellCrossings from given coord according to given threatPaths.
 * preq: piece's cellCrossings weren't created before
 *
 * action = THREAT_ACTION_DELETE_PATH:
 * delete cellCrossings from given coord according to given threatPath.
 * preq: piece's cellCrossings already created
 *
 * action = THREAT_ACTION_RETURN_MOVES:
 * preq: given an ArrayList of Vectors.
 * update ArrayList of coord of cell, that piece in origin can move to them according to given threatPath.
 */
void stepWalkerBeginningOfMove(const Vector* origin, const ThreatPath* threatPath, const GameBoard* board, ThreatAction action ,ArrayList* movesCoord);


/**
 * traverse from startCoord on given threatPath, and given position on this path,
 *
 * action = THREAT_ACTION_CONTINUE_PATH:
 * create cellCrossings from given coord according to given threatPaths and position.
 * preq: piece's cellCrossings weren't created before
 *
 * action = THREAT_ACTION_DELETE_PATH:
 * delete cellCrossings from given coord according to given threatPath and position.
 * preq: piece's cellCrossings already created
 *
 * action = THREAT_ACTION_RETURN_MOVES:
 * preq: given an ArrayList of Vectors.
 * update ArrayList of coord of cell, that piece in origin can move to them according to given threatPath and position.
 *
 * @param startCoord - coord from where we start walking.
 * @param threatPath - threatPath of walk.
 * @param startStrideIndex - index of the stride we start from.
 * @param inStrideStepIndex - index of the step we start in the first stride.
 * @return
 */
void stepWalkerAdvanced(const Vector* startCoord, const ThreatPath* threatPath,
		int startStrideIndex, int inStrideStepIndex, const GameBoard* board, ThreatAction action, ArrayList* movesCoord);




/**
 * manage threatCrossing of cell, according to action:
 *
 * action = THREAT_ACTION_CONTINUE_PATH:
 * create cellCrossings in given cell to given threatPath.
 * preq: piece's cellCrossings weren't created before
 *
 * action = THREAT_ACTION_DELETE_PATH:
 * delete cellCrossings in given cell to given threatPath.
 * preq: piece's cellCrossings already created
 */
void manageThreatCrossing(Cell* cell, const ThreatPath* threatPath, ThreatAction action, int strideIndex, int inStrideStepIndex, ThreatType threatType);

/**
 * check if move from srcCoord to destCoord is possible.
 * func checks according to cellCrossings and its threatTypes.
 * NOTE: func check only if piece should be able to do the move. So it DOESN'T check if move make the board illegal.
 * i.e. move that put own player's king in risk could be possible, but still illegal.
 */
bool isPossibleMove(const Vector* srcCoord, const Vector* destCoord, const GameBoard* board);

/**
 * return if given cells hold pieces with different color
 */
bool cellsHoldEnemies(Cell* cell1, Cell* cell2);

/**
 * check if cell in given coord contain piece of given color
 */
bool isExistsPieceColor(const Vector* coord, const GameBoard* board, Color color);


/**
 * return pointer to cell in gameBoard, in given coordination
 * if coordination not in the board, then return NULL
 */
Cell* cellInCoord(const Vector *coord, const GameBoard *gameBoard);


/**
 * Checks if coord is inside the boards borders
 */

bool isCoordInBoard(const Vector* coord, const GameBoard* board);


/**
 * Returns direction vector for specified xy direction
 */

Vector getDirVec(OrientXY dir, OrientXY ori);

/**
 * flip orientation in 180 degrees. i.e. up is down.
 */
OrientXY flipOrient(OrientXY orient);

/**
 * return a+b
 */
int add(int a, int b);

/**
 * make given operation (*f) on vectors, and return result.
 */
Vector vectorOp(const Vector* a, const Vector* b, int (*f)(int a, int b));

/**
 * return if cell is empty
 */
bool isCellEmpty(Cell* cell);

/**
 * return Vector that represent the index-cell position in board
 */
Vector getCoordFromArrayPos(GameBoard* board, int index);

/**
 * return index board, of given cell position
 */
int getArrayPosFromCoord(GameBoard* board, Vector* coord);

#endif //CHESSPROG_BOARDGAME_H
