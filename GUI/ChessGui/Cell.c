

#include "../ChessGui/Cell.h"


// === Drag and Drop Op static helpers
static Widget* singleDragHoverer = NULL;
static SDL_Rect dragdropBorder = {0,0,0,0};
static Widget* singleDragSource = NULL;
static Widget* singleDragTarget = NULL;
static Widget* singleActiveRightModeCell = NULL;


// === CREATE

Widget* createCell(SDL_Renderer* renderer, SDL_Rect* targetRect,
		const char* imagePath,
		BtnEventHandler btnEventHandler){

	Widget* widget = createButton(renderer, targetRect,
			CELL_BTN_DISP_MODE_DEF, imagePath,
			NULL, NULL, BTN_SETUP_TWO_STATE, btnEventHandler);

	if (widget == NULL) return NULL; // Creation Null check

	// Allocate data for button aspect
	CellExt* ext = (CellExt*) myMalloc(sizeof(CellExt), __func__, __LINE__);
	// Allocation Null check
	if (ext == NULL) {
		myFree(ext, __func__, __LINE__);
		destroyButton(widget);
		return NULL ;
	}

	// ** Link Extension structures
	BtnExt* btnExt = (BtnExt*) widget->ext;
	btnExt->ext = ext;

	// Cell Extension
	ext->cellState = CELL_STATE_NORMAL;
	ext->curPieceColor = CELL_PIECE_WHITE;
	ext->curSprite = CELL_SPRITE_ROOK;


	// ** Override Button aspect
	// 	.. Background Color
	btnExt->updateBgColorForState = updateCellBgColorOnStateChangeDefault;
	btnExt->updateBgColorForState(widget); // (sets bgColor)
	// *** SrcFramRect:
	btnExt->updateSpriteSrcFrameForState = updateCellSpriteSrcFrameOnStateChange;
	btnExt->updateSpriteSrcFrameForState(btnExt); // (sets srcFrameRect)

	// ** Widget Aspect override
	widget->handleSDLEvent = onSdlEventHandlerCellGuiWrapper;


	// DONE!
	return widget;
}


// === DRAW

void updateCellSpriteSrcFrameOnStateChange(BtnExt* btnExt) {
	CellExt* cellExt = (CellExt*) btnExt->ext;

	// int spriteRowIdx = (int) cellExt->curPieceColor;

	if (cellExt->cellState == CELL_STATE_PRESSED_LEFT_DRAG_STARTED_SOURCE) {
		return;
	}

	guiUpdateRectBySpriteRowColInSpriteSheet(btnExt->srcFrameRect,
			((int) cellExt->curPieceColor), ((int) cellExt->curSprite),
			CELL_SPRITE_WIDTH, CELL_SPRITE_HEIGHT);
}



void updateCellBgColorOnStateChangeDefault(Widget* widget){
	BtnExt* btnExt = (BtnExt*) widget->ext;
	CellExt* cellExt = (CellExt*) btnExt->ext;


	// Default bgColors
	static SDL_Color defColorsSet[6] = {
			{104,168,255, 0}, 					// CELL_STATE_NORMAL
			{255,180,104, SDL_ALPHA_OPAQUE}, 					// CELL_STATE_PRESSED_LEFT_DRAG_STARTED_SOURCE
			{158,103,233, SDL_ALPHA_OPAQUE},					// CELL_STATE_PRESSED_RIGHT_MARK_MOVES_MODE
			{5,5,5, SDL_ALPHA_OPAQUE}, 						// CELL_STATE_LOCKED_FOR_DRAG_OP
			{114,225,114, SDL_ALPHA_OPAQUE},					// CELL_HOVERER_STATE_HOVER
			{20,20,20, SDL_ALPHA_OPAQUE}	 					// CELL_HOVERER_STATE_HIDE
	};

	widget->bgColor = &(defColorsSet[cellExt->cellState]);
}


// === DRAW - HELPERS


