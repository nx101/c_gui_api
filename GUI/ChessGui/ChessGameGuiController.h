

#ifndef CHESSGAMEGUICONFIG_H_
#define CHESSGAMEGUICONFIG_H_


#include <stdbool.h>
#include <stdio.h>

#include "Cell.h"
#include "../GuiApi/AffecterBtn.h"
#include "../GuiApi/Button.h"
#include "../GuiApi/GuiConfigSamples.h"
#include "../GuiApi/Panel.h"
#include "../GuiApi/WidgetPrimitive.h"
#include "../GuiApi/Window.h"
#include "../../ChessGame/ChessGame.h"
#include "../../ChessGame/ChessAI.h"

// LAYOUT CONSTS

// ** Main window:
#define GUI_MAIN_WINDOW_TITLE "Chess Game"
#define GUI_MAIN_WINDOW_NUMOF_PANELS 7
#define GUI_MAIN_WIN_WIDTH 1400
#define GUI_MAIN_WIN_HEIGHT 800

// ** Chess board:
#define GUI_BOARD_WIDTH (GUI_MAIN_WIN_WIDTH-GUI_MAIN_SIDE_PANEL_WIDTH)
#define GUI_BOARD_HEIGHT (GUI_MAIN_WIN_HEIGHT)
#define GUI_BOARD_BMP "./GUI/ChessGui/chessboardBg.bmp"
#define CHESS_CELLS_NUMOF_X 8
#define CHESS_CELLS_NUMOF_Y 8
#define GUI_CELL_WIDTH_OR_HEIGHT 100
#define GUI_CELL_BMP "./GUI/ChessGui/chessPiecesSprites.bmp"

// ** Side Panel:
#define GUI_MAIN_SIDE_PANEL_WIDTH 600
#define GUI_MAIN_SIDE_PANEL_HEIGHT 800

#define GUI_PANEL_MENU_X 0
#define GUI_PANEL_MENU_Y 0
#define GUI_PANEL_MENU_WIDTH GUI_MAIN_SIDE_PANEL_WIDTH
#define GUI_PANEL_MENU_HEIGHT 50
#define GUI_PANEL_MENU_BMP "./GUI/ChessGui/panelMenu.bmp"
#define GUI_PANEL_MENU_NUMOF_WIDGETS 4

#define GUI_PANEL_SUBPANELS_X GUI_PANEL_MENU_X
#define GUI_PANEL_SUBPANELS_Y (GUI_PANEL_MENU_Y+GUI_PANEL_MENU_HEIGHT)
#define GUI_PANEL_SUBPANELS_WIDTH GUI_MAIN_SIDE_PANEL_WIDTH
#define GUI_PANEL_SUBPANELS_HEIGHT (GUI_MAIN_SIDE_PANEL_HEIGHT-GUI_PANEL_MENU_HEIGHT)

#define GUI_PANEL_SETTINGS_BMP "./GUI/ChessGui/panelSettings.bmp"
#define GUI_PANEL_SETTINGS_NUMOF_WIDGETS 6

#define GUI_PANEL_LOADING_BMP "./GUI/ChessGui/panelLoading.bmp"
#define GUI_PANEL_LOADING_NUMOF_WIDGETS 2
#define GUI_PANEL_LOADING_NUMOF_FIXED_SLOTS 5


#define GUI_PANEL_GAMECMDS_BMP "./GUI/ChessGui/panelGameplay.bmp"
#define GUI_PANEL_GAMEPLAY_NUMOF_WIDGETS 4





// = LAYOUT CONFIGS CREATORS


// === MAIN WINDOW
Widget* createMainChessWindow();

void createPanelsForMainWindow(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets);

// ..	functions
void switchToSubPanelAtIdx(int idx);




// === MENU PANEL
void createWidgetsForMenuPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets);

void btnHandlerActivateSettingsPanel(Widget* widget, BtnEvent* btnEvent);
void btnHandlerActivateLoadingPanel(Widget* widget, BtnEvent* btnEvent);
void btnHandlerActivateGameplayPanel(Widget* widget, BtnEvent* btnEvent);
void btnHandlerExitProgram(Widget* widget, BtnEvent* btnEvent);


// === SETTINGS PANEL
void createWidgetsForSettingsPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets);

void createWidgetsForSettingsPlayersPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets);
void createWidgetsForSettingsDifficultyPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets);
void createWidgetsForSettingsColorPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets);

void btnSettingsStartNewGame(Widget* widget, BtnEvent* btnEvent);
void btnHandlerPlayerSelect(Widget* widget, BtnEvent* btnEvent);
void btnHandlerDifficultySelect(Widget* widget, BtnEvent* btnEvent);
void btnHandlerColorSelect(Widget* widget, BtnEvent* btnEvent);



// === LOADING PANEL
void createWidgetsForLoadingPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets);

void createWidgetsForLoadingSlotsPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets);


void btnHandlerLoadGame(Widget* widget, BtnEvent* btnEvent);
void btnHandlerLoadingSlots(Widget* widget, BtnEvent* btnEvent);

// ..	functions
void updateSlotSelectFromWidget(Widget* widget);
void setNumOfActiveSlots(int num);




// === GAMEPLAY PANEL

void createWidgetsForGameplayPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets);

void btnGameplayRestart(Widget* widget, BtnEvent* btnEvent);
void btnGameplaySave(Widget* widget, BtnEvent* btnEvent);
void btnGameplayUndo(Widget* widget, BtnEvent* btnEvent);



// === GAMEBOARD PANEL

void createCellsForGameboardPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets);

void handlerForChessCell (Widget* widget, BtnEvent* btnEvent);
void handlerForHoverCell (Widget* widget, BtnEvent* btnEvent);

void doDragDropMove(Widget* dragSourceCell, Widget* dragTargetCell);





// HELPER FUNCTIONS

void handleGameBehaveAtEndTurn();



void populateBoardAccordingToGame();


int getBtnIndexInPanel(Widget* widget);

int getBtnIndexSelectedInPanel(Widget* widget);

void popMsgGameStatus();

bool getUserConfirmedToProceedGameNotSaved(); //TODO

CellSprite getCellSpriteFromEnum(Symbol nameId);



#endif /* CHESSGAMEGUICONFIG_H_ */
