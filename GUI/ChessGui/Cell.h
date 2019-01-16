/*
 * ButtonHighlighted.h
 *
 *  Created on: Sep 7, 2017
 *      Author: user01
 */

#ifndef CELL_H_
#define CELL_H_


#include "../GuiApi/Button.h"


#define CELL_SPRITE_WIDTH 100
#define CELL_SPRITE_HEIGHT 100
#define CELL_BTN_DISP_MODE_DEF BTN_DISP_BG_AND_SPRITE
#define CELL_HOVERER_MOUSE_X_ADJUST CELL_SPRITE_WIDTH/2
#define CELL_HOVERER_MOUSE_Y_ADJUST CELL_SPRITE_HEIGHT/2



typedef enum t_chess_cell_piece_color{
	CELL_PIECE_BLACK,
	CELL_PIECE_WHITE,
	CELL_PIECE_NONE,
} CellPieceColor;


typedef enum t_chess_cell_sprites{
	CELL_SPRITE_KING,
	CELL_SPRITE_QUEEN,
	CELL_SPRITE_ROOK,
	CELL_SPRITE_BISHOP,
	CELL_SPRITE_KNIGHT,
	CELL_SPRITE_PAWN,
	CELL_SPRITE_EMPTY,
} CellSprite;


typedef enum t_chess_cell_state{
	CELL_STATE_NORMAL = 0,
	CELL_STATE_PRESSED_LEFT_DRAG_STARTED_SOURCE,
	CELL_STATE_PRESSED_RIGHT_MARK_MOVES_MODE,
	CELL_STATE_LOCKED_FOR_DRAG_OP,
	CELL_HOVERER_STATE_HOVER,
	CELL_HOVERER_STATE_HIDE,
} CellState;




// Button Extension: Cell
// * brief : Button support for a chess cell with changeable colored background under the image
typedef struct t_button_extension_cell CellExt;
struct t_button_extension_cell{

	CellState cellState;
	CellSprite curSprite;
	CellPieceColor curPieceColor;

};




// === DESTROY



// === CREATE
Widget* createCell(SDL_Renderer* renderer, SDL_Rect* targetRect,
		const char* imagePath, BtnEventHandler btnEventHandler);


// === DRAW

void updateCellSpriteSrcFrameOnStateChange(BtnExt* ext);
void updateCellBgColorOnStateChangeDefault(Widget* widget);


// === DRAW - HELPERS

void setCellState(Widget* widget, CellState cellState, WidgetCmdDraw drawCmd);

void setCellSpriteAndPieceColor(Widget* widget,
		CellSprite curSprite, CellPieceColor curPieceColor);

void setCellSpriteAndPieceColorAndRenderPresent(Widget* widget,
		CellSprite curSprite, CellPieceColor curPieceColor);

void setCellEmptyAndRenderPresent(Widget* widget);


// === EVENT HANDLER
void setupCellsDragdropHoverer(Widget* widget, const SDL_Rect* rect);
void onSdlEventHandlerCellGuiWrapper(Widget* widget, SDL_Event* event);





#endif /* CELL_H_ */
