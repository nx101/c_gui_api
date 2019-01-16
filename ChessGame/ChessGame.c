
#include "ChessGame.h"


ChessGame* createGame() {
	ChessGame* game = (ChessGame*) malloc(sizeof(ChessGame));
	if (game == NULL) return NULL;

	// add piece types
	ArrayList* pieceTypes = initializePieceTypes();
	if (pieceTypes == NULL) {
		free(game);
		return NULL;
	}

	GameBoard* board = initializeBoardGame(CHESS_N_COLUMNS, CHESS_N_ROWS, MAX_THREATS,
			&game->gameBoard, pieceTypes);
	if (board == NULL) { // allocation of elements on board failed
		destroyAllElementsInBoard(&game->gameBoard);
		free(game);
		return NULL;
	}

	// add history for undo commands
	game->history = ArrayListCreate(HISTORY_SIZE, sizeof(History));
	if (game->history == NULL) {
		destroyAllElementsInBoard(&game->gameBoard);
		free(game);
		return NULL;
	}


	return game;
}

void destroyGame(ChessGame* game) {
	ArrayListDestroy(game->history);
	destroyAllElementsInBoard(&game->gameBoard);
	free(game);
}

void initializeGame(ChessGame* game) {
	char startBoard[CHESS_N_COLUMNS][CHESS_N_ROWS] = START_BOARD;
	loadTable(game, startBoard);
	// TODO make sure we printed errors if were

	setKings(game);

	game->settings.gameModeUsersNum = DEFAULT_GAME_MODE;
	game->settings.difLevel = DEFAULT_DIFFOCULTY;
	game->settings.user1Color = DEFAULT_COLOR;
	game->currPlayer = WHITE;

}

COMMAND doSettings(ChessGame* game) {
	printMsgForSettingsState(MSG_SETTINGS_INP_REQ_SETTING_OR_START, __func__);
	char input[MAX_LINE_LENGTH];
	COMMAND cmd;
	while (true) {
		fgets(input, MAX_LINE_LENGTH + 1, stdin); //TODO security
		Command cmdPack = ParseUserInputLine(input, SETTINGS_TIME); //const char* assignment
		cmd = cmdPack.cmd;

		switch (cmd) {

		case COMMAND_GAME_MODE:
			if (cmdPack.isValidArg && (cmdPack.arg == 1 || cmdPack.arg == 2)) {
				game->settings.gameModeUsersNum = cmdPack.arg;

				if (game->settings.gameModeUsersNum == 1)
					printMsgForSettingsState(MSG_SETTINGS_CMD_GAMEMODE_SET_1, __func__);
				else
					printMsgForSettingsState(MSG_SETTINGS_CMD_GAMEMODE_SET_2, __func__);

			}
			else
				printMsgForSettingsState(MSG_SETTINGS_CMD_GAMEMODE_BAD_ARG, __func__);

			continue;


		case COMMAND_DIFFICULTY:
			if (game->settings.gameModeUsersNum == PLAYER_VS_COMP) {
				if (cmdPack.isValidArg && (1 <= cmdPack.arg && cmdPack.arg <= MAX_DIFF)) {
					if (cmdPack.arg == MAX_DIFF)
						printMsgForSettingsState(MSG_SETTINGS_CMD_DIFFICULTY_EXPERT_NOT_SUPPORTED, __func__);
					else
						game->settings.difLevel = cmdPack.arg;
				}
				else
					printMsgForSettingsState(MSG_SETTINGS_CMD_DIFFICULTY_VAL_NOT_IN_RANGE, __func__);
			}
			else	// PLAYER VS PLAYER
				printMsgForSettingsState(MSG_SETTINGS_CMD_DIFFICULTY_INVALID_CMD, __func__);

			continue;


		case COMMAND_USER_COLOR:
			if (game->settings.gameModeUsersNum == PLAYER_VS_COMP &&
					cmdPack.isValidArg && (cmdPack.arg == BLACK || cmdPack.arg == WHITE)) {
				game->settings.user1Color = (Color) cmdPack.arg;
			}
			else
				printMsgForSettingsState(MSG_SETTINGS_CMD_USERCOLOR_INVALID_CMD, __func__);

			continue;


		case COMMAND_LOAD:
			doLoad(game, cmdPack.path);
			continue;


		case COMMAND_DEFAULT:
			game->settings.gameModeUsersNum = DEFAULT_GAME_MODE;
			game->settings.difLevel = DEFAULT_DIFFOCULTY;
			game->settings.user1Color = DEFAULT_COLOR;
			continue;


		case COMMAND_PRINT_SETTING:
			printSetting(game);
			continue;


		case COMMAND_QUIT:
			printMsgForSettingsState(MSG_SETTINGS_QUIT, __func__);
			return cmd;


		case COMMAND_START:
			return cmd;

		default:
			printErr(ERR_INV_CMD, __func__);
			continue;
		}
	}
}