void setCellState(Widget* widget, CellState cellState, WidgetCmdDraw drawCmd){

	if (widget == NULL) return;

	BtnExt* btnExt = (BtnExt*) widget->ext;
	CellExt* cellExt = (CellExt*) btnExt->ext;
	CellState formerState = cellExt->cellState;

	// Set cell state
	cellExt->cellState = cellState;

	// Refresh cell bg color and sprite as needed
	btnExt->updateBgColorForState(widget);
	if (isBtnSupportsTexture(btnExt->dispSetup))
		btnExt->updateSpriteSrcFrameForState(btnExt);


	// REGULAR CELLS : ENTER / LEAVE MODES - LEFT OR RIGHT
	if (formerState == CELL_STATE_NORMAL) {
		// Enter Left Mode
		if (cellState == CELL_STATE_PRESSED_LEFT_DRAG_STARTED_SOURCE)
		setBtnPressedSameSelect(widget, W_CMD_NO_DRAW_NO_RENDER);
		setBtnDisplayMode(widget, BTN_DISP_BG_ONLY, drawCmd);
		// Enter Right Mode
		if (cellState == CELL_STATE_PRESSED_RIGHT_MARK_MOVES_MODE)
			setBtnPressedSameSelect(widget, W_CMD_NO_DRAW_NO_RENDER);
		setBtnDisplayMode(widget, BTN_DISP_BG_AND_SPRITE, drawCmd);
	}
	else if (cellState == CELL_STATE_NORMAL) {
		setBtnNormalSameSelect(widget, W_CMD_NO_DRAW_NO_RENDER);
		setBtnDisplayMode(widget, CELL_BTN_DISP_MODE_DEF, drawCmd);
	}


	// HOVERER CELL : ENTER / LEAVE MODE
	if (cellState == CELL_HOVERER_STATE_HIDE) {
		hideWidget(widget, W_CMD_SET_HIDDEN, W_CMD_DRAW_AND_RENDER);
	}
	else if (cellState == CELL_HOVERER_STATE_HOVER) {
		hideWidget(widget, W_CMD_SET_VISIBLE, W_CMD_DRAW_AND_RENDER);
	}

}


void setCellSpriteAndPieceColor(Widget* widget,
		CellSprite curSprite, CellPieceColor curPieceColor){
	CellExt* ext = (CellExt*) ((BtnExt*) widget->ext)->ext;
	ext->curSprite = curSprite;
	ext->curPieceColor = curPieceColor;
}

void setCellSpriteAndPieceColorAndRenderPresent(Widget* widget,
		CellSprite curSprite, CellPieceColor curPieceColor){
	setCellSpriteAndPieceColor(widget, curSprite, curPieceColor);
	hideWidget(widget, W_CMD_SET_HIDDEN, W_CMD_DRAW_AND_RENDER);
	hideWidget(widget, W_CMD_SET_VISIBLE, W_CMD_DRAW_AND_RENDER);
	// TODO
	BtnExt* btnExt =(BtnExt*)widget->ext;
	btnExt->updateSpriteSrcFrameForState(btnExt);
	drawWidget(widget, W_CMD_DRAW_AND_RENDER);
}

void setCellEmptyAndRenderPresent(Widget* widget){
	setCellSpriteAndPieceColorAndRenderPresent(widget, CELL_SPRITE_EMPTY, CELL_PIECE_NONE);
}


// === EVENT HANDLER


void setupCellsDragdropHoverer(Widget* widget, const SDL_Rect* rect){
	singleDragHoverer = widget;
	setCellState(singleDragHoverer, CELL_HOVERER_STATE_HIDE, W_CMD_DRAW_AND_RENDER);
	dragdropBorder = *rect;
	// Adjust for cell drag hoverer adjustments
	dragdropBorder.x += CELL_HOVERER_MOUSE_X_ADJUST + 10;
	dragdropBorder.y += CELL_HOVERER_MOUSE_X_ADJUST + 10;
	dragdropBorder.w -= 2*CELL_HOVERER_MOUSE_X_ADJUST - 20;
	dragdropBorder.h -= 2*CELL_HOVERER_MOUSE_X_ADJUST -20;

}





