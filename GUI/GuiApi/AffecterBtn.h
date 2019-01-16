/*
 * ButtonCanAffect.h
 *
 *  Created on: Sep 5, 2017
 *      Author: user01
 */

#ifndef AFFECTERBTN_H_
#define AFFECTERBTN_H_

#include "Button.h"


typedef void (*BtnEventAffectNotifiedWidgets)(Widget* widget, BtnEvent* btnEvent);

// Button Extension: Affecter
typedef struct t_button_extension_Affecter AffecterBtnExt;
struct t_button_extension_Affecter{

	// User onBtnEventHandler
	BtnEventHandler btnEventHandler;

	// Affecter onEvent Handler
	BtnEventAffectNotifiedWidgets btnEventAffectNotifiedWidgets;

	// Affected (Notified) Widgets
	Widget** notifiedWidgets;
	int numNotifiedWidgets;

};


// === DESTROY

void destroyAffecter(Widget* widget);


// === CREATE - Templates

Widget* createAffecterSimple1(SDL_Renderer* renderer, SDL_Rect* targetRect,
		BtnEventHandler btnEventHandler,
		Widget** notifiedWidgets, int numNotifiedWidgets,
		BtnEventAffectNotifiedWidgets btnEventAffectNotifiedWidgets);

// === CREATE - Factory
Widget* createButtonAffecter(
		SDL_Renderer* renderer, SDL_Rect* targetRect,
		BtnDisplayMode dispMode, const char* imagePath,
		BtnUpdateBgColorOnStateChange updateBgColorForState,
		BtnUpdateSpriteSrcFrameOnStateChange updateSpriteSrcFrameForState,
		BtnSetupSelectType type, BtnEventHandler btnEventHandler,
		Widget** notifiedWidgets, int numNotifiedWidgets,
		BtnEventAffectNotifiedWidgets btnEventAffectNotifiedWidgets);

// === EVENT HANDLER
void onBtnEventAffecterWrapper(Widget* widget, BtnEvent* btnEvent);

// === HELPERS
void runOnAffectedWidgets(Widget* widget, BtnEvent* btnEvent,
		void (*affectFunction)(Widget* widget));

void affectWidgetsEmpty(Widget* widget, BtnEvent* btnEvent);

#endif /* AFFECTERBTN_H_ */