COMMAND doPlayerTurn(ChessGame* game) {

	assertAndPrintFuncErr(game != NULL, __func__); //TODO break from func?
	Color color = game->currPlayer;
	if (game->settings.gameModeUsersNum == PLAYER_VS_COMP) {
		assertAndPrintFuncErr(color == game->settings.user1Color, __func__); // assert is's player turn
	}

	printBoard(&game->gameBoard);

	bool success;

	char input[MAX_LINE_LENGTH];
	COMMAND cmd;
	while (true) {
		printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_INP_REQ_ENTER_MOVE, __func__, color, NULL, NULL);
		//TODO print this after succeeded undo or load?
		fgets(input, MAX_LINE_LENGTH + 1, stdin); //TODO security
		Command cmdPack = ParseUserInputLine(input, GAME_TIME); //const char* assignment
		cmd = cmdPack.cmd;
		switch (cmd) {

		case COMMAND_MOVE:
			success = move(game, cmdPack);
			if (success) return cmd;
			else continue;

		case COMMAND_SAVE:
			saveFile(game, cmdPack.path);
			continue;

		case COMMAND_UNDO:
			undo(game);
			continue;

		case COMMAND_RESET:
			printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_RESET, __func__, color, NULL, NULL);
			return cmd;

		case COMMAND_QUIT:
			printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_QUIT, __func__, color, NULL, NULL);
			return cmd;

		default:
			printErr(ERR_INV_CMD, __func__);
			continue;
		}
	}
}



bool move(ChessGame* game, Command cmd) {
	Vector currPos = {{cmd.currPos[0], cmd.currPos[1]}};
	Vector nextPos = {{cmd.nexPos[0], cmd.nexPos[1]}};
	if (!cmd.isValidArg) { //TODO maybe print something else?
		Color color = game->currPlayer;
		printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_CMD_MOVE_XY_TO_IJ_1_INVALID_POS, __func__,
                                           color, &currPos, &nextPos);
		return false;
	} else {
        /// doing move
		return moveAdvanced(game, &currPos, &nextPos, true);
	}
}


bool moveAdvanced(ChessGame* game, const Vector* prevPos, const Vector* nextPos, bool print) {

	if (isGameFinished(game)) return false;


	GameBoard* board = &game->gameBoard;
	Color playerColor = game->currPlayer;

	// check if position is in board
	if (!isCoordInBoard(prevPos, board) || !isCoordInBoard(nextPos, board)) {
		if (print)
			printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_CMD_MOVE_XY_TO_IJ_1_INVALID_POS, __func__,
                                               playerColor, prevPos, nextPos);
		return false;
	}

	// check if current position contain piece of current player
	if (!isExistsPieceColor(prevPos, board, game->currPlayer)) {
		if (print)
			printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_CMD_MOVE_XY_TO_IJ_2_XY_NOT_PLAYER_PIECE, __func__,
                                               playerColor, prevPos, nextPos);
		return false;
	}

	// check if the move legal
	if (!isPossibleMove(prevPos, nextPos, board)) {
		if (print)
			printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_CMD_MOVE_XY_TO_IJ_3_MOVE_ILLEGAL_FOR_PLAYER_PIECE,
                                               __func__, playerColor, prevPos, nextPos);
		return false;
	}


	// doing move

	// remove piece from previous pos
	Piece* movingPiece = cellInCoord(prevPos, board)->piece;
	removePiece(movingPiece, board);

	// destroy piece in dest pos, if exists
	PieceType* killedPieceType = NULL;
    Piece* destPiece = cellInCoord(nextPos, board)->piece;
	if (destPiece != NULL) {
        killedPieceType = destPiece->type;
		removePiece(destPiece, board);
	}

	// put piece in the next pos
	putPiece(movingPiece, board, nextPos);

	// make sure move didn't make it's own king vulnerable
	if (isKingThreatend(game, playerColor)) {
		if (print)
			printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_CMD_MOVE_XY_TO_IJ_3_MOVE_ILLEGAL_FOR_PLAYER_PIECE,
                                               __func__, playerColor, prevPos, nextPos);
		// undo the move we just did
		removePiece(movingPiece, board);
		putPiece(movingPiece, board, prevPos);

		if (destPiece != NULL) {      // put piece back to its place if we kill it in the move
            putPiece(destPiece, &game->gameBoard, nextPos);
        }

		return false;
	}
    // move is legal
    if (destPiece != NULL)
        destroyPiece(destPiece, &game->gameBoard); // prevent leaks

    // move don't make it's own king vulnerable
	// update history
	History history = {{*prevPos, *nextPos}, killedPieceType};
	if (ArrayListIsFull(game->history)) {
		ArrayListRemoveLast(game->history); // make space in list array
	}
	ArrayListAddFirst(game->history, &history);

	flipCurrentPlayer(game);

    updateGameState(game);

	// if requested: printing about check, check-mate, and TIE
	if (print) {
		switch (game->gameState) {
		case GAME_STATE_WHITE_WON:
			printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_ON_BOARD_UPDATED_GAME_ENDS_CHECKMATE
                    , __func__, WHITE, prevPos, nextPos);
			break;

		case GAME_STATE_BLACK_WON:
			printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_ON_BOARD_UPDATED_GAME_ENDS_CHECKMATE
                    , __func__, BLACK, prevPos, nextPos);
			break;

		case GAME_STATE_TIE:
			printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_ON_BOARD_UPDATED_GAME_ENDS_TIE
                    , __func__, game->currPlayer, prevPos, nextPos);
			break;

		case GAME_STATE_ON:
			if (isKingThreatend(game, game->currPlayer)) {
				printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_ON_BOARD_UPDATED_KING_IN_CHECK
                        , __func__, game->currPlayer, prevPos, nextPos);
			}
			break;
		}
	}


    return true;

}