void onSdlEventHandlerCellGuiWrapper(Widget* widget, SDL_Event* event){

	if (widget == NULL || event == NULL) {return;}
	// === PREPS
	SDL_Point point;
	point.x = event->button.x;
	point.y = event->button.y;
	bool inTargetArea = SDL_PointInRect(&point, widget->targetRect);
	BtnExt* btnExt = (BtnExt*) widget->ext;
	CellExt* cellExt = (CellExt*) btnExt->ext;
	Uint8 sdlMouseBtn = event->button.button;
	BtnEvent btnEvent = BTN_EVENT_DO_NOTHING;


	// === EXIT TRIGGERS Don't handle events if hidden or not active
	if (isHidden(widget) || !isBtnActive(widget)) return;


	//  === GUI BUTTON BEHAVIOR

	// ** For hoverer cell only
	if (widget == singleDragHoverer){
		switch (event->type) {
		case SDL_MOUSEMOTION :
			// A drag and drop is in progress
			if (cellExt->cellState == CELL_HOVERER_STATE_HOVER && singleDragSource != NULL) { // Internal check
				if (SDL_PointInRect(&point, &dragdropBorder)) { // In allowed drag borders
					// Follow cursor
					moveWidgetToPos(widget,
							point.x-CELL_HOVERER_MOUSE_X_ADJUST,
							point.y-CELL_HOVERER_MOUSE_Y_ADJUST,
							W_CMD_DRAW_AND_RENDER);
				}
			}
			break;
		case SDL_MOUSEBUTTONUP :
			if (sdlMouseBtn == SDL_BUTTON_LEFT)
				if (cellExt->cellState == CELL_HOVERER_STATE_HOVER)// ** For Hoverer - make hidden
					setCellState(singleDragHoverer, CELL_HOVERER_STATE_HIDE, W_CMD_DRAW_AND_RENDER);
		}
	}

	// ** For regular cells
	else {
		if (inTargetArea) { // IN TARGET AREA
			switch (event->type) {

			case SDL_MOUSEBUTTONUP :
				if (sdlMouseBtn == SDL_BUTTON_LEFT && singleActiveRightModeCell == NULL) {
					// Drop on source cell - cancel
					if (cellExt->cellState == CELL_STATE_PRESSED_LEFT_DRAG_STARTED_SOURCE) {
						setCellState(widget, CELL_STATE_NORMAL, W_CMD_DRAW_AND_RENDER);
						btnEvent = BTN_EVENT_ON_LEFT_CLICK_CANCELED;
					}
					else {
						// Drop on non-source cell - drop TARGET
						singleDragTarget = widget;
						btnEvent = BTN_EVENT_ON_LEFT_CLICK_COMPLETED;
					}
				}
				break;

			case SDL_MOUSEBUTTONDOWN :
				if (sdlMouseBtn == SDL_BUTTON_LEFT && singleActiveRightModeCell == NULL){ // Enter drag drop mode - drag SOURCE
					if(cellExt->cellState == CELL_STATE_NORMAL) { // Can only press down on non-pressed cells
						setCellState(widget, CELL_STATE_PRESSED_LEFT_DRAG_STARTED_SOURCE, W_CMD_DRAW_AND_RENDER);
						singleDragSource = widget;
						btnEvent = BTN_EVENT_ON_LEFT_PRESSED_DOWN;
						// Setup hoverer
						setCellState(singleDragHoverer, CELL_HOVERER_STATE_HOVER, W_CMD_NO_DRAW_NO_RENDER);
						moveWidgetToPos(singleDragHoverer,
								point.x-CELL_HOVERER_MOUSE_X_ADJUST,
								point.y-CELL_HOVERER_MOUSE_Y_ADJUST,
								W_CMD_DRAW_AND_RENDER);
					}
				}
				else if (sdlMouseBtn == SDL_BUTTON_RIGHT && singleDragSource == NULL) {
					// TOGGLE right mark mode
					if(cellExt->cellState == CELL_STATE_NORMAL && singleActiveRightModeCell == NULL) {
						// ** Enter mode
						setCellState(widget, CELL_STATE_PRESSED_RIGHT_MARK_MOVES_MODE, W_CMD_DRAW_AND_RENDER);
						btnEvent = BTN_EVENT_ON_RIGHT_PRESSED_DOWN;
						singleActiveRightModeCell = widget;

					}
					else if (cellExt->cellState == CELL_STATE_PRESSED_RIGHT_MARK_MOVES_MODE) {
						// ** Leave mode
						setCellState(widget, CELL_STATE_NORMAL, W_CMD_DRAW_AND_RENDER);
						btnEvent = BTN_EVENT_ON_LEFT_CLICK_CANCELED; // Cancel Right press mode
						singleActiveRightModeCell = NULL;
					}
				}
				break;

			default : break;
			}
		}
		else { // NOT IN TARGET AREA
			switch (event->type) {
			case SDL_MOUSEBUTTONUP :
				// Left up treats only when left pressed
				if (event->button.button == SDL_BUTTON_LEFT && singleActiveRightModeCell == NULL) {
					if (cellExt->cellState == CELL_STATE_PRESSED_LEFT_DRAG_STARTED_SOURCE) {
						// Drop on non-source cell - for drop SOURCE
						setCellState(widget, CELL_STATE_NORMAL, W_CMD_DRAW_AND_RENDER);
						btnEvent = BTN_EVENT_ON_LEFT_CLICK_COMPLETED;
					}
				}
				break;
			default :
				break;
			}
		}
	}


	// === FORWARD BTN EVENT
	if (btnEvent != BTN_EVENT_DO_NOTHING) {
		btnExt->btnEventHandler(widget, &btnEvent);
		// Reset drag and drop source and target after end-handling complete
		if (btnEvent == BTN_EVENT_ON_LEFT_CLICK_CANCELED || btnEvent == BTN_EVENT_ON_LEFT_CLICK_COMPLETED) {
			singleDragSource = NULL;
		}
	}
}














// === HELPERS - Toggles






