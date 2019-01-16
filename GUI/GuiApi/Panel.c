
#include "Panel.h"


static SDL_Color colorBgForPanelDefault = {230, 0, 230, SDL_ALPHA_OPAQUE};



// === DESTROY

void destroyPanel(Widget* widget){
	// Args Null check
	if (widget == NULL) {return;}

	PanelExt* ext = (PanelExt*) widget->ext;

	// Destroy Children First
	destroyWidgetsArrayAndElements(ext->panelWidgets, ext->numOfWidgets);

	// Free Panel aspect
	// Free Panel Extension
	// * Don't free renderer
	// * Don't free handler functions
	myFree(widget->targetRect, __func__, __LINE__);

	if (isPanelSupportsTexture(ext->setupDispMode)){
		SDL_DestroyTexture(ext->texture);
		myFree(ext->srcFrameRect, __func__, __LINE__);
		// Marker
	} else assert(ext->texture == NULL && ext->srcFrameRect == NULL);
	myFree(ext, __func__, __LINE__);

	// Free widget aspect
	myFree(widget, __func__, __LINE__);

	// Done!

}



// === CREATE


Widget* createPanel(SDL_Renderer* renderer, SDL_Rect* targetRect,
		PanelSetupDisplayMode setupDispMode, const char* imagePath,
		int numOfWidgets, CreateWidgetsInArray createWidgetsInArray){

	bool supportsTexture = isPanelSupportsTexture(setupDispMode);

	// Args Checks
	if (renderer == NULL || targetRect == NULL ||
			(imagePath == NULL && supportsTexture)||
			((numOfWidgets > 0) && (createWidgetsInArray == NULL))  ){
		printErr(ERR_FUNC_FAILED, __func__);
		return NULL ;
	}

	// Allocate data for Widget aspect
	Widget* widget = (Widget*) myMalloc(sizeof(Widget), __func__, __LINE__);
	// Allocate data for Panel aspect
	PanelExt* ext = (PanelExt*) myMalloc(sizeof(PanelExt), __func__, __LINE__);

	// Load texture from file via surface
	SDL_Texture* texture = NULL;
	if (supportsTexture)
		texture = guiLoadTexture(imagePath, renderer);

	// Create Panel's Widgets
	Widget** widgets = NULL;
	if (createWidgetsInArray != NULL) {
		widgets = createPanelWidgets(renderer, targetRect, numOfWidgets, createWidgetsInArray);
	}

	// Allocation and Creation Null checks
	if (widget == NULL || ext == NULL ||
			(texture == NULL && supportsTexture) ) {
		printErr(ERR_FUNC_FAILED, __func__);
		myFree(widget, __func__, __LINE__);
		myFree(ext, __func__, __LINE__);
		SDL_DestroyTexture(texture); // Null Safe
		return NULL ;
	}


	// Init :

	// = INIT UPPER TYPE: WIDGET
	// == Mem Destroy
	widget->destroyWidget = destroyPanel;
	// == Behavior
	widget->type = W_TYPE_WIDGET_IS_PARENT;
	widget->parent = NULL; // Will be set by parent after its createWidgetsArray
	widget->handleSDLEvent = handlePanelEvent;
	// == Display
	// .. Rendering
	widget->targetRect = guiCreateRectByCopy(targetRect);
	widget->renderer = renderer;
	// .. Draw and Hide
	widget->savedHideState = widget->activeHideState = W_STATE_VISIBLE;
	widget->drawWidget = drawPanel;
	widget->hideWidget = hidePanel;
	// 	.. Background Color
	widget->bgColor = &colorBgForPanelDefault;
	// == Extension
	widget->ext = ext;


	// = init Panel extension
	// == Display
	ext->setupDispMode = setupDispMode;
	// 	.. Image Texture
	ext->srcFrameRect = NULL;
	ext->texture = texture;
	// == Panel's Widget Array
	ext->panelWidgets = widgets;
	ext->numOfWidgets = numOfWidgets;
	// == Behavior
	setParentForWidgetsInArray(widgets, numOfWidgets, widget);

	return widget;

}


Widget** createPanelWidgets(SDL_Renderer* renderer, const SDL_Rect*  targetRect,
		int numOfWidgets, CreateWidgetsInArray createWidgetsInArray) {
	// Args Checks
	if (renderer == NULL) {return NULL;}
	// Allocate Widget* Array
	Widget** widgets = myCalloc(numOfWidgets, sizeof(Widget*), __func__, __LINE__);
	if (widgets == NULL ) {return NULL ;}

	// Create Widgets in array
	createWidgetsInArray(renderer, targetRect, widgets, numOfWidgets);

	return widgets;
}




// === DRAW


