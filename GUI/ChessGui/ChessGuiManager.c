#include "ChessGuiManager.h"

int guiMainLoop(ChessGame* game){

	if (SDL_Init(SDL_INIT_VIDEO) < 0) { //SDL2 INIT
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 0;
	}

	// *** IMPORTANT NOTE REGRADING WIDGETS AND WINDOWS ***
	// Although the provided example model seperated them,
	// We **knowingly decided** to expand the 'widget' term to include a Window type,
	// to allow for better modularity, functional structure inheritance, and less code repetitions.

	Widget* windowWidget = createMainChessWindow(game);

	if (windowWidget == NULL) {
		SDL_Quit();
		return 0;
	}

	//initGameForGui();

	drawWidget(windowWidget, W_CMD_DRAW_AND_RENDER);

	initializeValidAndRecentSlot(); // for save handling

	// GUI EVENT LOOP
	SDL_Event event;
	while (1) {
		SDL_WaitEvent(&event);
		if(event.type == SDL_QUIT){
			break;
		}

		// SDL Event handler for Main Window GUI
		handleSDLEventForWidget(windowWidget,&event);



	}

	saveValidAndRecentSlot(); // save current values for save handling

	// Clear window
	destroyWidget(windowWidget);
	SDL_Quit();
	return 0;

}




// === GUI Commands - Game Events \ Cmds - (Game-->Gui)

void setupGui(Gui* gui, MainLoop mainloop, GameEventListener gameEventListener){
	gui->mainloop = mainloop;
	gui->gameEventListener = gameEventListener;

}







