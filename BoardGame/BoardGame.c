#include "BoardGame.h"



static const Vector VECDIRS[XY_ORIENT_NUM] = {
        {{ 0, 1}} , {{ 1, 1}} , {{ 1, 0}} , {{ 1,-1}},
        {{ 0,-1}} , {{-1,-1}} , {{-1, 0}} , {{-1, 1}}
};



GameBoard* initializeBoardGame(int x, int y, int maxThreats, GameBoard* gameBoard, ArrayList* pieceTypes) {
    if (gameBoard == NULL) return NULL;
    // Allocation cells
    Cell* p = (Cell*) calloc((size_t) x*y, sizeof(Cell));
    if (p == NULL) return NULL;

    gameBoard->board = (Cell**) calloc((size_t) y, sizeof(Cell*));
    if (gameBoard->board == NULL) {
        free(p);
        return NULL;
    }
    // Assign the matrix in contiguous block
    for (int i = 0; i < x; i++) {
        gameBoard->board[i] = p+i*y;
    }

    // Allocate ThreatsStepIndex && initialize PosOnBoard of cells
    for (int col = 0; col < x; col++) {
        for (int row = 0; row < y; row++) {
            Cell* cell = &gameBoard->board[col][row];
            cell->posOnBoard = (Vector) {{col, row}};
            cell->CellCrossings = ArrayListCreate(maxThreats, sizeof(CellCrossing));
            if (cell->CellCrossings == NULL) return NULL;
        }
    }


    // Assign dimensions
    gameBoard->dims[0] = x;
    gameBoard->dims[1] = y;

    // assign pieceTypes
    gameBoard->pieceTypes = pieceTypes;

    return gameBoard;
}



Piece* initializePiece(PieceType* type, const GameBoard* board, const Vector* coord, Color color) {

    assertAndPrintFuncErr(isCellEmpty(cellInCoord(coord, board)), __func__); // assert destCell is empty


    // create piece
    Piece* piece = (Piece*) malloc(sizeof(Piece));
    if (piece == NULL)
        return NULL;

    piece->type = type;
    piece->color = color;

    // allocate piece threatPaths
    piece->threatPaths = ArrayListCreate(type->threatPathsNum, sizeof(ThreatPath));
    if (piece->threatPaths == NULL) {
        free(piece);
        return NULL;
    }
    // loop on all moves
    for (int i = 0; i < ArrayListSize(type->moveSet); i++) {
        Move* move = (Move*)  ArrayListGetAt(type->moveSet, i);
        // loop on all valid Rotations in move
        for (int rot = 0; rot < XY_ORIENT_NUM; rot++) {
            if (move->validRotationsXY[rot]) {
                OrientXY orientXY =  (OrientXY) rot;
                if (color == BLACK) {
                    orientXY = flipOrient(orientXY);
                }
                ThreatPath threatPath = {piece, move,  orientXY};
                ArrayListAddLast(piece->threatPaths, &threatPath);
            }
        }
    }
    assertAndPrintFuncErr(ArrayListIsFull(piece->threatPaths), __func__);

    // put piece in coord and update threat accordingly
    putPiece(piece, board, coord);

    return piece;
}

void destroyAllElementsInBoard(GameBoard *board) {

    // destroy all pieces
    destoryBoardPieces(board);

    // destroy crossing allocations
    for (int i = 0; i < board->dims[0]; i++) {
        for (int j = 0; j < board->dims[1]; j++) {
            Cell* cell = &board->board[i][j];
            // ArrayList should be empty because we destory all pieces
            assertAndPrintFuncErr(ArrayListIsEmpty(cell->CellCrossings), __func__);
            ArrayListDestroy(cell->CellCrossings);
        }
    }

    free(board->board[0]); // To free the calloc of matrix cells we allocated
    free(board->board);

    // destroy pieceTypes and their elements
    for (int i = 0; i < ArrayListSize(board->pieceTypes); i++) {
        PieceType* type = (PieceType*) ArrayListGetAt(board->pieceTypes, i);
        destroyPieceTypeElements(type);
    }
    ArrayListDestroy(board->pieceTypes);
}


