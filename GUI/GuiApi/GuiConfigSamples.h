/*
 * GuiConfigSamples.h
 *
 *  Created on: Sep 11, 2017
 *      Author: user01
 */

#ifndef GUICONFIGSAMPLES_H_
#define GUICONFIGSAMPLES_H_

#include "AffecterBtn.h"
#include "Button.h"
#include "../../MemGateway/MemGateway.h"


void createWidgetsInArray2TypedBtns(SDL_Renderer* renderer, const SDL_Rect* targetRect, Widget** widgets, int numOfWidgets);
void createWidgetsInArraySingleMultiChoiceBtns(SDL_Renderer* renderer, const SDL_Rect* targetRect, Widget** widgets, int numOfWidgets);

void affectWidgetsSingleMultiChoice(Widget* widget, BtnEvent* btnEvent);
void affectWidgetsBtnMultiChoiceAlwaysOneSelected(Widget* widget, BtnEvent* btnEvent);
void onBtnEventHandlerEmptySample (Widget* widget, BtnEvent* btnEvent);


// HELPER FUNCTIONS

void setBtnNormalCancelSelectAndRender(Widget* widget);
void toggleHideWidgetNoRender(Widget* widget);
void setBtnNormalActiveUnselectedAndRender(Widget* widget);
void setBtnPressedSameSelectAndRender(Widget* widget);


#endif /* GUICONFIGSAMPLES_H_ */
