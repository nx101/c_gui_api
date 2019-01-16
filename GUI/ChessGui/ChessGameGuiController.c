
#include "ChessGameGuiController.h"





// === LAYOUT CONFIGS CREATORS


// === MENU WINDOW

static ChessGame* game = NULL;
static Widget* window = NULL;
static Widget** windowPanels = NULL;
static Widget* menuPanel = NULL;
static Widget* newBtn = NULL;
static Widget* boardPanel = NULL;
static Widget** chessBoardWidgets = NULL;
static bool isSaveDirty = false;
static Widget* undoBtn = NULL;



Widget* createMainChessWindow(ChessGame* chessGame){
	game = chessGame;
	return window = createWindow(GUI_MAIN_WIN_WIDTH, GUI_MAIN_WIN_HEIGHT,
			GUI_MAIN_WINDOW_TITLE,
			GUI_MAIN_WINDOW_NUMOF_PANELS, createPanelsForMainWindow);
}


void createPanelsForMainWindow(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets) {
	SDL_Rect target;

	if (targetRect) {}

	if (numOfWidgets<5) return; // Literal check against widgets defined here

	windowPanels = widgets;

	// Panel : Menu
	target = guiGetAsRect(GUI_PANEL_MENU_X,GUI_PANEL_MENU_Y,
			GUI_PANEL_MENU_WIDTH,GUI_PANEL_MENU_HEIGHT);
	menuPanel = widgets[0] =
			createPanel(renderer, &target,
					PANEL_DISP_SPRITE_ONLY, GUI_PANEL_MENU_BMP,
					GUI_PANEL_MENU_NUMOF_WIDGETS,
					createWidgetsForMenuPanel);


	// Panel : Settings
	target = guiGetAsRect(GUI_PANEL_SUBPANELS_X,GUI_PANEL_SUBPANELS_Y,
			GUI_PANEL_SUBPANELS_WIDTH,GUI_PANEL_SUBPANELS_HEIGHT);
	widgets[1] =
			createPanel(renderer, &target,
					PANEL_DISP_SPRITE_ONLY, GUI_PANEL_SETTINGS_BMP,
					GUI_PANEL_SETTINGS_NUMOF_WIDGETS,
					createWidgetsForSettingsPanel);
	hideWidget(widgets[1], W_CMD_SAVE_AND_HIDE, W_CMD_NO_DRAW_NO_RENDER);

	//	 Panel : Load
	widgets[2] =
			createPanel(renderer, &target,
					PANEL_DISP_SPRITE_ONLY, GUI_PANEL_LOADING_BMP,
					GUI_PANEL_LOADING_NUMOF_WIDGETS,
					createWidgetsForLoadingPanel);
	//hideWidget(widgets[2], W_CMD_SAVE_AND_HIDE, W_CMD_NO_DRAW_NO_RENDER);


	//	 Panel : Gameplay
	widgets[3] =
			createPanel(renderer, &target,
					PANEL_DISP_SPRITE_ONLY, GUI_PANEL_GAMECMDS_BMP,
					GUI_PANEL_GAMEPLAY_NUMOF_WIDGETS,
					createWidgetsForGameplayPanel);
	hideWidget(widgets[3], W_CMD_SAVE_AND_HIDE, W_CMD_NO_DRAW_NO_RENDER);


	// Panel : Chess Board
	target = guiGetAsRect(GUI_MAIN_SIDE_PANEL_WIDTH, 0,
			GUI_BOARD_WIDTH, GUI_BOARD_HEIGHT);

	boardPanel = widgets[4] = createPanel(renderer, &target,
			PANEL_DISP_SPRITE_ONLY, GUI_BOARD_BMP,
			CHESS_CELLS_NUMOF_X*CHESS_CELLS_NUMOF_Y + 1,
			createCellsForGameboardPanel);

}


// ..	functions

void switchToSubPanelAtIdx(int idx){
	// Sub Panel indexes: 1 to 3
	if (idx < 1 || idx > 3) return;

	for (int i=1; i<=3; i++) {
		if (i != idx && !isHidden(windowPanels[i])) {
			hideWidget(windowPanels[i], W_CMD_SAVE_AND_HIDE, W_CMD_DRAW_AND_RENDER);
		}
	}

	hideWidget(windowPanels[idx], W_CMD_RESTORE, W_CMD_DRAW_AND_RENDER);

}



