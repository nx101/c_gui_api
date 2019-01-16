#include "Window.h"


static SDL_Color colorBgForWindowDefault = {230, 230, 230, SDL_ALPHA_OPAQUE};




// === DESTROY
void destroyWindow(Widget* widget) {
	if (widget == NULL) {return;}

	Window* winExt = (Window*) widget->ext;
	// === Last SDL components
	SDL_Renderer* sdlRenderer = widget->renderer;
	SDL_Window* sdlWindow = winExt->sdlWindow;

	// == Window Aspect
	// Destroy Children First
	// .. Window's Widget Array
	destroyWidgetsArrayAndElements(winExt->windowWidgets, winExt->numOfWidgets);

	// Destory Window Ext
	myFree(widget->ext, __func__, __LINE__);

	// == Widget Aspect
	// .. Rendering
	myFree(widget->targetRect, __func__, __LINE__);
	myFree(widget, __func__, __LINE__);

	// === Main SDL Components
	// (!) MUST Destroy renderer before window
	SDL_DestroyRenderer(sdlRenderer);
	// .. SDL Window - must destroy at end
	SDL_DestroyWindow(sdlWindow);
}


// === CREATE


// * Main create window function
Widget* createWindow(int width, int height, const char* windowTitle,
		int numOfWidgets, CreateWidgetsInArray createWidgetsInArray) {

	// Args Checks
	if (width < 0 || height < 0 ||
			((numOfWidgets > 0) && (createWidgetsInArray == NULL))){return NULL;}

	// Allocate data for widget aspect
	Widget* widget = (Widget*) myMalloc(sizeof(Widget), __func__, __LINE__);
	// Allocate data for window aspect
	Window* ext = myMalloc(sizeof(Window), __func__, __LINE__);
	SDL_Window* window = SDL_CreateWindow(windowTitle,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Rect* targetRect = guiCreateRectByInts(0, 0, width, height);

	// Create Windows's Widgets
	Widget** widgets = NULL;
	if (createWidgetsInArray != NULL && targetRect != NULL) {
		widgets = createWidgetsInWindow(renderer, targetRect,
				numOfWidgets, createWidgetsInArray);
	}

	// Allocation and Creation Null checks
	if (widget == NULL || ext == NULL ||
			window == NULL || renderer == NULL || targetRect == NULL ) {
		myFree(widget, __func__, __LINE__);
		myFree(ext, __func__, __LINE__);
		myFree(widgets, __func__, __LINE__);
		myFree(targetRect, __func__, __LINE__);
		//We first destroy the renderer
		SDL_DestroyRenderer(renderer); //NULL safe
		SDL_DestroyWindow(window); //NULL safe
		return NULL ;
	}


	// = INIT UPPER TYPE: WIDGET
	// .. Mem Destroy
	widget->destroyWidget = destroyWindow;
	// .. Behavior
	widget->type = W_TYPE_WIDGET_WINDOW;
	widget->parent = NULL; // No parent for window
	widget->handleSDLEvent = handleSdlEventWindow;
	// .. Display
	// .. Rendering
	widget->targetRect = targetRect;
	widget->renderer = renderer;
	// .. Draw and Hide
	widget->savedHideState = widget->activeHideState = W_STATE_VISIBLE;
	widget->drawWidget = drawWindow;
	widget->hideWidget = hideWindow;
	// 	.. Background Color
	widget->bgColor = &colorBgForWindowDefault;
	// .. Extension
	widget->ext = ext; // Link Extension structure before init extension

	// = INIT WINDOW EXT:
	// .. SDL Window
	ext->sdlWindow = window;
	// .. Window's Widget Array
	ext->numOfWidgets = numOfWidgets;
	ext->windowWidgets = widgets;
	setParentForWidgetsInArray(widgets, numOfWidgets, widget);


	// SET ANY GLOBAL SDL FLAGS
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);



	// DONE!
	return widget;
}


// * Delegated creation of Window's Widgets
Widget** createWidgetsInWindow(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		int numOfWidgets, CreateWidgetsInArray createWidgetsInArray) {
	// Args Checks
	if (renderer == NULL) {return NULL;}
	// Allocate Widget* Array
	Widget** widgets = myCalloc(numOfWidgets, sizeof(Widget*), __func__, __LINE__);
	if (widgets == NULL) {return NULL;}
	// Create Widgets in array
	createWidgetsInArray(renderer, targetRect, widgets, numOfWidgets);
	return widgets;
}


// === DRAW
void drawWindow(Widget* widget, WidgetCmdDraw drawCmd, SDL_Rect* renderTarget) {
	if (renderTarget ) {};
	if (widget == NULL ) {return;}
	if (isHidden(widget)){} // Hiding not supported for window;
	Window* ext = (Window*) widget->ext;

	// == DRAW
	if (shouldDrawByCmd(drawCmd)){

		//Draw window
		SDL_SetRenderDrawColor(widget->renderer,
				widget->bgColor->r, widget->bgColor->g,
				widget->bgColor->b, widget->bgColor->a);
		SDL_RenderClear(widget->renderer);

		//Draw widgets
		drawWidgetsInArray(ext->windowWidgets, ext->numOfWidgets, W_CMD_DRAW_AND_RENDER);
	}

	// == RENDER
	if (shouldRenderByCmd(drawCmd)) {
		SDL_RenderPresent(widget->renderer);
	}
}


// DRAW - HIDE
void hideWindow(Widget* widget, WidgetCmdHide hideCmd, WidgetCmdDraw drawCmd){
	if (widget || hideCmd || drawCmd) {};
}


// === EVENT HANDLER
void handleSdlEventWindow(Widget* widget, SDL_Event* event){
	if (widget == NULL || widget->ext == NULL) return;
	Window* ext = (Window*)widget->ext;
	handleSDLEventForWidgetsInArray(ext->windowWidgets, ext->numOfWidgets, event);
	return;
}


