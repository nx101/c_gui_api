/*
 * Panel.h
 *
 *  Created on: Sep 3, 2017
 *      Author: user01
 */

#ifndef PANEL_H_
#define PANEL_H_

#include "WidgetPrimitive.h"


// Display modes
typedef enum t_panel_display_mode {
	PANEL_DISP_EMPTY,
	PANEL_DISP_BG_ONLY,
	PANEL_DISP_SPRITE_ONLY,
	PANEL_DISP_BG_AND_SPRITE,
} PanelSetupDisplayMode;



typedef struct t_widget_extension_panel PanelExt;
struct t_widget_extension_panel{

	// == Display
	PanelSetupDisplayMode setupDispMode;
	// 	.. Image Texture
	SDL_Rect* srcFrameRect;
	SDL_Texture* texture;

	// == Panel's Widget Array
	Widget** panelWidgets;
	int numOfWidgets;

	// == Extension
	void* ext;


};



// === DESTROY
void destroyPanel(Widget*);


// === CREATE

Widget* createPanel(SDL_Renderer* renderer, SDL_Rect* targetRect,
		PanelSetupDisplayMode setupDispMode, const char* imagePath,
		int numOfWidgets, CreateWidgetsInArray createWidgetsInArray);


Widget** createPanelWidgets(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		int numOfWidgets, CreateWidgetsInArray createWidgetsInArray);


// === DRAW

void drawPanel(Widget* widget, WidgetCmdDraw drawCmd, SDL_Rect* renderTarget);
void drawPanelAndRenderPresent(Widget* widget);
void hidePanel(Widget* widget, WidgetCmdHide hideCmd, WidgetCmdDraw drawCmd);


// === DRAW - HELPERS
bool shouldDrawPanelBgColor(PanelSetupDisplayMode dispMode);
bool shouldDrawPanelSprite(PanelSetupDisplayMode dispMode);
bool isPanelSupportsTexture(PanelSetupDisplayMode dispSetup);
bool shouldRenderByPanelSetup(PanelSetupDisplayMode setupDispMode);

// === EVENT HANDLER
void handlePanelEvent(Widget*, SDL_Event*);



// === HELPERS





#endif /* PANEL_H_ */
