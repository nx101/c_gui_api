
#include "ChessGameBoard.h"


ArrayList* initializePieceTypes() {

    ArrayList* pieceTypes = ArrayListCreate(PIECE_TYPES_NUM, sizeof(PieceType));

    if (pieceTypes == NULL)
        return NULL;

    // configure the pieceTypes
    PieceType pawn = createPawnType();
    PieceType bishop = createBishopType();
    PieceType rook = createRookType();
    PieceType knight = createKnightType();
    PieceType queen = createQueenType();
    PieceType king = createKingType();

    if (pawn.nameId != SYMBOL_PAWN || bishop.nameId != SYMBOL_BISHOP || rook.nameId != SYMBOL_ROOK ||
        knight.nameId != SYMBOL_KNIGHT || queen.nameId != SYMBOL_QUEEN || king.nameId != SYMBOL_KING) {
        printErr(ERR_FUNC_FAILED, __func__);
        destroyPieceTypeElements(&pawn);
        destroyPieceTypeElements(&bishop);
        destroyPieceTypeElements(&rook);
        destroyPieceTypeElements(&knight);
        destroyPieceTypeElements(&queen);
        destroyPieceTypeElements(&king);

        free(pieceTypes);
        return NULL;
    }

    // add the pieceTypes to arrayList
    ArrayListAddLast(pieceTypes, &pawn);
    ArrayListAddLast(pieceTypes, &bishop);
    ArrayListAddLast(pieceTypes, &rook);
    ArrayListAddLast(pieceTypes, &knight);
    ArrayListAddLast(pieceTypes, &queen);
    ArrayListAddLast(pieceTypes, &king);


    return pieceTypes;
}



void loadTable(ChessGame* game, char table[CHESS_N_COLUMNS][CHESS_N_ROWS]) {

	// clean board from pieces
	destoryBoardPieces(&game->gameBoard);

	for (int row = 0; row < CHESS_N_ROWS; row++) {
		for (int col = 0; col < CHESS_N_COLUMNS; col++) {
			PieceType* type = getPieceTypeFromSymbol(game->gameBoard.pieceTypes, table[row][col]);
			if (type != NULL) {
                Color color = getColorFromSymbol(table[row][col]);
				Vector vec = {{col,row}};
				initializePiece(type, &game->gameBoard, &vec, color);
			}
		}
	}
}





PieceType* getPieceTypeFromSymbol(ArrayList* pieceTypes ,char symbol) {
    assertAndPrintFuncErr(pieceTypes != NULL, __func__);
    if (symbol == '_') return NULL;

    for (int i = 0; i < ArrayListSize(pieceTypes); i++) {
        PieceType* type = (PieceType*) ArrayListGetAt(pieceTypes, i);
        char whiteSymbol = type->nameId;
        if (symbol == whiteSymbol || symbol == toUppercase(whiteSymbol)) {
            return type;
        }
    }
    return NULL;
}

Color getColorFromSymbol(char symbol) {
    if ('a' <= symbol && symbol <= 'z')
        return WHITE;
    if ('A' <= symbol && symbol <= 'Z')
        return BLACK;

    printErr(ERR_FUNC_FAILED, __func__);
    return WHITE;
}


Color getOppositeColor(Color color) {
	switch (color){
        case BLACK:
            return WHITE;

        case WHITE:
		    return BLACK;
	}

	printErr(ERR_FUNC_FAILED, __func__);
	return WHITE;
}

void flipCurrentPlayer(ChessGame* game) {
    game->currPlayer = getOppositeColor(game->currPlayer);
}

Color getPieceColor(Piece* piece) {
	assertAndPrintFuncErr(piece != NULL, __func__);
	return piece->color;
}


char toUppercase(char lowCase) {
    assertAndPrintFuncErr('a' <= lowCase && lowCase <= 'z', __func__);
    return lowCase-32;
}


void setKings(ChessGame* game) {
	for (int row = 0; row < CHESS_N_ROWS; row++) {
		for (int col = 0; col < CHESS_N_COLUMNS; col++) {
			Vector vec = {{row, col}};
			Cell* cell = cellInCoord(&vec, &game->gameBoard);
			if (!isCellEmpty(cell)) {
				if (cell->piece->type->nameId == SYMBOL_KING) {
					if (cell->piece->color == WHITE)
						game->whiteKing = cell->piece;
					else
						game->blackKing = cell->piece;
				}
			}
		}
	}
}



// ==== Define PieceTypes

