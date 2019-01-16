#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdbool.h>
#include <stdio.h>

#include "GuiCommonSdlHelpers.h"
#include "WidgetPrimitive.h"

// SDL BUTTON
// via widget extension


#define BTN_SPRITE_WIDTH 80
#define BTN_SPRITE_HEIGHT 40
#define BTN_SPRITES_BMP_DEF "./GUI/GuiApi/buttonSprites.bmp"

#define BTN_USER_DATA_DEF 0


// === Function flags
typedef enum t_func_flags { // Keep in order with BtnSetupDisplay
	SAME = 0,
	FLIP = 1,
} FuncFlag;


// === Button Configuration SETUP

// Button setup - selection type
typedef enum t_button_setup_selection_type {
	BTN_SETUP_ONE_STATE,
	BTN_SETUP_TWO_STATE,
} BtnSetupSelectType;

// Button setup - display assets
typedef enum t_button_setup_display {
	BTN_SETUP_BG_ONLY = 0,
	BTN_SETUP_SPRITE_ONLY = 1,
	BTN_SETUP_BG_AND_SPRITE = 2,
} BtnSetupDisplay;


// === Button STATES

// Internal button selection behavior mechanism
// closely related to BtnSetupType
typedef enum t_button_gui_state {
	BTN_NORMAL_UNSELECTED = 0,
	BTN_PRESSED_DOWN_UNSELECTED,
	BTN_NORMAL_SELECTED,			// only for BTN_TYPE_SELECTION
	BTN_PRESSED_DOWN_SELECTED,		// only for BTN_TYPE_SELECTION
	BTN_NOT_ACTIVE_UNSELECTED,
	BTN_NOT_ACTIVE_SELECTED,
} BtnState;


// Display modes
typedef enum t_button_display_mode { // Keep in order with BtnSetupDisplay
	BTN_DISP_BG_ONLY = 0,
	BTN_DISP_SPRITE_ONLY = 1,
	BTN_DISP_BG_AND_SPRITE = 2,
} BtnDisplayMode;


// Button Events
typedef enum t_button_event {
	BTN_EVENT_ON_LEFT_CLICK_COMPLETED = 0,
	BTN_EVENT_ON_LEFT_PRESSED_DOWN,
	BTN_EVENT_ON_RIGHT_CLICK_COMPLETED,
	BTN_EVENT_ON_RIGHT_PRESSED_DOWN,
	BTN_EVENT_ON_LEFT_CLICK_CANCELED,
	BTN_EVENT_ON_RIGHT_CLICK_CANCELED,
	BTN_EVENT_DO_NOTHING,
} BtnEvent;




// Widget Extension: Button
typedef struct t_widget_extension_button BtnExt;

typedef void (*BtnEventHandler)(Widget* widget, BtnEvent* btnEvent);
typedef void (*BtnUpdateBgColorOnStateChange)(Widget* widget);
typedef void (*BtnUpdateSpriteSrcFrameOnStateChange)(BtnExt* ext);


struct t_widget_extension_button{

	// == Behavior
	BtnSetupSelectType selectType;
	BtnState btnState;
	int userData;
	BtnEventHandler btnEventHandler;
	// == Display
	BtnSetupDisplay dispSetup;
	BtnDisplayMode activeDispMode;
	BtnDisplayMode savedDispMode; // Remember dispMode when hiding
	// 	.. Background Color
	BtnUpdateBgColorOnStateChange updateBgColorForState;
	//SDL_Color* bgColor;
	// 	.. Image Texture
	SDL_Texture* texture;	// Free via SDL
	BtnUpdateSpriteSrcFrameOnStateChange updateSpriteSrcFrameForState;
	SDL_Rect* srcFrameRect; // Free

	// == Extension
	void* ext;
};







// === DESTROY
void destroyButton(Widget*);




// === CREATE - Simple Templates

Widget* createButtonSimple1 (
		SDL_Renderer* renderer, SDL_Rect* targetRect,
		BtnEventHandler btnEventHandler);


// === CREATE - Factory

Widget* createButton(
		SDL_Renderer* renderer, SDL_Rect* targetRect,
		BtnSetupDisplay dispSetup, const char* imagePath,
		BtnUpdateBgColorOnStateChange updateBgColorForState,
		BtnUpdateSpriteSrcFrameOnStateChange updateSpriteSrcFrameForState,
		BtnSetupSelectType type, BtnEventHandler btnEventHandler);


// === DRAW
void drawButton(Widget* widget,  WidgetCmdDraw drawCmd, SDL_Rect* renderTarget);
void drawButtonAndRenderPresent(Widget* widget);

// === DRAW - HELPERS
void updateBtnSrcFrameSpriteUponStateChangeDefault(BtnExt* ext);
void updateBtnBgColorOnStateChangeDefault(Widget* widget);
void loadTextureForBtnDisplayType(Widget* widget, const char* imagePath);
bool shouldDrawBtnBgColor(BtnDisplayMode dispMode);
bool shouldDrawBtnSprite(BtnDisplayMode dispMode);
bool isBtnSupportsTexture(BtnSetupDisplay dispSetup);
BtnDisplayMode getDispModeForDispSetup(BtnSetupDisplay dispSetup);


// === EVENT HANDLER
void onSdlEventHandlerBtnGuiWrapper(Widget* widget, SDL_Event* event);
void onBtnEventHandlerEmpty (Widget* widget, BtnEvent* btnEvent);


// == HELPERS - BUTTON STATE TOGGLES
bool isBtnActive(Widget* widget);
bool isBtnSelected(Widget* widget);
bool isBtnPressedDown(Widget* widget);
void setBtnState(Widget* widget, BtnState state, WidgetCmdDraw drawCmd);
void setBtnDisplayMode(Widget* widget, BtnDisplayMode dispMode, WidgetCmdDraw drawCmd);
void setBtnFlipPressSameSelect(Widget* widget, WidgetCmdDraw drawCmd);
void setBtnPressedSameSelect(Widget* widget, WidgetCmdDraw drawCmd);
void setBtnNormalSameSelect(Widget* widget, WidgetCmdDraw drawCmd);
void setBtnNormalFlipSelect(Widget* widget, WidgetCmdDraw drawCmd);
void setBtnNormalCancelSelect(Widget* widget, WidgetCmdDraw drawCmd);
void setBtnInactiveSameSelect(Widget* widget, WidgetCmdDraw drawCmd);
void setBtnNormalActiveUnselected(Widget* widget, WidgetCmdDraw drawCmd);


// == HELPERS - COLORS BG AND MORE INTERACTIONS
void setBtnBgColor(Widget* widget, SDL_Color* color, WidgetCmdDraw drawCmd);
void restoreBtnBgColor(Widget* widget, WidgetCmdDraw drawCmd);
void setBtnUserData(Widget* widget, int data);
int getBtnUserData(Widget* widget);


#endif