bool isMoveValid(ChessGame* game, Vector* currPos, Vector* nextPos) {

    GameBoard* board = &game->gameBoard;

    // check if move supplement the basic requirements
    if (!isCoordInBoard(currPos, board) || !isCoordInBoard(nextPos, board) ||
            !isExistsPieceColor(currPos, board, game->currPlayer) ||
            !isPossibleMove(currPos, nextPos, board))  {

        return false;
    }

    Piece* movingPiece = cellInCoord(currPos, board)->piece;
	if (movingPiece == NULL) {printErr(ERR_FUNC_FAILED, __func__); return false;}
	Color playerColor = game->currPlayer;

	removePiece(movingPiece, board);

	// destroy piece in dest pos, if exists
    Piece* destPiece = cellInCoord(nextPos, board)->piece;
	if (destPiece != NULL) {
		removePiece(destPiece, board);
	}

	// put piece in the next pos
	putPiece(movingPiece, board, nextPos);

	bool isValid = true;
	// check if move make it's own king vulnerable, and so is illegal
	if (isKingThreatend(game, playerColor)) {
		isValid = false;
	}

	// undo the move we just did
	removePiece(movingPiece, board);
	putPiece(movingPiece, board, currPos);

    // put piece back to its place if we kill it in the move
	if (destPiece != NULL) {
		putPiece(destPiece, &game->gameBoard, nextPos);
    }


    return isValid;
}


ArrayList* getValidMoves(ChessGame* game, Vector* coord) {

    Piece* piece = cellInCoord(coord, &game->gameBoard)->piece;

    assertAndPrintFuncErr(piece != NULL, __func__);

	if (isCellEmpty(cellInCoord(coord, &game->gameBoard))) return NULL;

	ArrayList* optionalMovesCoords = getAllPathReport(coord, &game->gameBoard, THREAT_ACTION_RETURN_MOVES);

	if (optionalMovesCoords == NULL) return NULL;

	// loop on all moves
	for (int i = 0; i < ArrayListSize(optionalMovesCoords); i++) {
		Vector* nextCoord = (Vector*) ArrayListGetAt(optionalMovesCoords, i);
		// check if move threaten own king, and so invalid
		bool isLegalMove = isMoveValid(game, coord, nextCoord);
		if (!isLegalMove) {
			ArrayListRemoveAt(optionalMovesCoords, i); // move isn't legal, need to remove it
			i--; // because the same index now point to different element
		}
	}
	return optionalMovesCoords;
}


void undo(ChessGame* game) {
	assertAndPrintFuncErr(game != NULL, __func__);

	Color color = game->currPlayer;

	// check if we can undo move
	if (game->settings.gameModeUsersNum == PLAYER_VS_PLAYER) {
		printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_CMD_UNDO_1_NOT_AVAIL_IN_2PLAYER_MODE,
										   __func__, color, NULL, NULL);
		return;
	}
	else if (ArrayListIsEmpty(game->history)) {
		printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_CMD_UNDO_2_NO_HISTORY,
										   __func__, color, NULL, NULL);
		return;
	}

	undoingMove(game, true); // undoing compMove
	undoingMove(game, true); // undoing userMove

	printBoard(&game->gameBoard);
}


