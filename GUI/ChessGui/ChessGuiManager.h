/*
 * ChessGameGui.h
 *
 *  Created on: Sep 3, 2017
 *      Author: user01
 */

#ifndef CHESSGAMEGUI_H_
#define CHESSGAMEGUI_H_


/*
 * HANDLES MAIN GUI
 * Supports input output events
 *
 *
 * Examples:
 *
 *  setmaindisplay
 *  displayboard
 *
 *
 */


/*
 * General Notes on SDL:
 * SDL_Rect 			: A structure that contains the definition of a rectangle, with the origin at the upper left.
 * SDL_Surface			: A structure that contains a collection of pixels used in software blitting.
 * SDL_Texture			: A structure that contains an efficient, driver-specific representation of pixel data.
 * SDL_BlitSurface() 	: Use this function to perform a fast surface copy to a destination surface.
 *
 */



#include <stdio.h>

#include "ChessGameGuiController.h"


typedef struct t_gui Gui;
typedef int (*MainLoop)();
typedef int (*GameEventListener)();
struct t_gui {
	MainLoop mainloop;
	GameEventListener gameEventListener;

};





int guiMainLoop(ChessGame* game);

void setupGui(Gui* gui, MainLoop mainloop, GameEventListener gameEventListener);






#endif /* CHESSGAMEGUI_H_ */