PieceType createPawnType() {
    int movesNum = 3;
    ArrayList* moveSet = ArrayListCreate(movesNum, sizeof(Move));
    if (moveSet == NULL)
        return (PieceType) NULL_PIECE_TYPE;

    int strideNum;

    // 1 UP
    strideNum = 1;
    ArrayList* strides1 = ArrayListCreate(strideNum, sizeof(Stride));
    if (strides1 == NULL) {
        free(moveSet);
        return (PieceType) NULL_PIECE_TYPE;
    }
    Stride stride1 = {DIR_XY_UP, 1, true, stepHandlerOnlyEmptyCellsNonSkip};
    ArrayListAddFirst(strides1, &stride1);
    Move move1 = {true, strides1, {1,0,0,0,0,0,0,0}};
    ArrayListAddFirst(moveSet, &move1);

    // 1 Diagonal
    strideNum = 1;
    ArrayList* strides2 = ArrayListCreate(strideNum, sizeof(Stride));
    if (strides2 == NULL) {
        free(strides1);
        free(moveSet);
        return (PieceType) NULL_PIECE_TYPE;
    }
    Stride stride2 = {DIR_XY_UP, 1, true, stepHandlerOnlyEnemyPieceNonSkip};
    ArrayListAddFirst(strides2, &stride2);
    Move move2 = {true, strides2, {0,1,0,0,0,0,0,1}};
    ArrayListAddFirst(moveSet, &move2);

    // 2 Up on conditional
    strideNum = 1;
    ArrayList* strides3 = ArrayListCreate(strideNum, sizeof(Stride));
    if (strides3 == NULL) {
        free(strides2);
        free(strides1);
        free(moveSet);
        return (PieceType) NULL_PIECE_TYPE;
    }
    Stride stride3 = {DIR_XY_UP, 2, true, stepHandlerPawn2UpOpenning};
    ArrayListAddFirst(strides3, &stride3);
    Move move3 = {true, strides3, {1,0,0,0,0,0,0,0}};
    ArrayListAddFirst(moveSet, &move3);


    PieceType pieceType = {SYMBOL_PAWN, moveSet, 4};

    return pieceType;
}


PieceType createBishopType() {
    int movesNum = 1;
    ArrayList* moveSet = ArrayListCreate(movesNum, sizeof(Move));
    if (moveSet == NULL)
        return (PieceType) NULL_PIECE_TYPE;

    // Diagonal until stopped
    int strideNum = 1;
    ArrayList* strides = ArrayListCreate(strideNum, sizeof(Stride));
    if (strides == NULL) {
        free(moveSet);
        return (PieceType) NULL_PIECE_TYPE;
    }
    Stride stride = {DIR_XY_UP, INF, true, stepHandlerEnemyPieceOrEmptyCellNonSkip};
    ArrayListAddFirst(strides, &stride);
    Move move = {true, strides, {0, 1, 0, 1, 0, 1, 0, 1}};
    ArrayListAddFirst(moveSet, &move);



    PieceType pieceType = {SYMBOL_BISHOP, moveSet, 4};

    return pieceType;
}


PieceType createRookType() {
    int movesNum = 1;
    ArrayList* moveSet = ArrayListCreate(movesNum, sizeof(Move));
    if (moveSet == NULL)
        return (PieceType) NULL_PIECE_TYPE;

    // Vertical or Horizontal until stopped
    int strideNum = 1;
    ArrayList* strides = ArrayListCreate(strideNum, sizeof(Stride));
    Stride stride = {DIR_XY_UP, INF, true, stepHandlerEnemyPieceOrEmptyCellNonSkip};
    if (strides == NULL) {
        free(moveSet);
        return (PieceType) NULL_PIECE_TYPE;
    }
    ArrayListAddFirst(strides, &stride);
    Move move = {true, strides, {1, 0, 1, 0, 1, 0, 1, 0}};
    ArrayListAddFirst(moveSet, &move);

    PieceType pieceType = {SYMBOL_ROOK, moveSet, 4};

    return pieceType;}


PieceType createKnightType() {
    int movesNum = 2;
    ArrayList* moveSet = ArrayListCreate(movesNum, sizeof(Move));
    if (moveSet == NULL)
        return (PieceType) NULL_PIECE_TYPE;

    int strideNum;

    // 2 step and 1 clockwise
    strideNum = 2;
    ArrayList* strides1 = ArrayListCreate(strideNum, sizeof(Stride));
    if (strides1 == NULL) {
        free(moveSet);
        return (PieceType) NULL_PIECE_TYPE;
    }
    Stride stride1_1 = {DIR_XY_UP, 2, false, stepHandlerNoThreatCanSkip};
    ArrayListAddLast(strides1, &stride1_1);
    Stride stride1_2 = {DIR_XY_RIGHT, 1, true, stepHandlerEnemyPieceOrEmptyCellNonSkip};
    ArrayListAddLast(strides1, &stride1_2);
    Move move1 = {true, strides1, {1, 0, 1, 0, 1, 0, 1, 0}};
    ArrayListAddLast(moveSet, &move1);

    // 2 step and 1 counterclockwise
    strideNum = 2;
    ArrayList* strides2 = ArrayListCreate(strideNum, sizeof(Stride));
    Stride stride2_1 = {DIR_XY_UP, 2, false, stepHandlerNoThreatCanSkip};
    if (strides2 == NULL) {
        free(strides1);
        free(moveSet);
        return (PieceType) NULL_PIECE_TYPE;
    }
    ArrayListAddLast(strides2, &stride2_1);
    Stride stride2_2 = {DIR_XY_LEFT, 1, true, stepHandlerEnemyPieceOrEmptyCellNonSkip};
    ArrayListAddLast(strides2, &stride2_2);
    Move move2 = {true, strides2, {1, 0, 1, 0, 1, 0, 1, 0}};
    ArrayListAddLast(moveSet, &move2);


    PieceType pieceType = {SYMBOL_KNIGHT, moveSet, 8};

    return pieceType;
}


