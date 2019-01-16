#include "ChessAI.h"

void doCompTurn(ChessGame* game, bool print) {
    Color compColor = getOppositeColor(game->settings.user1Color);
    assertAndPrintFuncErr(game != NULL && game->currPlayer == compColor, __func__);

    Choice choice = MinimaxSuggestMove(game, (unsigned) game->settings.difLevel);

    assertAndPrintFuncErr(isChoiceValid(&game->gameBoard, &choice), __func__);

    moveAdvanced(game, &choice.prevPos, &choice.nextPos, false);

    Piece* piece = cellInCoord(&choice.nextPos, &game->gameBoard)->piece;

    assertAndPrintFuncErr(piece != NULL && piece->color != game->settings.user1Color, __func__);


    if (print) {
        printMsgForPlayingStateAndAITurn(MSG_GAME_AI_ON_BOARD_UPDATED_AI_REPORT_REGULAR_NO_PAWN_PROMOTE,
                                         piece, &choice.prevPos, &choice.nextPos);

        switch (game->gameState) {
            case GAME_STATE_BLACK_WON:
                printMsgForPlayingStateAndAITurn(MSG_GAME_AI_ON_BOARD_UPDATED_KING_IN_CHECK, piece, NULL, NULL);
                break;

            case GAME_STATE_WHITE_WON:
                printMsgForPlayingStateAndAITurn(MSG_GAME_AI_ON_BOARD_UPDATED_KING_IN_CHECK, piece, NULL, NULL);
                break;

            case GAME_STATE_TIE:
                printMsgForPlayingStateAndAITurn(MSG_GAME_AI_ON_BOARD_UPDATED_GAME_ENDS_TIE, piece, NULL, NULL);
                break;

            case GAME_STATE_ON:
                if (isKingThreatend(game, game->settings.user1Color)) {
                    printMsgForPlayingStateAndAITurn(MSG_GAME_AI_ON_BOARD_UPDATED_KING_IN_CHECK, NULL, NULL, NULL);
                }
                break;
        }

    }
}


Choice MinimaxSuggestMove(ChessGame* game, unsigned maxDepth){

    // return -1 as fault code
    if (game==NULL || maxDepth<=0 || MAXDIFFC<maxDepth) return getInvalidChoice();

    ArrayList* realHistory = game->history;
    ArrayList* historyForMinMax = ArrayListCreate(maxDepth, sizeof(History));
    game->history = historyForMinMax;

    MinmaxRes nodeScore = recNodeScore(true, game, maxDepth, INT_MIN, INT_MAX);

    game->history = realHistory;
    ArrayListDestroy(historyForMinMax);

    return nodeScore.choice;
}


MinmaxRes recNodeScore(bool isMaxNode, ChessGame* game, unsigned depth, int alpha, int beta) {
    // Init nodeScore
    // colChoice=NULL for a winned / tied game - no choices are possible.
    MinmaxRes nodeScore;
    nodeScore.choice = getInvalidChoice();
    nodeScore.score = (isMaxNode) ? INT_MIN : INT_MAX;
    // Stop conditions

    // Game is FINISHED || DEPTH REACHED - Final minmax tree depth arrived:
    if (isGameFinished(game) || depth == 0) {
        nodeScore.score = getBoardScore(game);
        return nodeScore;
    }

    // Game continues - must be at least one valid move - check kids recursively

    // loop on each cell to find all pieces
    for (int i = 0; i < game->gameBoard.dims[0] ; i++) {
        for (int j = 0; j < game->gameBoard.dims[1]; j++) {
            Piece* piece = game->gameBoard.board[i][j].piece;
            if (piece != NULL && piece->color == game->currPlayer) {
                ArrayList* optionalsMoveCoords =
                        getAllPathReport(&piece->currentCell->posOnBoard, &game->gameBoard, THREAT_ACTION_RETURN_MOVES);

                //loop on each cell in this peiece's path
                for (int k = 0; k < ArrayListSize(optionalsMoveCoords); k++) {
                    Vector* nextPos = (Vector*) ArrayListGetAt(optionalsMoveCoords, k);
                    bool isValidMove = moveAdvanced(game, &piece->currentCell->posOnBoard, nextPos, false);
                    if (!isValidMove) continue; //move got player's king vulnerable thus invalid, so we skip it

                    MinmaxRes childIRes = recNodeScore(!isMaxNode, game, depth-1, alpha, beta);

                    piece = cellInCoord(nextPos, &game->gameBoard)->piece; // update because piece can be destroyed
                    // (and then undo, but the pointer is no longer valid)
                    // we need to use cellInCcord instead just nextCell because nextCell allocated inside optionalCells
                    // and not inside gameBoard.board;



                    // update alpha in case of MaxNode
                    if (isMaxNode && nodeScore.score <= childIRes.score) {
                        if (nodeScore.score < childIRes.score || !isChoiceValid(&game->gameBoard, &nodeScore.choice)) {
                            Choice choice = {(Vector) {{i,j}}, *nextPos};
                            nodeScore.choice = choice;
                        }
                        nodeScore.score = childIRes.score;
                        alpha = childIRes.score;
                    }
                    // update beta in case of MinNode
                    else if (!isMaxNode && nodeScore.score >= childIRes.score) {
                        if (nodeScore.score > childIRes.score || !isChoiceValid(&game->gameBoard, &nodeScore.choice)) {
                            Choice choice = {(Vector) {{i,j}}, *nextPos};
                            nodeScore.choice = choice;
                        }
                        nodeScore.score = childIRes.score;
                        beta = childIRes.score;
                    }

                    undoingMove(game, false);

                    // pruning
                    if (alpha >= beta) {
                        ArrayListDestroy(optionalsMoveCoords);
                        return nodeScore;
                    }
                }
                ArrayListDestroy(optionalsMoveCoords);
            }
        }
    }
    return nodeScore;
}


int getBoardScore(ChessGame* game) {
    int score = 0;
    Color compColor = getOppositeColor(game->settings.user1Color);

    for (int i = 0; i < game->gameBoard.dims[0]; i++) {
        for (int j = 0; j < game->gameBoard.dims[1]; j++) {
            Piece* piece = game->gameBoard.board[i][j].piece;
            if (piece != NULL) {
                char symbol = piece->type->nameId;
                if (piece->color == compColor)
                    score += getSymbolScore(symbol);
                else
                    score -= getSymbolScore(symbol);
            }
        }
    }

    return score;
}


int getSymbolScore(char symbol) {
    switch (symbol) {
        case SYMBOL_PAWN:
            return 1;
        case SYMBOL_KNIGHT:
            return 3;
        case SYMBOL_BISHOP:
            return 3;
        case SYMBOL_ROOK:
            return 5;
        case SYMBOL_QUEEN:
            return 9;
        case SYMBOL_KING:
            return 100;

        default:
            printErr(ERR_FUNC_FAILED, __func__);
            return 0;
    }
}
