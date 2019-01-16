#include "Button.h"

#include <stdlib.h>

#include "GuiCommonSdlHelpers.h"




// === DESTROY

void destroyButton(Widget* widget) {

	// Args Null check
	if (widget == NULL) {return;}

	BtnExt* btnExt = (BtnExt*) widget->ext;

	// fprintf(stderr, "Destroying Button : %p \n", widget); fflush(stderr);

	// FREE BUTTON ASPECT
	// * Don't free renderer
	// * Don't free functions \ locals
	// == Display
	// .. Rendering
	myFree(btnExt->srcFrameRect, __func__, __LINE__);
	if (isBtnSupportsTexture(btnExt->dispSetup))
		SDL_DestroyTexture(btnExt->texture);
	myFree(btnExt->ext, __func__, __LINE__); // Any Extensions of Button

	// FREE WIDGET PRIMITIVE ASPECT
	myFree(btnExt, __func__, __LINE__);
	myFree(widget->targetRect, __func__, __LINE__);
	myFree(widget, __func__, __LINE__);
}





// === CREATE - Simple Templates


Widget* createButtonSimple1 (
		SDL_Renderer* renderer, SDL_Rect* targetRect,
		BtnEventHandler btnEventHandler){

	return createButton(renderer, targetRect, BTN_DISP_BG_AND_SPRITE,
			BTN_SPRITES_BMP_DEF, NULL, NULL, BTN_SETUP_ONE_STATE, btnEventHandler);
}





// === CREATE - Buttons Factory