PieceType createQueenType() {
    int movesNum = 1;
    ArrayList *moveSet = ArrayListCreate(movesNum, sizeof(Move));
    if (moveSet == NULL)
        return (PieceType) NULL_PIECE_TYPE;

    // all directions until stopped
    int strideNum = 1;
    ArrayList* strides = ArrayListCreate(strideNum, sizeof(Stride));
    if (strides == NULL) {
        free(moveSet);
        return (PieceType) NULL_PIECE_TYPE;
    }
    Stride stride = {DIR_XY_UP, INF, true, stepHandlerEnemyPieceOrEmptyCellNonSkip};
    ArrayListAddFirst(strides, &stride);
    Move move = {true, strides, {1, 1, 1, 1, 1, 1, 1, 1}};
    ArrayListAddFirst(moveSet, &move);


    PieceType pieceType = {SYMBOL_QUEEN, moveSet, 8};

    return pieceType;
}


PieceType createKingType() {
    int movesNum = 1;
    ArrayList* moveSet = ArrayListCreate(movesNum, sizeof(Move));
    if (moveSet == NULL)
        return (PieceType) NULL_PIECE_TYPE;

    // 1 step in all directions
    int strideNum = 1;
    ArrayList* strides = ArrayListCreate(strideNum, sizeof(Stride));
    if (strides == NULL) {
        free(moveSet);
        return (PieceType) NULL_PIECE_TYPE;
    }
    Stride stride = {DIR_XY_UP, 1, true, stepHandlerEnemyPieceOrEmptyCellNonSkip};
    ArrayListAddFirst(strides, &stride);
    Move move = {true, strides, {1, 1, 1, 1, 1, 1, 1, 1}};
    ArrayListAddFirst(moveSet, &move);


    PieceType pieceType = {SYMBOL_KING, moveSet, 8};

    return pieceType;
}



// ==== STEP HANDDLERS

void stepHandlerOnlyEmptyCellsNonSkip(StepDialog *stepDialog, const GameBoard *board){
	assertAndPrintFuncErr(stepDialog != NULL && board != NULL, __func__);
    stepDialog->threatType = THREAT_TYPE_ONLY_EMPTY_CELL_BLOCKABLE;
    if (!isCellEmpty(cellInCoord(&stepDialog->nextCoord, board)))
        stepDialog->stopWalking = true;
}


void stepHandlerOnlyEnemyPieceNonSkip(StepDialog *stepDialog, const GameBoard *board){
	assertAndPrintFuncErr(stepDialog != NULL && board != NULL, __func__);
	stepDialog->threatType = THREAT_TYPE_ONLY_ENEMY_PIECE_BLOCKABLE;
    if (!isCellEmpty(cellInCoord(&stepDialog->nextCoord, board)))
        stepDialog->stopWalking = true;
}

void stepHandlerEnemyPieceOrEmptyCellNonSkip(StepDialog *stepDialog, const GameBoard *board){
    assertAndPrintFuncErr(stepDialog != NULL && board != NULL, __func__);
    stepDialog->threatType = THREAT_TYPE_ENEMY_PIECE_OR_EMPTY_CELL_BLOCKABLE;
    if (!isCellEmpty(cellInCoord(&stepDialog->nextCoord, board)))
        stepDialog->stopWalking = true;
}

void stepHandlerNoThreatCanSkip(StepDialog *stepDialog, const GameBoard *board){
    assertAndPrintFuncErr(stepDialog != NULL && board != NULL, __func__);
    stepDialog->threatType = THREAT_TYPE_NO_THREAT_SKIPPABLE;
}

void stepHandlerPawn2UpOpenning(StepDialog* stepDialog, const GameBoard* board){
    assertAndPrintFuncErr(stepDialog != NULL && board != NULL, __func__);
    Piece* piece = cellInCoord(&stepDialog->origin, board)->piece;
    Color color = piece->color;
    // Move only allowed for certain origin position on board
    if ((color == WHITE && stepDialog->origin.v[1] == 1) ||
        (color == BLACK && stepDialog->origin.v[1] == 6)) {

        // Origin Pos OK! Report step attributes
        if (stepDialog->isStepMarkedAsTarget) {
            stepDialog->threatType = THREAT_TYPE_ONLY_EMPTY_CELL_BLOCKABLE;
        }
        else {
            stepDialog->threatType = THREAT_TYPE_NO_THREAT_BLOCKABLE;
            if (!isCellEmpty(cellInCoord(&stepDialog->nextCoord, board))) {
                stepDialog->stopWalking = true;
            }
        }
    }
    // Move isn't allowed
    else {
        stepDialog->threatType = THREAT_TYPE_NO_THREAT_SKIPPABLE; // so move wouldn't count in cell crossing
        stepDialog->stopWalking = true;
    }
}