void drawPanel(Widget* widget,  WidgetCmdDraw drawCmd, SDL_Rect* renderTarget) {
	// Args Null check
	if (widget == NULL) {return;}
	// Exit Triggers at Widget level - hidden correlates to children too
	if (isHidden(widget)) return;

	SDL_Renderer* renderer = widget->renderer;
	PanelExt* ext = (PanelExt*) widget->ext;


	// == DRAW
	if (shouldDrawByCmd(drawCmd)){
		// .. Draw Background
		if(shouldDrawPanelBgColor(ext->setupDispMode)) {
			SDL_SetRenderDrawColor(renderer,
					widget->bgColor->r, widget->bgColor->g, widget->bgColor->b, widget->bgColor->a);
			SDL_RenderFillRect(renderer, widget->targetRect);
		}
		// .. Draw Sprite
		if (shouldDrawPanelSprite(ext->setupDispMode)) {
			SDL_RenderCopy(renderer, ext->texture, ext->srcFrameRect, widget->targetRect);
		}

		// == DRAW CHILDREN WIGETS (on top)
		if (widget->type == W_TYPE_WIDGET_IS_PARENT) {
			drawWidgetsInArray(ext->panelWidgets, ext->numOfWidgets, W_CMD_DRAW_ONLY_NO_RENDER);
		}
	}

	// == RENDER
	if (shouldRenderByCmd(drawCmd)) {
		if (!shouldRenderByPanelSetup(ext->setupDispMode)) { // Don't render panel - do render kids' areas
			drawWidgetsInArray(ext->panelWidgets, ext->numOfWidgets, W_CMD_RENDER_ONLY_NO_DRAW);
		}
		else { // Render at entire panel area
			// Select Render Target Area
			if (renderTarget != NULL)
				SDL_RenderSetViewport(renderer, renderTarget);
			else SDL_RenderSetViewport(renderer, widget->targetRect); // Set renderer viewport to target area
			// Render
			SDL_RenderPresent(renderer); // Commit Render
			SDL_RenderSetViewport(renderer, NULL); // Set renderer viewport to all
		}
	}
}



// === DRAW - HELPERS

bool shouldDrawPanelBgColor(PanelSetupDisplayMode dispMode){
	return (dispMode != PANEL_DISP_SPRITE_ONLY && dispMode != PANEL_DISP_EMPTY);
}
bool shouldDrawPanelSprite(PanelSetupDisplayMode dispMode){
	return (dispMode != PANEL_DISP_BG_ONLY && dispMode != PANEL_DISP_EMPTY);
}
bool isPanelSupportsTexture(PanelSetupDisplayMode dispSetup){
	return (dispSetup != PANEL_DISP_BG_ONLY && dispSetup != PANEL_DISP_EMPTY);
}

bool shouldRenderByPanelSetup(PanelSetupDisplayMode setupDispMode){
	return !(setupDispMode == PANEL_DISP_EMPTY);
}




// === DRAW - Hide



void hidePanel(Widget* widget, WidgetCmdHide hideCmd, WidgetCmdDraw drawCmd){

	if (drawCmd) {};

	PanelExt* ext = (PanelExt*) widget->ext;

	WidgetCmdHide hideAction = getHideActionAndTreatSaved(widget, hideCmd);

	// == HIDE
	if (hideAction == W_CMD_SET_HIDDEN) {
		// Hide children, don't draw or render them
		hideWidgetsInArray(ext->panelWidgets, ext->numOfWidgets,
				hideCmd, W_CMD_NO_DRAW_NO_RENDER);
		// Hide self
		widget->activeHideState = W_STATE_HIDDEN;
		// Draw And Render Parent *efficiently* - only at needed area
		if (shouldDrawByCmd(drawCmd) || shouldRenderByCmd(drawCmd))
			drawWidgetAtTarget(widget->parent,
					W_CMD_DRAW_AND_RENDER, widget->targetRect);
	}
	// == UNHIDE
	else {
		// method: drawing all then rendering once at parent's region
		// Un-hide, draw - self
		widget->activeHideState = W_STATE_VISIBLE;
		drawWidget(widget, W_CMD_DRAW_ONLY_NO_RENDER);
		// Un-hide, draw - Children
		hideWidgetsInArray(ext->panelWidgets, ext->numOfWidgets,
				hideCmd, W_CMD_DRAW_ONLY_NO_RENDER);
		// Render, self region
		drawWidget(widget, W_CMD_RENDER_ONLY_NO_DRAW);
	}

}



// === EVENT HANDLER

void handlePanelEvent(Widget* widget, SDL_Event* event){
	if(widget == NULL || event == NULL){return;}

	PanelExt* ext = (PanelExt*) widget->ext;
	handleSDLEventForWidgetsInArray(ext->panelWidgets, ext->numOfWidgets, event);

	return;

}