void destroyPieceTypeElements(PieceType* pieceType) {
    if (pieceType == NULL) return;
    for (int i = 0; i < ArrayListSize(pieceType->moveSet); i++) {
        Move* move = (Move*) ArrayListGetAt(pieceType->moveSet, i);
        ArrayListDestroy(move->strides);
    }
    ArrayListDestroy(pieceType->moveSet);
}


void destoryBoardPieces(GameBoard* board) {
    // destroy all pieces
    for (int i = 0; i < board->dims[0]; i++) {
        for (int j = 0; j < board->dims[1]; j++) {
            Cell* cell = &board->board[i][j];
            if (!isCellEmpty(cell)) {
                destroyPiece(cell->piece, board);
            }
        }
    }
}


void destroyPiece(Piece* piece, const GameBoard* board) {

    if (piece->onBoard)
        removePiece(piece, board);

    ArrayListDestroy(piece->threatPaths);
    free(piece);
}


void putPiece(Piece* piece, const GameBoard* board, const Vector* coord) {
    Cell* cell = cellInCoord(coord, board);
    assertAndPrintFuncErr(isCellEmpty(cell), __func__); // assert pos was empty
    // put piece in appropriate cell
    piece->currentCell = cell;
    cell->piece = piece;
    piece->onBoard = true;

    // delete relevant threats
    // In chess, when we continue path extension, we don't need to check piece Color
    // foreach cross in cell
    for (int i = 0; i < ArrayListSize(cell->CellCrossings); i++) {
        CellCrossing* cross = ArrayListGetAt(cell->CellCrossings, i);
        stepWalkerAdvanced(coord,
                           cross->threatPath, cross->strideIndex, cross->inStrideStepIndex,
                           board, THREAT_ACTION_DELETE_PATH, NULL);
    }

    // create piece's threats
    createOrDeletePieceThreats(piece, board, THREAT_ACTION_CONTINUE_PATH);
}


void removePiece(Piece* piece, const GameBoard* board) {
    createOrDeletePieceThreats(piece, board, THREAT_ACTION_DELETE_PATH);
    piece->onBoard = false;

    Cell* cell = piece->currentCell;
    cell->piece = NULL;

    // continue other pieces' threatPaths
    for (int i = 0; i < ArrayListSize(cell->CellCrossings); ++i) {
        CellCrossing* cross = ArrayListGetAt(cell->CellCrossings, i);
        stepWalkerAdvanced(&cell->posOnBoard, cross->threatPath,
                           cross->strideIndex, cross->inStrideStepIndex, board, THREAT_ACTION_CONTINUE_PATH, NULL);
    }
}




void createOrDeletePieceThreats(Piece* piece, const GameBoard* board, ThreatAction action) {
    assertAndPrintFuncErr(piece != NULL, __func__);
    Vector* vec = &piece->currentCell->posOnBoard;
    getAllPathReport(vec, board, action);
}







ArrayList* getAllPathReport(const Vector *origin, const GameBoard *board, ThreatAction action){

    ArrayList* movesCoord = NULL;
    if (action == THREAT_ACTION_RETURN_MOVES) {
        int cellNum = board->dims[0]*board->dims[1];
        movesCoord = ArrayListCreate(cellNum, sizeof(Vector));
        assertAndPrintFuncErr(movesCoord != NULL, __func__);
    }

    Piece* piece = cellInCoord(origin, board)->piece;
    assertAndPrintFuncErr(piece != NULL, __func__);
    // Foreach path, collect steps report
    for (int i=0; i<ArrayListSize(piece->threatPaths); i++){
        ThreatPath* path = (ThreatPath*) ArrayListGetAt(piece->threatPaths, i);
        stepWalkerBeginningOfMove(origin, path, board, action, movesCoord);
    }

    return movesCoord;
}


void stepWalkerBeginningOfMove(const Vector* origin, const ThreatPath* threatPath, const GameBoard* board,
                              ThreatAction action, ArrayList* movesCoord){

    stepWalkerAdvanced(origin, threatPath, 0, 0, board, action, movesCoord);
}



