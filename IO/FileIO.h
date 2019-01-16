



/**
 * HANDLES ALL FILE I\O SERVICES
 */



#ifndef CHESSPROG_FILEIO_H
#define CHESSPROG_FILEIO_H


#include "Parser.h"
#include "../ChessGame/ChessGameBoard.h"
#include "../ChessGame/ChessGameViewOutput.h"

#define T_FIRST_XML_LINE "?xml version=\"1.0\" encoding=\"UTF-8\"?"
#define T_GAME "game"
#define T_TURN "current_turn"
#define T_BOARD "board"
#define MAX_TAG 24
#define NUM_OF_SLOTS 5


/**
 * load save in filePath to game
 * assume correction of the file format
 * return if the filed opened correctly
 */
bool loadFile(ChessGame* game, const char filePath[]);


/**
 * save current game to new file in filePath
 * return if the filed opened correctly
 */
bool saveFile(const ChessGame* game, const char filePath[]);



// ============== FUNC FOR GUI ==================

/**
 * load saving from given slot
 */
bool loadFileFromSlot(ChessGame* game, int slotNum);

/**
 * save game to relevant slot
 */
bool saveFileToSlot(ChessGame* game);

/**
 * return validSlot
 */
int getValidSlotsNum();

/**
 * load validSlot and Recent Slot from dedicated file
 */
void initializeValidAndRecentSlot();

/**
 * save validSlot and Recent Slot to dedicated file
 */
void saveValidAndRecentSlot();











#endif //CHESSPROG_FILEIO_H
