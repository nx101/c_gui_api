
#include "main.h"



int main(int argc, char** argv) {
	if (argc || argv) {};

    ChessGame* game = createGame();
    if (game == NULL) return 0;


	if ((argc == 2 && strcmp(argv[1],"-g") == 0)) { // Cmd line "-g"

		// run in gui mode
		guiMainLoop(game);
	}

	else if (argc == 1 || // No args - default mode
			(argc == 2 && strcmp(argv[1],"-c") == 0)){ // Cmd line "-c"

		// run in console mode
		startGameAtModeConsole(game);

	}

	destroyGame(game);

	return 1;
}


void startGameAtModeConsole(ChessGame* game) {

    COMMAND cmd = NO_CMD;

    while (cmd != COMMAND_QUIT) {
        initializeGame(game);

         cmd = doSettings(game);
        if (cmd == COMMAND_QUIT)  {
            //destroyGame(game); // cancel to prevent Segmentation fault when we destroy again the game in line 27
            return;
        }


        // cmd == COMMAND_START
        game->gameState = GAME_STATE_ON;


        Color userColor = game->settings.user1Color;
        while (game->gameState == GAME_STATE_ON && cmd != COMMAND_QUIT && cmd != COMMAND_RESET) {
            if (game->settings.gameModeUsersNum == PLAYER_VS_COMP) {
                if (userColor == game->currPlayer) { // user turn
                    cmd = doPlayerTurn(game);
                }
                else if (game->gameState == GAME_STATE_ON){      // comp turn
                    doCompTurn(game, true);
                }
            }
            else {  // PLAYER_VS_PLAYER
                cmd = doPlayerTurn(game);
            }
        }
        if (game->gameState != GAME_STATE_ON) break;
    }

    return;
}