void stepWalkerAdvanced(const Vector* startCoord, const ThreatPath* threatPath,
		int startStrideIndex, int inStrideStepIndex, const GameBoard* board, ThreatAction action, ArrayList* movesCoord) {
    // init a stride dialog struct for control and exchange with user.

    StepDialog sDiag;
    sDiag.origin = threatPath->originPiece->currentCell->posOnBoard; // coord of the relevant piece
    sDiag.curCoord = *startCoord;
    sDiag.nextCoord = *startCoord;
    sDiag.stopWalking = false;

    // check origin is legal
    assertAndPrintFuncErr(isCoordInBoard(&sDiag.origin, board), __func__);


    // init Loop Vars
    Vector dirVec;
    int vecLen = 0;
    const Move* move = threatPath->move;
    OrientXY orientXY = threatPath->orientXY;


    if (action == THREAT_ACTION_RETURN_MOVES)
        assertAndPrintFuncErr(movesCoord != NULL, __func__);

    // == foreach STRIDE (VECTOR) in the move pattern:
    for (int i=startStrideIndex; i<ArrayListSize(move->strides); i++) {

    	sDiag.stride = (Stride*) ArrayListGetAt(move->strides, i);

        // load the xy-rotated stride unit-vector for traversal
        dirVec = getDirVec(sDiag.stride->direction, orientXY);

        // set vector length counter
        vecLen = sDiag.stride->strideLen; // reminder: const INF means repeat vector endlessly
        if (i != startStrideIndex) inStrideStepIndex = 0; //inStrideStepIndex only relevant to first stride
        if (vecLen != INF)
        	vecLen -= inStrideStepIndex;  // Adjust for start step pos in first stride

        // foreach stride in STRIDE (vector) LENGTH:
        while (vecLen != 0) {

            // counter for vector size
            if (vecLen != INF) vecLen--;
            inStrideStepIndex++;

            sDiag.isStepMarkedAsTarget = ((vecLen == INF || vecLen == 0) &&
                    sDiag.stride->isTarget);

            // CHECK NEXT COORD:

            // calc next coordinate of this stride
            sDiag.nextCoord = vectorOp(&sDiag.curCoord, &dirVec, add);

            // check OffBoard / 'Border Collision'
            if (!isCoordInBoard(&sDiag.nextCoord, board)) return;

            // update and Exchange StepDialog with user
            sDiag.stride->stepHandler(&sDiag, board);

            sDiag.curCoord = sDiag.nextCoord;
            // Progressed single cell in board

            if (sDiag.threatType != THREAT_TYPE_NO_THREAT_SKIPPABLE) {
            	if (action == THREAT_ACTION_CONTINUE_PATH || action == THREAT_ACTION_DELETE_PATH) {
            		manageThreatCrossing(cellInCoord(&sDiag.curCoord, board),
            				threatPath, action, i, inStrideStepIndex, sDiag.threatType);
                    //TODO change inStrideStepIndex to correct value
            	}

            	else if (action == THREAT_ACTION_RETURN_MOVES) {
                    if (isPossibleMove(&sDiag.origin, &sDiag.curCoord, board)) {
                        ArrayListAddFirst(movesCoord, &sDiag.curCoord);
                    }
            	}
            }


            // user Decided if the stride is valid or not.
            // ** NOTE : an invalid stride terminates the entire stride sequence.
            if (sDiag.stopWalking) return;

        }




        // Finished a stride


    } // Finished all strides in the sequence.
}


void manageThreatCrossing(Cell* cell, const ThreatPath* threatPath, ThreatAction action, int strideIndex, int inStrideStepIndex, ThreatType threatType) {

    switch(action) {
	case THREAT_ACTION_CONTINUE_PATH:
		// make sure we didn't already create that CellCross
		for (int i = 0; i < ArrayListSize(cell->CellCrossings); i++) {
			CellCrossing* cross = (CellCrossing*) ArrayListGetAt(cell->CellCrossings, i);
            assertAndPrintFuncErr(cross->threatPath != threatPath, __func__);

		}
		// create the Cell Cross
		CellCrossing crossToAdd = {threatPath, strideIndex, inStrideStepIndex, threatType};
		ArrayListAddLast(cell->CellCrossings, &crossToAdd);
		return;

	case THREAT_ACTION_DELETE_PATH:
		for (int i = 0; i < ArrayListSize(cell->CellCrossings); i++) {
			CellCrossing* crossToRemove = (CellCrossing*) ArrayListGetAt(cell->CellCrossings, i);
			if (crossToRemove->threatPath == threatPath) {
                ArrayListRemoveAt(cell->CellCrossings, i);
                return;
			}
		}
            printErr(ERR_FUNC_FAILED, __func__); // we shouldn't got here
		break;

    default:
        printErr(ERR_FUNC_FAILED, __func__); // we shouldn't got here
    }
}


