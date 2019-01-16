#ifndef GUI_GUIAPI_WIDGETPRIMITIVE_H_
#define GUI_GUIAPI_WIDGETPRIMITIVE_H_


#include "../../MemGateway/MemGateway.h"
#include "../../IO/BasicErrorsHandling.h"
#include <SDL.h>
#include <SDL_video.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../GuiApi/GuiCommonSdlHelpers.h"

typedef enum t_widget_prime_type {
	W_TYPE_WIDGET_IS_PARENT,
	W_TYPE_WIDGET_NOT_PARENT,
	W_TYPE_WIDGET_WINDOW,
} WidgetType;

typedef enum t_widget_hide_state {
	W_STATE_VISIBLE = 0,
	W_STATE_HIDDEN = 1, // Used in isHidden for true value
} WidgetHideState;

typedef enum t_widget_draw_cmd {
	W_CMD_NO_DRAW_NO_RENDER,
	W_CMD_DRAW_ONLY_NO_RENDER,
	W_CMD_DRAW_AND_RENDER,
	W_CMD_RENDER_ONLY_NO_DRAW
} WidgetCmdDraw;

typedef enum t_widget_hide_cmd {
	W_CMD_SET_VISIBLE = 0,
	W_CMD_SET_HIDDEN = 1,
	W_CMD_TOGGLE_SAVE_RESTORE,
	W_CMD_SAVE_AND_HIDE,
	W_CMD_RESTORE,
} WidgetCmdHide;

/*
typedef enum t_widget_handler_report {
	W_HANDLER_CONT_CHAIN,
	W_HANDLER_END_CHAIN,
} ;
*/

typedef struct t_gui_element Widget;

typedef void (*HandleSDLEvent)(Widget* widget, SDL_Event* event);
typedef void (*DrawWidget)(Widget* widget, WidgetCmdDraw drawCmd, SDL_Rect* renderTarget);
typedef void (*HideWidget)(Widget* widget, WidgetCmdHide hideCmd, WidgetCmdDraw drawCmd);

typedef void (*CreateWidgetsInArray)(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets);


struct t_gui_element {

	// == Destroy
	void (*destroyWidget)(Widget*);

	// == Behavior
	WidgetType type;
	Widget* parent;
	HandleSDLEvent handleSDLEvent;

	// == Display
	// .. Rendering
	SDL_Rect* targetRect;
	SDL_Renderer* renderer;
	// .. Draw and Hide
	WidgetHideState activeHideState;
	WidgetHideState savedHideState;
	DrawWidget drawWidget;
	HideWidget hideWidget;
	// 	.. Background Color
	SDL_Color* bgColor;

	// == Extension
	void* ext;


};



// DESTROY
void destroyWidget(Widget* widget);

// DESTROY - HELPERS
void destroyWidgetsInsideArray(Widget** widgets, int numOfWidget);
void destroyWidgetsArrayAndElements(Widget** widgets, int numOfWidgets);

// DRAW

void drawWidget(Widget* widget, WidgetCmdDraw drawCmd);
void drawWidgetAtTarget(Widget* widget, WidgetCmdDraw drawCmd, SDL_Rect* renderTarget);

// DRAW - HELPERS
void drawWidgetsInArray(Widget** widgets, int numOfWidget, WidgetCmdDraw drawCmd);
void drawWidgetsInArrayAtTarget(Widget** widgets, int numOfWidget, WidgetCmdDraw drawCmd,
		SDL_Rect* renderTarget);

bool shouldDrawByCmd(WidgetCmdDraw drawCmd);
bool shouldRenderByCmd(WidgetCmdDraw drawCmd);


// DRAW - HIDE
bool isHidden(Widget* widget);
void hideWidget(Widget* widget, WidgetCmdHide hideCmd, WidgetCmdDraw drawCmd);
void hideWidgetsInArray(Widget** widgets, int numOfWidget,
		WidgetCmdHide hideCmd, WidgetCmdDraw drawCmd);
WidgetCmdHide getHideActionAndTreatSaved(Widget* widget, WidgetCmdHide hideCmd);
void hidingHandlerForWidgetDefault(Widget* widget, WidgetCmdHide hideCmd, WidgetCmdDraw drawCmd);

// DRAW - MORE COMMON
void moveWidgetToPos(Widget* widget, int x, int y, WidgetCmdDraw drawCmd);


// EVENT HANDLERS
void handleSDLEventForWidget(Widget* widget, SDL_Event* event);
void handleSDLEventForWidgetsInArray(Widget** widgets, int numOfWidget, SDL_Event* event);
void handleSDLEventEmptyHandler(Widget* widget, SDL_Event* event);



// HELPERS - Setters
void setParentForWidget(Widget* widget, Widget* parent);
	void setParentForWidgetsInArray(Widget** widgets, int numOfWidgets, Widget* parent);

// HELPERS - QUERIES
bool isNullWidget(Widget* widgets);
bool isNullWidgetsArray(Widget** widgets);
bool ifAnyWidgetNotNullCheck(Widget** widgets, int numOfWidget);







#endif
