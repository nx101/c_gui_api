#include "WidgetPrimitive.h"

#include <stdlib.h>


// DESTROY

void destroyWidget(Widget* widget) {
	if (isNullWidget(widget) || widget->destroyWidget == NULL)
		return;
	widget->destroyWidget(widget);
}


// DESTROY - HELPERS
void destroyWidgetsInsideArray(Widget** widgets, int numOfWidgets){
	if (isNullWidgetsArray(widgets)) return;
	for (int i=0; i<numOfWidgets; i++){
		destroyWidget(widgets[i]);
	}
}

void destroyWidgetsArrayAndElements(Widget** widgets, int numOfWidgets){
	if (isNullWidgetsArray(widgets)) return;
	// fprintf(stderr, "Destroying %d elems in Widget** : %p -> %p \n",numOfWidgets, widgets, *widgets); fflush(stderr);
	destroyWidgetsInsideArray(widgets, numOfWidgets);
	// fprintf(stderr, "Finished %d.. Now destroying Widget** : %p -> %p \n",numOfWidgets, widgets, *widgets); fflush(stderr);
	myFree(widgets, __func__, __LINE__);
}




// DRAW

void drawWidget(Widget* widget, WidgetCmdDraw drawCmd) {
	drawWidgetAtTarget(widget, drawCmd, NULL);
}


void drawWidgetAtTarget(Widget* widget, WidgetCmdDraw drawCmd, SDL_Rect* renderTarget){
	if (isNullWidget(widget) || widget->drawWidget == NULL) return;
	widget->drawWidget(widget, drawCmd, renderTarget);
}




// DRAW - HELPERS

void drawWidgetsInArray(Widget** widgets, int numOfWidgets, WidgetCmdDraw drawCmd){
	if (isNullWidgetsArray(widgets)) return;

	for (int i=0; i < numOfWidgets; i++) {
		drawWidgetAtTarget(widgets[i], drawCmd, NULL); // Null safe
	}
}



void drawWidgetsInArrayAtTarget(Widget** widgets, int numOfWidgets,
		WidgetCmdDraw drawCmd, SDL_Rect* renderTarget){
	if (isNullWidgetsArray(widgets)) return;

	for (int i=0; i < numOfWidgets; i++) {
		drawWidgetAtTarget(widgets[i], drawCmd, renderTarget); // Null safe
	}
}



bool shouldDrawByCmd(WidgetCmdDraw drawCmd){
	return (drawCmd == W_CMD_DRAW_AND_RENDER || drawCmd == W_CMD_DRAW_ONLY_NO_RENDER);
}

bool shouldRenderByCmd(WidgetCmdDraw drawCmd){
	return (drawCmd == W_CMD_DRAW_AND_RENDER || drawCmd == W_CMD_RENDER_ONLY_NO_DRAW);
}




// DRAW - HELPERS - HIDE


void hideWidget(Widget* widget, WidgetCmdHide hideCmd, WidgetCmdDraw drawCmd){
	if (isNullWidget(widget)) return;
	widget->hideWidget(widget, hideCmd, drawCmd);
}


void hideWidgetsInArray(Widget** widgets, int numOfWidgets,
		WidgetCmdHide hideCmd, WidgetCmdDraw drawCmd){
	if (isNullWidgetsArray(widgets)) return;

	for (int i=0; i < numOfWidgets; i++) {
		if (widgets[i] != NULL) {
			(widgets[i])->hideWidget(widgets[i], hideCmd, drawCmd);
		}
	}
}


