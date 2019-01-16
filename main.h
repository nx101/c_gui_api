/*
 * main.h
 *
 *  Created on: Sep 18, 2017
 *      Author: user01
 */

#ifndef MAIN_H_
#define MAIN_H_
#include "ChessGame/ChessGame.h"
#include "ChessGame/ChessAI.h"
#include "GUI/ChessGui/ChessGuiManager.h"




int main(int argc, char** argv);


/**
 * @param game - our game
 * main loop of the game in console mode
 */
void startGameAtModeConsole(ChessGame* game);




#endif /* MAIN_H_ */
