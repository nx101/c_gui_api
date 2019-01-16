#ifndef CHESSPROG_CHESSCOMP_H
#define CHESSPROG_CHESSCOMP_H

#include <limits.h>
#include "ChessGame.h"
#define MAXDIFFC 4



typedef struct minmaxres_t {
    int score;
    Choice choice;
} MinmaxRes;

/**
 * computer make move according to MinimaxSuggestMove
 * @param game - our game
 * @param print - to print msg about comp move or not
 */
void doCompTurn(ChessGame* game, bool print);


// This is a comment

/**
 * Given a game state, this function evaluates the best move according to
 * the current player. The function initiates a MiniMax algorithm up to a
 * specified length given by maxDepth. The current game state doesn't change
 * by this function including the history of previous moves.
 *
 * @param currentGame - The current game state
 * @param maxDepth - The maximum depth of the miniMax algorithm
 * @return
 * -1 if either currentGame is NULL or maxDepth <= 0.
 * On success the function returns choice which represent the best move for the current player.
 */
Choice MinimaxSuggestMove(ChessGame* game, unsigned maxDepth);


/**
 * Recursive MinMax Strategy decision tree function, using pruning.
 * Unfolds then folds decision branches, selecting best move choice.
 * the tree depth - is set according to depth.
 * @param isMaxPlayer - does the current level belong to the maximising player
 * @param depth - (remaining) tree depth
 * @param curGame - current game
 * @param alpha - alpha from pruning
 * @param beta - beta from pruning
 * @return
 * MinmaxRes:
 *      score - the optimal score of the sub-tree
 *      colChoice - the col choice for optimal score
 */
MinmaxRes recNodeScore(bool isMaxNode, ChessGame* game, unsigned depth, int alpha, int beta);

/**
 * calculate score of board, if player was white.
 */
int getBoardScore(ChessGame* game);

/**
 *  return score of pieceType, which represented by its symbol.
 */
int getSymbolScore(char symbol);




#endif //CHESSPROG_CHESSCOMP_H