WidgetCmdHide getHideActionAndTreatSaved(Widget* widget, WidgetCmdHide hideCmd){
	// == Deal with saved state and infer action

	if (hideCmd == W_CMD_TOGGLE_SAVE_RESTORE) {
		if (widget->activeHideState == W_STATE_VISIBLE)
			hideCmd = W_CMD_SAVE_AND_HIDE;
		else hideCmd = W_CMD_RESTORE;
	}

	switch (hideCmd) {
	case W_CMD_SAVE_AND_HIDE :
		widget->savedHideState = widget->activeHideState;
		hideCmd = W_CMD_SET_HIDDEN;
		break;
	case W_CMD_RESTORE :
		hideCmd = (WidgetCmdHide) widget->savedHideState;
		break;
	case W_CMD_SET_HIDDEN :
		widget->savedHideState = W_STATE_HIDDEN;
		break;
	case W_CMD_SET_VISIBLE :
		widget->savedHideState = W_STATE_VISIBLE;
		break;
	default:
		break;
	}

	return hideCmd;
}


void hidingHandlerForWidgetDefault(Widget* widget, WidgetCmdHide hideCmd, WidgetCmdDraw drawCmd){
	WidgetCmdHide hideAction = getHideActionAndTreatSaved(widget, hideCmd);

	// == HIDE
	if (hideAction == W_CMD_SET_HIDDEN) {
		widget->activeHideState = W_STATE_HIDDEN;

		// Draw And Render Parent *efficiently* - only at needed area
		if (shouldDrawByCmd(drawCmd) || shouldRenderByCmd(drawCmd)) {
			drawWidgetAtTarget(widget->parent, drawCmd, widget->targetRect);
		}
	}
	// == UNHIDE
	else {
		// Un-hide, draw - self
		widget->activeHideState = W_STATE_VISIBLE;
		drawWidget(widget, drawCmd);
	}
}



bool isHidden(Widget* widget){
	if (widget == NULL) return 0;
	return (bool) widget->activeHideState;
}



// DRAW - MORE COMMON

void moveWidgetToPos(Widget* widget, int x, int y, WidgetCmdDraw drawCmd){
	if (widget == NULL) return;

	// Could also enforce move only within parent rect
	hideWidget(widget, W_CMD_SET_HIDDEN, drawCmd);
	guiUpdateRectXY(widget->targetRect, x, y);
	hideWidget(widget, W_CMD_SET_VISIBLE, drawCmd);
}


// HELPERS - EVENT HANDLER

void handleSDLEventForWidget(Widget* widget, SDL_Event* event){
	if (!isNullWidget(widget)) { // Widget NullSafe
		if (widget->handleSDLEvent != NULL){ // handleEvent NullSafe
			widget->handleSDLEvent(widget, event);
		}
	}

}


void handleSDLEventForWidgetsInArray(Widget** widgets, int numOfWidgets, SDL_Event* event){
	if (isNullWidgetsArray(widgets)) return;

	for (int i=0; i<numOfWidgets; i++){
		handleSDLEventForWidget(widgets[i], event); // Null safe
	}
}



void handleEventEmptyHandler(Widget* widget, SDL_Event* event){
	// Empty Widget Event handler
	if (widget || event) {};
}



// HELPERS - Setters

void setParentForWidget(Widget* widget, Widget* parent){
	if (widget != NULL) {
		widget->parent = parent;
	}
}


void setParentForWidgetsInArray(Widget** widgets, int numOfWidgets, Widget* parent){
	if (isNullWidgetsArray(widgets)) return;
	for (int i=0; i<numOfWidgets; i++){
		setParentForWidget(widgets[i], parent);
	}
}



// HELPERS - QUERIES

bool isNullWidget(Widget* widget) {
	if (widget == NULL) { // Widgets Array NullSafe
		// SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "WARNING", "NULL Widget DETECTED", NULL);
		return 1;
	}
	return 0;
}


bool isNullWidgetsArray(Widget** widgets) {
	if (widgets == NULL) { // Widgets Array NullSafe
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "WARNING!!", "NULL WidgetS ARRAY DETECTED", NULL);
		return 1;
	}
	return 0;
}



bool ifAnyWidgetNotNullCheck(Widget** widgets, int numOfWidgets){
	if (isNullWidgetsArray(widgets)) return 0;

	for (int i=0; i<numOfWidgets; i++){
		if (!isNullWidget(widgets[i])) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "WARNING!!", "not NULL Widget init", NULL);

			return 1;
		}
	}
	return 0;
}