Widget* createButton(
		SDL_Renderer* renderer, SDL_Rect* targetRect,
		BtnSetupDisplay dispSetup, const char* imagePath,
		BtnUpdateBgColorOnStateChange updateBgColorForState,
		BtnUpdateSpriteSrcFrameOnStateChange updateSpriteSrcFrameForState,
		BtnSetupSelectType type, BtnEventHandler btnEventHandler) {

	bool supportsTexture = isBtnSupportsTexture(dispSetup);

	// Args Checks
	if (renderer == NULL || targetRect == NULL ||
			(imagePath == NULL && isBtnSupportsTexture(dispSetup)) ||
			btnEventHandler == NULL) {
		printErr(ERR_FUNC_FAILED, __func__);
		return NULL ;
	}

	// Allocate data for widget aspect
	Widget* widget = (Widget*) myMalloc(sizeof(Widget), __func__, __LINE__);
	// Allocate data for button aspect
	BtnExt* ext = (BtnExt*) myMalloc(sizeof(BtnExt), __func__, __LINE__);

	// Load texture from file via surface
	SDL_Texture* texture = NULL;
	if (supportsTexture)
		texture = guiLoadTexture(imagePath, renderer);

	// Allocation and Creation Null checks
	if (widget == NULL || ext == NULL ||
			(texture == NULL && supportsTexture)) {
		printErr(ERR_FUNC_FAILED, __func__);
		myFree(ext, __func__, __LINE__);
		myFree(widget, __func__, __LINE__);
		return NULL ;
	}

	// = INIT UPPER TYPE: WIDGET
	// == Mem Destroy
	widget->destroyWidget = destroyButton;
	// == Behavior
	widget->type = W_TYPE_WIDGET_NOT_PARENT;
	widget->parent = NULL; // Will be set by parent after its createWidgetsArray
	widget->handleSDLEvent = onSdlEventHandlerBtnGuiWrapper;
	// == Display
	// .. Rendering
	widget->targetRect = guiCreateRectByCopy(targetRect);
	widget->renderer = renderer;
	// .. Draw and Hide
	widget->savedHideState = widget->activeHideState = W_STATE_VISIBLE;
	widget->drawWidget = drawButton;
	widget->hideWidget = hidingHandlerForWidgetDefault;
	// == Extension
	widget->ext = ext; // Link Extension structure before init extension


	// = INIT BUTTON EXT:

	// == Extension
	ext->ext = NULL;

	// == Behavior
	ext->selectType = type;
	ext->btnState = BTN_NORMAL_UNSELECTED;
	ext->userData = BTN_USER_DATA_DEF;
	ext->btnEventHandler = btnEventHandler;

	// == Display
	ext->dispSetup = dispSetup;
	ext->activeDispMode = ext->savedDispMode = getDispModeForDispSetup(dispSetup);
	// 	.. Background Color
	if (updateBgColorForState != NULL)
		ext->updateBgColorForState = updateBgColorForState;
	else ext->updateBgColorForState = updateBtnBgColorOnStateChangeDefault;
	ext->updateBgColorForState(widget); // Set gColor according to current state
	// 	.. Image Texture
	ext->texture = texture;
	if (updateSpriteSrcFrameForState != NULL)
		ext->updateSpriteSrcFrameForState = updateSpriteSrcFrameForState;
	else ext->updateSpriteSrcFrameForState = updateBtnSrcFrameSpriteUponStateChangeDefault;
	if (supportsTexture)
		ext->srcFrameRect = guiCreateRectByInts(0,0,BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	else ext->srcFrameRect = NULL;
	if (supportsTexture) ext->updateSpriteSrcFrameForState(ext); // Set SrcFrame according to current state


	// DONE!

	return widget;
}


// === DRAW


void drawButton(Widget* widget,  WidgetCmdDraw drawCmd, SDL_Rect* renderTarget) {
	// Args Null check
	if (widget == NULL) {return;}
	// Exit Triggers at Widget level
	if (isHidden(widget)) return;

	SDL_Renderer* renderer = widget->renderer;
	BtnExt* ext = (BtnExt*) widget->ext;

	// If supporting transparency, or removing background,
	// Make sure to always Draw widget's Parent at widget's target region
	// drawWidgetAtTarget(widget->parent, W_CMD_DRAW_ONLY_NO_RENDER, widget->targetRect);

	if (shouldDrawByCmd(drawCmd)){
		// == DRAW
		// Draw Background
		if(shouldDrawBtnBgColor(ext->activeDispMode)) {
			SDL_SetRenderDrawColor(renderer, widget->bgColor->r, widget->bgColor->g,
					widget->bgColor->b, widget->bgColor->a);
			SDL_RenderFillRect(renderer, widget->targetRect);
		}
		// Draw Sprite
		if (shouldDrawBtnSprite(ext->activeDispMode)) {
			SDL_RenderCopy(renderer, ext->texture, ext->srcFrameRect, widget->targetRect);
		}
	}

	// == RENDER
	if (shouldRenderByCmd(drawCmd)) {
		// Select Render Target Area
		if (renderTarget != NULL)
			SDL_RenderSetViewport(renderer, renderTarget);
		else SDL_RenderSetViewport(renderer, widget->targetRect); // Set renderer viewport to target area
		// Render
		SDL_RenderPresent(renderer); // Commit Render
		SDL_RenderSetViewport(renderer, NULL); // Set renderer viewport to all
	}
}



// DRAW - HELPERS - HIDE







// === DRAW - HELPERS

// Sprite behavior - default template
void updateBtnSrcFrameSpriteUponStateChangeDefault(BtnExt* ext) {
	if (isBtnSupportsTexture(ext->dispSetup)) {
		guiUpdateRectBySpriteRowColInSpriteSheet(ext->srcFrameRect,
				0, (int) ext->btnState,
				BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	}
}

// Background Color behavior - default template
void updateBtnBgColorOnStateChangeDefault(Widget* widget){
	BtnExt* ext = (BtnExt*) widget->ext;

	// Default bgColors
	static SDL_Color defColorsSet[6] = {
			{193,164,189, SDL_ALPHA_OPAQUE}, 	// BTN_NORMAL_UNSELECTED
			{227,37,202, SDL_ALPHA_OPAQUE}, 	// BTN_PRESSED_DOWN_UNSELECTED
			{0,255,0, SDL_ALPHA_OPAQUE},	 	// BTN_NORMAL_SELECTED
			{0,182,30, SDL_ALPHA_OPAQUE}, 		// BTN_PRESSED_DOWN_SELECTED
			{156,156,156, SDL_ALPHA_OPAQUE},	// BTN_NOT_ACTIVE_UNSELECTED
			{156,255,156, SDL_ALPHA_OPAQUE}	 	// BTN_NOT_ACTIVE_SELECTED
	};

	widget->bgColor = &(defColorsSet[ext->btnState]);
}


void loadTextureForBtnDisplayType(Widget* widget, const char* imagePath){
	if (widget == NULL) return;
	BtnExt* ext = (BtnExt*) widget->ext;
	if (isBtnSupportsTexture(ext->dispSetup)) {
		ext->texture = guiLoadTexture(imagePath, widget->renderer);
	}
	else {
		ext->texture = NULL;
	}
}


bool shouldDrawBtnBgColor(BtnDisplayMode dispMode){
	return (dispMode == BTN_DISP_BG_ONLY ||
			dispMode == BTN_DISP_BG_AND_SPRITE);
}

bool shouldDrawBtnSprite(BtnDisplayMode dispMode){
	return (dispMode == BTN_DISP_SPRITE_ONLY ||
			dispMode == BTN_DISP_BG_AND_SPRITE);
}

bool isBtnSupportsTexture(BtnSetupDisplay dispSetup){
	return (dispSetup != BTN_SETUP_BG_ONLY);
}


BtnDisplayMode getDispModeForDispSetup(BtnSetupDisplay dispSetup){
	return (BtnDisplayMode) dispSetup;
}

void updateBtnTargetRect(Widget* widget, SDL_Rect* rect){
	if (widget == NULL || widget->ext == NULL) return;
	guiUpdateRectFromRect(widget->targetRect, rect);
}




// === EVENT HANDLER

// Wraps and handles the gui behavior as a standard button
// Delegates handling non-gui logic, to user's btnEventHandler, via a btnEvent
// Use alt versions to create different buttons with different gui behaviors
void onSdlEventHandlerBtnGuiWrapper(Widget* widget, SDL_Event* event) {
	if (widget == NULL || event == NULL) {return;}
	// === PREPS
	SDL_Point point;
	point.x = event->button.x;
	point.y = event->button.y;
	bool inTargetArea = SDL_PointInRect(&point, widget->targetRect);
	BtnExt* ext = (BtnExt*) widget->ext;
	Uint8 sdlMouseBtn = event->button.button;
	BtnEvent btnEvent = BTN_EVENT_DO_NOTHING;

	// === EXIT TRIGGERS Don't handle events if hidden or not active
	if (isHidden(widget) || !isBtnActive(widget)) return;

	//  === GUI BUTTON BEHAVIOR
	if (inTargetArea) { // IN TARGET AREA
		switch (event->type) {
		case SDL_MOUSEBUTTONUP :
			if (isBtnPressedDown(widget)) {  // COMPLETE IF STARTED CLICK PROCESS
				if (sdlMouseBtn == SDL_BUTTON_LEFT) {
					if (ext->selectType == BTN_SETUP_ONE_STATE)
						setBtnNormalSameSelect(widget, W_CMD_DRAW_AND_RENDER);
					else setBtnNormalFlipSelect(widget, W_CMD_DRAW_AND_RENDER);
					btnEvent = BTN_EVENT_ON_LEFT_CLICK_COMPLETED;
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN :
			if (!isBtnPressedDown(widget)) {  // START CLICK PROCESS
				if (sdlMouseBtn == SDL_BUTTON_LEFT) {
					setBtnPressedSameSelect(widget, W_CMD_DRAW_AND_RENDER);
					btnEvent = BTN_EVENT_ON_LEFT_PRESSED_DOWN;
				}
			}
			break;
		default : break;
		}
	}
	else { //NOT IN TARGET AREA
		switch (event->type) {
		case SDL_MOUSEBUTTONUP : // CANCEL IF STARTED CLICK PROCESS
			if (isBtnPressedDown(widget)) {
				if (sdlMouseBtn == SDL_BUTTON_LEFT) {
					setBtnNormalSameSelect(widget, W_CMD_DRAW_AND_RENDER);
					btnEvent = BTN_EVENT_ON_LEFT_CLICK_CANCELED;
				}
			}
			break;
		default :
			break;
		}
	}

	// === FORWARD BTN EVENT
	if (btnEvent != BTN_EVENT_DO_NOTHING) {
		ext->btnEventHandler(widget, &btnEvent);
	}
}


void onBtnEventHandlerEmpty (Widget* widget, BtnEvent* btnEvent){
	if (widget || btnEvent) {};
}





// == HELPERS - BUTTON STATE AND DISPLAY TOGGLES


bool isBtnActive(Widget* widget){
	if (widget == NULL) return 0;
	BtnExt* ext = (BtnExt*) widget->ext;
	return !(ext->btnState == BTN_NOT_ACTIVE_UNSELECTED ||
			ext->btnState == BTN_NOT_ACTIVE_SELECTED);
}

bool isBtnSelected(Widget* widget){
	if (widget == NULL) return 0;
	BtnExt* ext = (BtnExt*) widget->ext;
	return (ext->btnState == BTN_NORMAL_SELECTED ||
			ext->btnState == BTN_PRESSED_DOWN_SELECTED||
			ext->btnState == BTN_NOT_ACTIVE_SELECTED);
}


bool isBtnPressedDown(Widget* widget) {
	if (widget == NULL) return 0;
	BtnExt* ext = (BtnExt*) widget->ext;
	if (ext->btnState == BTN_PRESSED_DOWN_UNSELECTED
			|| ext->btnState == BTN_PRESSED_DOWN_SELECTED)
		return true;
	else
		return false;
}



void setBtnState(Widget* widget, BtnState state, WidgetCmdDraw drawCmd){
	if (widget == NULL) {printErr(ERR_FUNC_FAILED, __func__); return;};
	BtnExt* ext = (BtnExt*) widget->ext;
	ext->btnState = state;
	ext->updateBgColorForState(widget);
	if (isBtnSupportsTexture(ext->dispSetup))
		ext->updateSpriteSrcFrameForState(ext);
	drawWidget(widget, drawCmd);
}


void setBtnDisplayMode(Widget* widget, BtnDisplayMode dispMode, WidgetCmdDraw drawCmd){
	if (widget == NULL) return;
	BtnExt* ext = (BtnExt*) widget->ext;

	// Don't allow unsupported display modes
	if (shouldDrawBtnSprite(dispMode) && !isBtnSupportsTexture(ext->dispSetup)) {
		printErr(ERR_FUNC_FAILED, __func__); // Should not be asked
		return;
	}

	ext->activeDispMode = dispMode;

	// If supporting transparency, or removing background,
	// Make sure to Draw widget's Parent at widget's target region
	drawWidgetAtTarget(widget->parent, W_CMD_DRAW_ONLY_NO_RENDER, widget->targetRect);

	// Now draw widget
	drawWidget(widget, drawCmd);
}


void setBtnFlipPressSameSelect(Widget* widget, WidgetCmdDraw drawCmd) {
	if (widget == NULL) return;
	if (isBtnPressedDown(widget))
		setBtnNormalSameSelect(widget, drawCmd);
	else
		setBtnPressedSameSelect(widget, drawCmd);
}

void setBtnPressedSameSelect(Widget* widget, WidgetCmdDraw drawCmd){
	if (widget == NULL) return;
	BtnExt* ext = (BtnExt*) widget->ext;
	if (ext->btnState == BTN_NORMAL_UNSELECTED)
		setBtnState(widget, BTN_PRESSED_DOWN_UNSELECTED, drawCmd);
	else if (ext->btnState == BTN_NORMAL_SELECTED)
		setBtnState(widget, BTN_PRESSED_DOWN_SELECTED, drawCmd);
}


void setBtnNormalSameSelect(Widget* widget, WidgetCmdDraw drawCmd) {
	if (widget == NULL) return;
	BtnExt* ext = (BtnExt*) widget->ext;
	if (ext->btnState == BTN_PRESSED_DOWN_UNSELECTED)
		setBtnState(widget, BTN_NORMAL_UNSELECTED, drawCmd);
	else if (ext->btnState == BTN_PRESSED_DOWN_SELECTED)
		setBtnState(widget, BTN_NORMAL_SELECTED, drawCmd);
}

void setBtnNormalFlipSelect(Widget* widget, WidgetCmdDraw drawCmd) {
	if (widget == NULL) return;
	BtnExt* ext = (BtnExt*) widget->ext;
	if (ext->btnState == BTN_PRESSED_DOWN_UNSELECTED)
		setBtnState(widget, BTN_NORMAL_SELECTED, drawCmd);
	else if (ext->btnState == BTN_PRESSED_DOWN_SELECTED)
		setBtnState(widget, BTN_NORMAL_UNSELECTED, drawCmd);
}

void setBtnNormalCancelSelect(Widget* widget, WidgetCmdDraw drawCmd){
	if (widget == NULL) return;
	BtnExt* ext = (BtnExt*) widget->ext;
	if (ext->btnState == BTN_NORMAL_SELECTED ||
			ext->btnState == BTN_PRESSED_DOWN_SELECTED) {
		setBtnState(widget, BTN_NORMAL_UNSELECTED, drawCmd);
	}
}


void setBtnInactiveSameSelect(Widget* widget, WidgetCmdDraw drawCmd){
	if (widget == NULL) return;
	if (isBtnSelected(widget))
		setBtnState(widget, BTN_NOT_ACTIVE_SELECTED, drawCmd);
	else
		setBtnState(widget, BTN_NOT_ACTIVE_UNSELECTED, drawCmd);
}


void setBtnNormalActiveUnselected(Widget* widget, WidgetCmdDraw drawCmd){
	if (widget == NULL) return;
	setBtnState(widget, BTN_NORMAL_UNSELECTED, drawCmd);
}



// == HELPERS - COLORS BG AND INTERACTION





void setBtnBgColor(Widget* widget, SDL_Color* color, WidgetCmdDraw drawCmd){
	if (widget == NULL) return;
	widget->bgColor = color;
	// support transparency
	drawWidgetAtTarget(widget->parent, W_CMD_DRAW_ONLY_NO_RENDER, widget->targetRect);
	drawWidget(widget, drawCmd);
}


void restoreBtnBgColor(Widget* widget, WidgetCmdDraw drawCmd){
	BtnExt* ext = (BtnExt*) widget->ext;
	ext->updateBgColorForState(widget);
	// support transparency
	drawWidgetAtTarget(widget->parent, W_CMD_DRAW_ONLY_NO_RENDER, widget->targetRect);
	drawWidget(widget, drawCmd);
}


void setBtnUserData(Widget* widget, int data){
	if (widget == NULL) return;
	BtnExt* ext = (BtnExt*) widget->ext;
	ext->userData = data;
}

int getBtnUserData(Widget* widget){
	if (widget == NULL) return 0;
	else return (((BtnExt*)(widget->ext))->userData);

}