// ====================================


// === MENU PANEL

static Widget* gameplayBtn = NULL;


void createWidgetsForMenuPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets) {

	if (numOfWidgets<4) return;


	int originX = targetRect->x;
	int originY = 5 + targetRect->y;

	SDL_Rect target;

	// button : "NEW"
	target = guiGetAsRect(originX + 55, originY,
			BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	newBtn = widgets[0] = createButtonSimple1(renderer, &target, btnHandlerActivateSettingsPanel);

	// button : "LOAD"
	target = guiGetAsRect(originX + BTN_SPRITE_WIDTH + 105, originY,
			BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	widgets[1] = createButtonSimple1(renderer, &target, btnHandlerActivateLoadingPanel);

	// button : "GAMEPLAY"
	target = guiGetAsRect(originX + 2*BTN_SPRITE_WIDTH + 165, originY,
			BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	gameplayBtn = widgets[2] = createButtonSimple1(renderer, &target, btnHandlerActivateGameplayPanel);
	setBtnInactiveSameSelect(gameplayBtn, W_CMD_NO_DRAW_NO_RENDER);

	// button : "EXIT"
	target = guiGetAsRect(originX + 2*BTN_SPRITE_WIDTH + 335, originY,
			BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	widgets[3] = createButtonSimple1(
			renderer, &target, btnHandlerExitProgram);

}


// === Buttons Event Handlers

void btnHandlerActivateSettingsPanel(Widget* widget, BtnEvent* btnEvent){
	if (widget == NULL || btnEvent == NULL) return;
	if (*btnEvent == BTN_EVENT_ON_LEFT_CLICK_COMPLETED) {
		switchToSubPanelAtIdx(1);
	}

}

void btnHandlerActivateLoadingPanel(Widget* widget, BtnEvent* btnEvent){
	if (widget == NULL || btnEvent == NULL) return;
	if (*btnEvent == BTN_EVENT_ON_LEFT_CLICK_COMPLETED) {
		setNumOfActiveSlots(getValidSlotsNum());
		switchToSubPanelAtIdx(2);
	}
}


void btnHandlerActivateGameplayPanel(Widget* widget, BtnEvent* btnEvent){
	if (widget == NULL || btnEvent == NULL) return;
	if (*btnEvent == BTN_EVENT_ON_LEFT_CLICK_COMPLETED) {
		setNumOfActiveSlots(getValidSlotsNum());
		switchToSubPanelAtIdx(3);
	}
}


void btnHandlerExitProgram(Widget* widget, BtnEvent* btnEvent){
	if (widget == NULL || btnEvent == NULL) return;
	if (*btnEvent == BTN_EVENT_ON_LEFT_CLICK_COMPLETED) {
		SDL_Event event;
		event.type = SDL_QUIT;
		SDL_PushEvent(&event);
	}
}



// ..	functions



// ====================================






// === SETTINGS PANEL


static Widget* settingsStartBtn = NULL;
static Widget* settingsDifficulyPanel = NULL;
static Widget* settingsPlayersPanel = NULL;
static Widget* settingsColorPanel = NULL;


void createWidgetsForSettingsPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets) {

	if (numOfWidgets<4) return;

	int x = targetRect->x;
	int y = targetRect->y;
	SDL_Rect target;
	int cnt = 0;


	// button : "STARTGAME"
	target = guiGetAsRect(x+10+50*6, y+5+50*3,
			BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	settingsStartBtn = widgets[cnt] = createButtonSimple1(renderer, &target, btnSettingsStartNewGame);
	//setBtnUserData(widgets[cnt], 20);
	cnt++;

	// Sub panel : players
	target = guiGetAsRect(x+50*1, y+50*3, 100, 50*2);
	settingsPlayersPanel = widgets[cnt] = createPanel(renderer, &target, PANEL_DISP_EMPTY, NULL,
			2, createWidgetsForSettingsPlayersPanel);
	cnt++;

	// Sub panel : difficulty
	target = guiGetAsRect(x+50*1, y+50*7, 100, 50*5);
	settingsDifficulyPanel = widgets[cnt] = createPanel(renderer, &target, PANEL_DISP_EMPTY, NULL,
			4, createWidgetsForSettingsDifficultyPanel);
	cnt++;


	// Sub panel : color
	target = guiGetAsRect(x+50*6, y+50*7, 100, 50*2);
	settingsColorPanel = widgets[cnt] = createPanel(renderer, &target, PANEL_DISP_EMPTY, NULL,
			2, createWidgetsForSettingsColorPanel);
	cnt++;

}


// === SETTINGS PANEL - Buttons Event Handlers

// buttons : "1 Player" , "2 Players"
void createWidgetsForSettingsPlayersPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets) {
	if (numOfWidgets<2) return;
	SDL_Rect target = guiGetAsRect(targetRect->x+10, targetRect->y+5, BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	for (int i=0; i<2; i++){
		widgets[i] = createAffecterSimple1(renderer, &target, btnHandlerPlayerSelect,
				widgets, numOfWidgets, affectWidgetsBtnMultiChoiceAlwaysOneSelected);
		target.y +=50;
	}
	setBtnState(widgets[0], BTN_NOT_ACTIVE_SELECTED, W_CMD_NO_DRAW_NO_RENDER);
}

// buttons : "Difficulty 1 to 5"
void createWidgetsForSettingsDifficultyPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets) {
	if (numOfWidgets<4) return;
	SDL_Rect target = guiGetAsRect(targetRect->x+10, targetRect->y+5, BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	for (int i=0; i<4; i++){
		widgets[i] = createAffecterSimple1(renderer, &target, btnHandlerDifficultySelect,
				widgets, numOfWidgets, affectWidgetsBtnMultiChoiceAlwaysOneSelected);
		target.y +=50;
	}
	setBtnState(widgets[1], BTN_NOT_ACTIVE_SELECTED, W_CMD_NO_DRAW_NO_RENDER);
}

// buttons : "BLACK" , "WHITE"
void createWidgetsForSettingsColorPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets) {
	if (numOfWidgets<2) return;
	SDL_Rect target = guiGetAsRect(targetRect->x+10, targetRect->y+5, BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	for (int i=0; i<2; i++){
		widgets[i] = createAffecterSimple1(renderer, &target, btnHandlerColorSelect,
				widgets, numOfWidgets, affectWidgetsBtnMultiChoiceAlwaysOneSelected);
		target.y +=50;
	}
	setBtnState(widgets[1], BTN_NOT_ACTIVE_SELECTED, W_CMD_NO_DRAW_NO_RENDER);
}



void btnSettingsStartNewGame(Widget* widget, BtnEvent* btnEvent){

	assertAndPrintFuncErr(widget != NULL && btnEvent != NULL, __func__);

	if (*btnEvent == BTN_EVENT_ON_LEFT_CLICK_COMPLETED) {

		if (isSaveDirty)
			if (!getUserConfirmedToProceedGameNotSaved()) return;

		initializeGame(game);

		int playerNum = getBtnIndexSelectedInPanel(settingsPlayersPanel);
		assertAndPrintFuncErr(playerNum == 0 || playerNum == 1, __func__);
		game->settings.gameModeUsersNum = playerNum + 1;

		if (game->settings.gameModeUsersNum == PLAYER_VS_COMP) {
			game->settings.difLevel =
					getBtnIndexSelectedInPanel(settingsDifficulyPanel) + 1;
			game->settings.user1Color =
					(Color) getBtnIndexSelectedInPanel(settingsColorPanel);
		}

		game->gameState = GAME_STATE_ON;

		populateBoardAccordingToGame();

		setBtnNormalActiveUnselected(gameplayBtn, W_CMD_DRAW_AND_RENDER);

		switchToSubPanelAtIdx(3);
	}
}


void btnHandlerPlayerSelect(Widget* widget, BtnEvent* btnEvent){
	if (widget || btnEvent) {};
}


void btnHandlerDifficultySelect(Widget* widget, BtnEvent* btnEvent){
	if (widget || btnEvent) {};
}


void btnHandlerColorSelect(Widget* widget, BtnEvent* btnEvent){
	if (widget || btnEvent) {};
}




// ..	functions



// ====================================





// === LOADING PANEL

static Widget* loadBtn = NULL;
static Widget* slotsPanel = NULL;
static Widget** slotsBtns = NULL;
static int slotselect = -1;
static int numOfActiveSlots = 0;


void createWidgetsForLoadingPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets) {

	if (renderer || targetRect || widgets || numOfWidgets) {}

	if (numOfWidgets < GUI_PANEL_LOADING_NUMOF_WIDGETS) return;

	int x = targetRect->x;
	int y = targetRect->y;
	SDL_Rect target;
	int cnt = 0;
	// button : "LOADGAME"
	target = guiGetAsRect(x+10+50*6, y+5+50*3,
			BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	loadBtn = widgets[cnt] = createButtonSimple1(renderer, &target, btnHandlerLoadGame);
	cnt++;
	setBtnState(widgets[0], BTN_NOT_ACTIVE_SELECTED, W_CMD_NO_DRAW_NO_RENDER);


	// set inactive if no slot selected available

	// Sub panel : saved slots
	target = guiGetAsRect(x+50*1, y+50*3, 100, 50*GUI_PANEL_LOADING_NUMOF_FIXED_SLOTS);
	slotsPanel = widgets[cnt] = createPanel(renderer, &target, PANEL_DISP_EMPTY, NULL,
			GUI_PANEL_LOADING_NUMOF_FIXED_SLOTS, createWidgetsForLoadingSlotsPanel);
	cnt++;

	setNumOfActiveSlots(getValidSlotsNum());


}


// buttons : "Load Slots 1 to 5"
void createWidgetsForLoadingSlotsPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets) {

	if (numOfWidgets < GUI_PANEL_LOADING_NUMOF_FIXED_SLOTS) return;

	slotsBtns = widgets;

	SDL_Rect target = guiGetAsRect(targetRect->x+10, targetRect->y+5, BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	for (int i=0; i<GUI_PANEL_LOADING_NUMOF_FIXED_SLOTS; i++){
		widgets[i] = createAffecterSimple1(renderer, &target, btnHandlerLoadingSlots,
				widgets, numOfWidgets, affectWidgetsSingleMultiChoice);
		target.y +=50;
	}
}


void btnHandlerLoadGame(Widget* widget, BtnEvent* btnEvent){
	if (*btnEvent == BTN_EVENT_ON_LEFT_CLICK_COMPLETED) {

		if (isSaveDirty) {
			if (!getUserConfirmedToProceedGameNotSaved()) return;
		}
		// collect config and get new game
		int loadIndex = getBtnIndexInPanel(widget);
		loadFileFromSlot(game, loadIndex);

		game->gameState = GAME_STATE_ON;
		isSaveDirty = false;

		populateBoardAccordingToGame();
		switchToSubPanelAtIdx(3);

        if (!canUndo(game))
			setBtnInactiveSameSelect(undoBtn, W_CMD_DRAW_AND_RENDER);
	}
}


void btnHandlerLoadingSlots(Widget* widget, BtnEvent* btnEvent){
	if (*btnEvent == BTN_EVENT_ON_LEFT_CLICK_COMPLETED) {
		if (isBtnSelected(widget)) {
			setBtnState(loadBtn, BTN_NORMAL_UNSELECTED, W_CMD_DRAW_AND_RENDER);
		}
		else {
			slotselect = -1;
			setBtnState(loadBtn, BTN_NOT_ACTIVE_UNSELECTED, W_CMD_DRAW_AND_RENDER);
		}

	}

}


// ..	functions



void setNumOfActiveSlots(int num){
	numOfActiveSlots = num;
	for (int i=0; i<GUI_PANEL_LOADING_NUMOF_FIXED_SLOTS; i++){
		if (i<num) {
			setBtnState(slotsBtns[i], BTN_NORMAL_UNSELECTED, W_CMD_NO_DRAW_NO_RENDER);
			hideWidget(slotsBtns[i], W_CMD_SET_VISIBLE, W_CMD_NO_DRAW_NO_RENDER);
		}
		else {
			setBtnState(slotsBtns[i], BTN_NOT_ACTIVE_UNSELECTED, W_CMD_NO_DRAW_NO_RENDER);
			hideWidget(slotsBtns[i], W_CMD_SET_HIDDEN, W_CMD_NO_DRAW_NO_RENDER);
		}
	}
}






// ====================================




// === GAMEPLAY PANEL

void createWidgetsForGameplayPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets) {

	if (renderer || targetRect || widgets || numOfWidgets) {};

	if (numOfWidgets<GUI_PANEL_GAMEPLAY_NUMOF_WIDGETS) return;

	SDL_Rect target = guiGetAsRect(targetRect->x+50*1, targetRect->y+50*3,
			BTN_SPRITE_WIDTH, BTN_SPRITE_HEIGHT);
	target.x += 10;
	target.y += 5;

	if (numOfWidgets < 4) return;

	// slotsBtns = widgets;

	// restart
	widgets[0] = createButtonSimple1(renderer, &target, btnGameplayRestart);
	target.y +=50;

	//save
	widgets[1] = createButtonSimple1(renderer, &target, btnGameplaySave);
	target.y +=50;

	// undo
	undoBtn = widgets[2] = createButtonSimple1(renderer, &target, btnGameplayUndo);
	target.y +=50;
	setBtnInactiveSameSelect(widgets[2], W_CMD_NO_DRAW_NO_RENDER); // can't undo at start

}


void btnGameplayRestart(Widget* widget, BtnEvent* btnEvent){
	btnSettingsStartNewGame(widget, btnEvent);
}

void btnGameplaySave(Widget* widget, BtnEvent* btnEvent){
	if (widget  == NULL || btnEvent == NULL) {printErr(ERR_FUNC_FAILED, __func__); return;}

	if (*btnEvent == BTN_EVENT_ON_LEFT_CLICK_COMPLETED) {
		saveFileToSlot(game);
		isSaveDirty = false;
	}
}

void btnGameplayUndo(Widget* widget, BtnEvent* btnEvent){
	if (widget  == NULL || btnEvent == NULL) {printErr(ERR_FUNC_FAILED, __func__); return;}

	if (*btnEvent == BTN_EVENT_ON_LEFT_CLICK_COMPLETED) {
		undo(game);
		isSaveDirty = true;
		populateBoardAccordingToGame();
		if (!canUndo(game))
			setBtnInactiveSameSelect(widget, W_CMD_DRAW_AND_RENDER);
	}
}



// ====================================






// === GAMEBOARD PANEL

static Widget* hoverCell = NULL;

void createCellsForGameboardPanel(SDL_Renderer* renderer, const SDL_Rect* targetRect,
		Widget** widgets, int numOfWidgets){

	if (renderer || targetRect || widgets || numOfWidgets) {};

	if (numOfWidgets < CHESS_CELLS_NUMOF_X * CHESS_CELLS_NUMOF_Y + 1) {
		widgets = NULL;
		return;
	}


	chessBoardWidgets = widgets;


	int originX = targetRect->x;
	int originY = targetRect->y;
	SDL_Rect target;
	int cnt = 0;

	// == Normal Cells
	for (int i=0; i<CHESS_CELLS_NUMOF_X; i++){
		for (int j=CHESS_CELLS_NUMOF_Y-1; j>=0; j--) {

			target = guiGetAsRect(
					originX + GUI_CELL_WIDTH_OR_HEIGHT*i,
					originY + GUI_CELL_WIDTH_OR_HEIGHT*j,
					GUI_CELL_WIDTH_OR_HEIGHT, GUI_CELL_WIDTH_OR_HEIGHT);

			widgets[cnt] = createCell(renderer, &target,
					GUI_CELL_BMP, handlerForChessCell);
			cnt++;
		}
	}


	// == HOVER CELL
	// for drag and drop utilization
	// Static hover cell pointer, accessible only for this entire file
	// Still Allocated and Destroyed by its Panel container
	target = guiGetAsRect(
			originX + GUI_CELL_WIDTH_OR_HEIGHT*5, originY + GUI_CELL_WIDTH_OR_HEIGHT*5,
			GUI_CELL_WIDTH_OR_HEIGHT, GUI_CELL_WIDTH_OR_HEIGHT);
	// Drag and Dropper
	hoverCell = widgets[cnt] = createCell(renderer, &target,
			GUI_CELL_BMP, handlerForChessCell);

	setupCellsDragdropHoverer(widgets[cnt], targetRect);


}


void handlerForChessCell (Widget* widget, BtnEvent* btnEvent){

	assertAndPrintFuncErr(widget != NULL, __func__);

	static Widget* dragSourceCell = NULL;
	static Widget* dragTargetCell = NULL;


	// Args Null check
	if (widget == NULL) {return;}
	//SDL_Delay(500);
	switch (*btnEvent) {

	// LEFT CLICK MODE - DRAG AND DROP
	case BTN_EVENT_ON_LEFT_PRESSED_DOWN :
		dragSourceCell = widget;
		BtnExt* btnExt = (BtnExt*) widget->ext;
		CellExt* cellExt = (CellExt*) btnExt->ext;
		setCellSpriteAndPieceColorAndRenderPresent(hoverCell, cellExt->curSprite, cellExt->curPieceColor);
		break;

	case BTN_EVENT_ON_LEFT_CLICK_COMPLETED :
		if (widget != dragSourceCell) {
			assertAndPrintFuncErr(dragSourceCell != NULL,"handlerForChessCell dragSourceCell");
			dragTargetCell = widget;
			doDragDropMove(dragSourceCell, dragTargetCell);
		}

		//dragSourceCell = dragTargetCell = NULL;
		break;

	case BTN_EVENT_ON_LEFT_CLICK_CANCELED :
		dragSourceCell = NULL;
		break;

		// RIGHT CLICK
	case BTN_EVENT_ON_RIGHT_CLICK_COMPLETED :
		break;

	case BTN_EVENT_ON_RIGHT_PRESSED_DOWN :
		break;

	default :
		break;

	}
}


void handlerForHoverCell (Widget* widget, BtnEvent* btnEvent){
	// Args Null check
	if (widget == NULL) {return;}
	switch (*btnEvent) {
	case BTN_EVENT_ON_LEFT_CLICK_COMPLETED :

		break;
	case BTN_EVENT_ON_LEFT_PRESSED_DOWN :

	default :
		break;
	}
}




void doDragDropMove(Widget* dragSourceCell, Widget* dragTargetCell) {

	//if (game Started) { //TODO

		int srcIndex = getBtnIndexInPanel(dragSourceCell);
		int targetIndex = getBtnIndexInPanel(dragTargetCell);
		Vector srcCoord = getCoordFromArrayPos(&game->gameBoard, srcIndex);
		Vector targetCoord = getCoordFromArrayPos(&game->gameBoard, targetIndex);
		// user turn
		bool isValidMove = moveAdvanced(game, &srcCoord, &targetCoord, false);
		if (isValidMove) {
			isSaveDirty = true;
			handleGameBehaveAtEndTurn();

			// comp turn;
			if (game->settings.gameModeUsersNum == PLAYER_VS_COMP) {
				doCompTurn(game, false);
				handleGameBehaveAtEndTurn();
			}
		}
}




void handleGameBehaveAtEndTurn(){
	if (canUndo(game))
		setBtnNormalActiveUnselected(undoBtn, W_CMD_DRAW_AND_RENDER);

	populateBoardAccordingToGame();

	popMsgGameStatus();
}


// ====================================














// HELPER FUNCTIONS

void populateBoardAccordingToGame() {
	for (int col=0; col<CHESS_CELLS_NUMOF_X; col++){
		for (int row=0; row<CHESS_CELLS_NUMOF_Y; row++) {
			Vector coord = {{col,row}};
			Cell* cell = cellInCoord(&coord, &(game->gameBoard));
			CellSprite cellSprite = CELL_SPRITE_EMPTY;
			CellPieceColor cellPieceColor = CELL_PIECE_NONE;
			if (!isCellEmpty(cell)) {
				Symbol symbol = (Symbol) cell->piece->type->nameId;
				cellSprite = getCellSpriteFromEnum(symbol);
				if (cell->piece->color == BLACK)
					cellPieceColor = CELL_PIECE_BLACK;
				else
					cellPieceColor = CELL_PIECE_WHITE;
			}
			setCellSpriteAndPieceColorAndRenderPresent(
					chessBoardWidgets[col*CHESS_CELLS_NUMOF_X+row],
					cellSprite, cellPieceColor);
		}
	}

}




int getBtnIndexInPanel(Widget* widget){
	if (widget == NULL || widget->parent == NULL) {printErr(ERR_FUNC_FAILED, __func__); return -1;}

	PanelExt* panelExt = (PanelExt*) widget->parent->ext;
	Widget** widgets =  panelExt->panelWidgets;
	int numOfWidgets = panelExt->numOfWidgets;

	for (int i=0; i<numOfWidgets; i++){
		if (widgets[i] == widget)
			return i;
	}
	return -1;
}


int getBtnIndexSelectedInPanel(Widget* widget) {
	if (widget == NULL || widget->parent == NULL) {printErr(ERR_FUNC_FAILED, __func__); return -1;}

	PanelExt* panelExt = (PanelExt*) widget->ext;
	Widget** widgets =  panelExt->panelWidgets;
	int numOfWidgets = panelExt->numOfWidgets;

	for (int i=0; i<numOfWidgets; i++){
		if (isBtnSelected(widgets[i]))
			return i;
	}
	return -1;

}


void popMsgGameStatus() {
	static const SDL_MessageBoxButtonData buttons[] = {
			//{ /* .flags, .buttonid, .text */        0, 0, "no" },
			//{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Cancel" },
			{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Ok Baby" },
	};
	static const SDL_MessageBoxColorScheme colorScheme = {
			{ /* .colors (.r, .g, .b) */
					/* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
					{118, 69, 56},
					/* [SDL_MESSAGEBOX_COLOR_TEXT] */
					{255, 249, 238},
					/* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
					{0, 255, 0},
					/* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
					{0, 0, 255},
					/* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
					{255, 0, 255}
			}
	};


	char str[128] = "";
	if (isGameFinished(game)) { //GameFinished
		switch (game->gameState) {
			case GAME_STATE_WHITE_WON:
				strcat(str, "Checkmate! white player wins the game");
				break;

			case GAME_STATE_BLACK_WON:
				strcat(str, "Checkmate! black player wins the game");
				break;

			case GAME_STATE_TIE:
				strcat(str, "The game is tied");
				break;

			default:
				printErr(ERR_FUNC_FAILED, __func__);
				break;
		}
	}
	else {
		if (isKingThreatend(game, WHITE))
			strcat(str, "Chess - white king is threatend!");
		else if (isKingThreatend(game, BLACK)) {
			strcat(str, "Chess - black king is threatend!");
		}

	}

	if (strcmp(str, "") != 0) {
		const SDL_MessageBoxData messageboxdata = {
				SDL_MESSAGEBOX_WARNING, /* .flags */
				((Window*)window->ext)->sdlWindow, /* .window */
				"Game Notification", /* .title */
				str, /* .message */
				SDL_arraysize(buttons), /* .numbuttons */
				buttons, /* .buttons */
				&colorScheme /* .colorScheme */
		};

		int buttonid;
		if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
			printErr(ERR_FUNC_FAILED, __func__);
		}

	}
}

bool getUserConfirmedToProceedGameNotSaved() {
	static const SDL_MessageBoxButtonData buttons[] = {
			//{ /* .flags, .buttonid, .text */        0, 0, "no" },
			{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Cancel" },
			{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Ok Baby" },
	};
	static const SDL_MessageBoxColorScheme colorScheme = {
			{ /* .colors (.r, .g, .b) */
					/* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
					{118, 69, 56},
					/* [SDL_MESSAGEBOX_COLOR_TEXT] */
					{255, 249, 238},
					/* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
					{0, 255, 0},
					/* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
					{0, 0, 255},
					/* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
					{255, 0, 255}
			}
	};
	const SDL_MessageBoxData messageboxdata = {
			SDL_MESSAGEBOX_WARNING, /* .flags */
			((Window*)window->ext)->sdlWindow, /* .window */
			"Game Changed", /* .title */
			"Are you sure you want to proceed?", /* .message */
			SDL_arraysize(buttons), /* .numbuttons */
			buttons, /* .buttons */
			&colorScheme /* .colorScheme */
	};
	int buttonid;
	if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
		printErr(ERR_FUNC_FAILED, __func__);
		return 1;
	}
	if (buttonid == -1) {
		printErr(ERR_FUNC_FAILED, __func__);
		return false;
	} else {
		return buttonid;
		//SDL_Log("selection was %s", buttons[buttonid].text);
	}

}


CellSprite getCellSpriteFromEnum(Symbol nameId) {
	switch (nameId) {
	case SYMBOL_PAWN:
		return CELL_SPRITE_PAWN;
	case SYMBOL_BISHOP:
		return CELL_SPRITE_BISHOP;
	case SYMBOL_ROOK:
		return CELL_SPRITE_ROOK;
	case SYMBOL_KNIGHT:
		return CELL_SPRITE_KNIGHT;
	case SYMBOL_QUEEN:
		return CELL_SPRITE_QUEEN;
	case SYMBOL_KING:
		return CELL_SPRITE_KING;
	default:
		printErr(ERR_FUNC_FAILED, __func__);
		return CELL_SPRITE_EMPTY;
	}
}