bool isPossibleMove(const Vector* srcCoord, const Vector* destCoord, const GameBoard* board) {

    Cell* srcCell = cellInCoord(srcCoord, board);
    if (isCellEmpty(srcCell))
        return false;

    Cell* destCell = cellInCoord(destCoord, board);
    for (int i = 0; i < ArrayListSize(destCell->CellCrossings); i++) {
        CellCrossing* cross = (CellCrossing*) ArrayListGetAt(destCell->CellCrossings, i);
        if (cross->threatPath->originPiece == srcCell->piece) {
            switch (cross->threatType) {
                case THREAT_TYPE_ONLY_EMPTY_CELL_BLOCKABLE:
                    if (isCellEmpty(destCell)) return true;
                    break;
                case THREAT_TYPE_ONLY_ENEMY_PIECE_BLOCKABLE:
                    if (cellsHoldEnemies(srcCell, destCell)) return true;
                    break;
                case THREAT_TYPE_ENEMY_PIECE_OR_EMPTY_CELL_BLOCKABLE:
                    if (isCellEmpty(destCell) || cellsHoldEnemies(srcCell, destCell))
                        return true;
                    break;
                default:
                    break;
            }
        }
    }

    return false;
}

bool cellsHoldEnemies(Cell* cell1, Cell* cell2) {
    if (cell1 == NULL || cell2 == NULL) {
        printErr(ERR_FUNC_FAILED, __func__);
        return false;
    }
    if (isCellEmpty(cell1)|| isCellEmpty(cell2)) return false;
    return (cell1->piece->color != cell2->piece->color);
}


bool isExistsPieceColor(const Vector* coord, const GameBoard* board, Color color) {
	Piece* piece = cellInCoord(coord, board)->piece;
	if (piece == NULL) return false;

	return (piece->color == color);
}


Cell* cellInCoord(const Vector *coord, const GameBoard *gameBoard) {
    if (!isCoordInBoard(coord, gameBoard)) return NULL;
    int x = coord->v[0];
    int y = coord->v[1];
    Cell* cell = &gameBoard->board[x][y];
    return cell;
}




bool isCoordInBoard(const Vector* coord, const GameBoard* board) {
    for (int i=0; i<DIMS_NUM; i++)
        if (coord->v[i] < 0|| coord->v[i] >= board->dims[i]) return false;

    return true;
}


Vector getDirVec(OrientXY dir, OrientXY ori){
    return VECDIRS[(dir + ori) % 8];
}

OrientXY flipOrient(OrientXY orient) {
    return ((orient + DIR_XY_DOWN) % 8);
}

int add(int a, int b){return a+b;}

Vector vectorOp(const Vector* a, const Vector* b, int (*f)(int a, int b)){
    Vector res = {{0,0}};
    for (int i=0; i<DIMS_NUM; i++){
        res.v[i] = f(a->v[i],b->v[i]);
    }
    return res;
}


bool isCellEmpty(Cell* cell) {
	if (cell == NULL) {
		printErr(ERR_FUNC_FAILED, __func__);
		return false;
	}
	else
		return cell->piece == NULL;
}



Vector getCoordFromArrayPos(GameBoard* board, int index) {
	int x = index / board->dims[1];
	int y = index % board->dims[1];
	return (Vector) {{x,y}};
}

int getArrayPosFromCoord(GameBoard* board, Vector* coord) {
	int x = coord->v[0];
	int y = coord->v[1];
	return x*board->dims[1] + y;
}

