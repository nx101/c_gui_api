#ifndef WINDOW_H_
#define WINDOW_H_
#include <stdlib.h>

#include "WidgetPrimitive.h"


// Window Extension: BasicWindow
typedef struct t_window_supports_widgets Window;
struct t_window_supports_widgets {

	// SDL Window
	SDL_Window* sdlWindow;

	// Window's Widget Array
	Widget** windowWidgets;
	int numOfWidgets;

};



// === DESTROY
void destroyWindow(Widget* widget);


// === CREATE

Widget* createWindow(int width, int height, const char* windowTitle,
		int numOfWidgets, CreateWidgetsInArray createWidgetsInArray);

Widget** createWidgetsInWindow(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		int numOfWidgets, CreateWidgetsInArray createWidgetsInArray);


// === DRAW
void drawWindow(Widget* widget, WidgetCmdDraw drawCmd, SDL_Rect* renderTarget);


// DRAW - HIDE
void hideWindow(Widget* widget, WidgetCmdHide hideCmd, WidgetCmdDraw drawCmd);


// === EVENT HANDLER
void handleSdlEventWindow(Widget* widget, SDL_Event* event);


#endif
