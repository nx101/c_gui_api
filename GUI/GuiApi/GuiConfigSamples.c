


#include "GuiConfigSamples.h"


// Samples: WidgetArray creators



void createWidgetsInArray2TypedBtns(SDL_Renderer* renderer, const SDL_Rect* targetRect, Widget** widgets, int numOfWidgets){

	if (targetRect) {};

	int origin = 20;
	SDL_Rect target;

	for (int i=0; i<numOfWidgets; i++){
		target = guiGetAsRect(origin, origin + i*(BTN_SPRITE_HEIGHT+20),
				BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);

		if (i%2==0) {

			widgets[i] = createButtonSimple1(
					renderer, &target, onBtnEventHandlerEmpty);
		}
		else {
			widgets[i] = createButtonSimple1(
					renderer, &target, onBtnEventHandlerEmpty);
		}
	}
}




void createWidgetsInArraySingleMultiChoiceBtns(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets){

	int originX = 20 + targetRect->x;
	int originY = 20 + targetRect->y;

	SDL_Rect target;

	for (int i=0; i<numOfWidgets; i++){

		target = guiGetAsRect(originX, originY + i*(BTN_SPRITE_HEIGHT+20),
				BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);

		widgets[i] = createAffecterSimple1(
				renderer, &target, onBtnEventHandlerEmpty,
				widgets, numOfWidgets, affectWidgetsSingleMultiChoice);
	}
}



// Template for Single Selection, Multi Choice, Dependency
void affectWidgetsSingleMultiChoice(Widget* widget, BtnEvent* btnEvent){
	switch (*btnEvent) {
	case BTN_EVENT_ON_LEFT_CLICK_COMPLETED :
		runOnAffectedWidgets(widget, btnEvent, setBtnNormalCancelSelectAndRender);
		break;
	case BTN_EVENT_ON_LEFT_PRESSED_DOWN :
		runOnAffectedWidgets(widget, btnEvent, setBtnPressedSameSelectAndRender);
		break;
	default :
		break;

	}
}



// Affecter- multi choice, single select, always one selected
void affectWidgetsBtnMultiChoiceAlwaysOneSelected(Widget* widget, BtnEvent* btnEvent){
	switch (*btnEvent) {
	case BTN_EVENT_ON_LEFT_CLICK_COMPLETED :
		setBtnInactiveSameSelect(widget, W_CMD_DRAW_AND_RENDER);
		runOnAffectedWidgets(widget, btnEvent, setBtnNormalActiveUnselectedAndRender);
		break;
	case BTN_EVENT_ON_LEFT_PRESSED_DOWN :
		if (isBtnSelected(widget)) { // don't allow deselection of selected option
			//SDL_Delay(800);
			setBtnState(widget, BTN_NORMAL_SELECTED, W_CMD_DRAW_AND_RENDER);
		} else {
			runOnAffectedWidgets(widget, btnEvent, setBtnPressedSameSelectAndRender);
		}
		break;
	default :
		break;

	}
}











void onBtnEventHandlerEmptySample (Widget* widget, BtnEvent* btnEvent){
	if (widget || btnEvent) {};
}


void setBtnNormalCancelSelectAndRender(Widget* widget){
	setBtnNormalCancelSelect(widget, W_CMD_DRAW_AND_RENDER);
}


void setBtnNormalActiveUnselectedAndRender(Widget* widget){
	setBtnNormalActiveUnselected(widget, W_CMD_DRAW_AND_RENDER);
}


void setBtnPressedSameSelectAndRender(Widget* widget){
	setBtnPressedSameSelect(widget, W_CMD_DRAW_AND_RENDER);
}



void toggleHideWidgetNoRender(Widget* widget){
	hideWidget(widget, W_CMD_TOGGLE_SAVE_RESTORE, W_CMD_DRAW_ONLY_NO_RENDER);
}
