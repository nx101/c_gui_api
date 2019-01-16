#include "AffecterBtn.h"


// === DESTROY

void destroyAffecter(Widget* widget) {

	// Args Null check
		if (widget == NULL) {
			return;
		}

		//BtnExt* btnExt = (BtnExt*) widget->ext;
		//AffecterBtnExt* ext = (AffecterBtnExt*) btnExt->ext;

}



// === CREATE - Templates

Widget* createAffecterSimple1(
		SDL_Renderer* renderer, SDL_Rect* targetRect,
		BtnEventHandler btnEventHandler,
		Widget** notifiedWidgets, int numNotifiedWidgets,
		BtnEventAffectNotifiedWidgets btnEventAffectNotifiedWidgets){

	return createButtonAffecter(renderer, targetRect,
			BTN_SETUP_BG_AND_SPRITE, BTN_SPRITES_BMP_DEF,
			NULL, NULL, BTN_SETUP_TWO_STATE, btnEventHandler,
			notifiedWidgets, numNotifiedWidgets,
			btnEventAffectNotifiedWidgets);
}


// === CREATE - Factory

Widget* createButtonAffecter(SDL_Renderer* renderer, SDL_Rect* targetRect,
		BtnDisplayMode dispMode, const char* imagePath,
		BtnUpdateBgColorOnStateChange updateBgColorForState,
		BtnUpdateSpriteSrcFrameOnStateChange updateSpriteSrcFrameForState,
		BtnSetupSelectType type, BtnEventHandler btnEventHandler,
		Widget** notifiedWidgets, int numNotifiedWidgets,
		BtnEventAffectNotifiedWidgets btnEventAffectNotifiedWidgets){


	Widget* res = createButton(
			renderer, targetRect,
			dispMode, imagePath,
			updateBgColorForState,
			updateSpriteSrcFrameForState,
			type, btnEventHandler);
	if (res == NULL) {
		destroyWidget(res);
		return NULL; // Creation Null check
	}

	// Allocate data for button aspect
	AffecterBtnExt* ext = (AffecterBtnExt*) myMalloc(sizeof(AffecterBtnExt), __func__, __LINE__);
	// Allocation Null check
	if (ext == NULL) {
		myFree(ext, __func__, __LINE__);
		destroyWidget(res);
		return NULL ;
	}


	// ** Init Widget
	// By upper button extension

	// Update Button aspect
	BtnExt* btnExt = (BtnExt*) res->ext;
	btnExt->ext = ext;
	btnExt->btnEventHandler = onBtnEventAffecterWrapper;

	// Init Affecter aspect
	ext->btnEventHandler = btnEventHandler;
	ext->notifiedWidgets = notifiedWidgets;
	ext->numNotifiedWidgets = numNotifiedWidgets;
	ext->btnEventAffectNotifiedWidgets = btnEventAffectNotifiedWidgets;


	// DONE!

	return res;
}



// === EVENT HANDLER

void onBtnEventAffecterWrapper(Widget* widget, BtnEvent* btnEvent){
	BtnExt* btnExt = (BtnExt*) widget->ext;
	AffecterBtnExt* ext = (AffecterBtnExt*) btnExt->ext;
	// Run provided btnEventHandler
	ext->btnEventHandler(widget, btnEvent);
	// Affect notified widgets
	ext->btnEventAffectNotifiedWidgets(widget, btnEvent);


}


// === HELPERS

// Run affecterFunction on all affected widgets
void runOnAffectedWidgets(Widget* widget, BtnEvent* btnEvent,
		void (*affectFunction)(Widget* widget)){

	if (widget || btnEvent) {};

	AffecterBtnExt* ext = (AffecterBtnExt*) ((BtnExt*)widget->ext)->ext;
	Widget** notifiedWidgets = ext->notifiedWidgets;

	for (int i=0; i<ext->numNotifiedWidgets; i++){
		if(notifiedWidgets[i] != NULL &&
				notifiedWidgets[i] != widget) { // Affect all others than affecting widget
			affectFunction(notifiedWidgets[i]);
		}
	}

}

void affectWidgetsEmpty(Widget* widget, BtnEvent* btnEvent){
	if (widget || btnEvent) {};
	// Run runOnAffectedWidgets for btnEvent cases
}