void undoingMove(ChessGame* game, bool print) {

	assertAndPrintFuncErr(game != NULL, __func__);

	History* history = (History*) ArrayListGetFirst(game->history); // get last move from history

	Choice* choice = &history->choice;
	Piece* piece = cellInCoord(&choice->nextPos, &game->gameBoard)->piece; // get piece to undo its move
	assertAndPrintFuncErr(piece != NULL, __func__);
	assertAndPrintFuncErr(piece->color != game->currPlayer, __func__);	// we undo the move of previous player, aka not current player
	removePiece(piece, &game->gameBoard);
	assertAndPrintFuncErr(cellInCoord(&choice->prevPos, &game->gameBoard)->piece == NULL, __func__); // assert the previous position is empty
	putPiece(piece, &game->gameBoard, &choice->prevPos);
	if (history->type != NULL) {
		initializePiece(history->type, &game->gameBoard, &choice->nextPos,
				getOppositeColor(piece->color)); // bring back killed piece, if existed
		//TODO check for allocation failed
	}

	if (print)
		printMsgForPlayingStateAndUserTurn(MSG_GAME_USER_CMD_UNDO_3_OK_REPORT_CHANGE,__func__,
										   piece->color, &choice->nextPos, &choice->prevPos);

	flipCurrentPlayer(game);

	ArrayListRemoveFirst(game->history); // delete last move from history

	game->gameState = GAME_STATE_ON; // can undo after game finished
}


bool isGameFinished(ChessGame *game) {
	return (game->gameState != GAME_STATE_ON);
}

Choice getInvalidChoice() {
	Choice choice = {{{-1,-1}}, {{-1,-1}}};
	return choice;
}

bool isChoiceValid(const GameBoard* board, const Choice* choice) {
	return isCoordInBoard(&choice->prevPos, board) && isCoordInBoard(&choice->nextPos, board);
}

bool isKingThreatend(ChessGame* game, Color kingColor) {
	// get pointer to relevant king
	Piece* king;
	if (kingColor == WHITE) king = game->whiteKing;
	else king = game->blackKing;

	// check threat in crossing
    assertAndPrintFuncErr(game->blackKing->color == BLACK, __func__);
	Cell* kingCell = king->currentCell;
	for (int i = 0; i < ArrayListSize(kingCell->CellCrossings); i++) {
		CellCrossing* cross = ArrayListGetAt(kingCell->CellCrossings, i);
		Cell* threatCell = cross->threatPath->originPiece->currentCell; // cell of threatening piece
		if (isPossibleMove(&threatCell->posOnBoard, &kingCell->posOnBoard, &game->gameBoard)) {
			return true; // we found an enemy who could move to king
		}
	}
	return false;
}


void updateGameState(ChessGame *game) {


	assertAndPrintFuncErr(game != NULL, __func__);

	if (!isPlayerCanMove(game, game->currPlayer)) {
		if (isKingThreatend(game, game->currPlayer)) {
			if (game->currPlayer == WHITE) {
				game->gameState = GAME_STATE_BLACK_WON;
			}
			else { // black king is threaten
				game->gameState = GAME_STATE_WHITE_WON;
			}
		}
		else { // King isn't threaten
			game->gameState = GAME_STATE_TIE;
		}

	}
}

void doLoad(ChessGame* game, const char* filePath) {
	loadFile(game, filePath);
}

bool isPlayerCanMove(ChessGame* game, Color color) {
	// loop on all cells to find all pieces
	for (int col = 0; col < game->gameBoard.dims[0]; col++) {
		for (int row = 0; row < game->gameBoard.dims[1]; row++) {
			Cell* cell = &game->gameBoard.board[col][row];
			if (!isCellEmpty(cell)) {
				// check if it's relevant Player's picece
				if (cell->piece->color == color) {
					// find all optional piece's moves
					Vector piecePos = {{col, row}};
					//ArrayList* optionalCells = getAllPathReport(&piecePos, &game->gameBoard, THREAT_ACTION_RETURN_MOVES);
                    ArrayList *optionalCells = getValidMoves(game, &piecePos);
					if (!ArrayListIsEmpty(optionalCells)) { // piece can move
						ArrayListDestroy(optionalCells);
						return true;
					}
					ArrayListDestroy(optionalCells);
				}
			}
		}
	}
    // no moves found
	return false;
}


bool canUndo(ChessGame* game) {
	return (game->settings.gameModeUsersNum == PLAYER_VS_COMP &&
			!ArrayListIsEmpty(game->history));
}
