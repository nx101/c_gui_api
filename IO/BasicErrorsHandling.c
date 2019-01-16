#include "BasicErrorsHandling.h"

// ==1== ERROR PRINTING


void printErr(ERRTYPE err, const char *func) {

    switch (err) {

        case ERR_ALLOC_FAILED:
            printf(ERR_PREFIX);
            printf("Memory allocation failed");
            break;

        case ERR_FUNC_FAILED : //ARG
            printf(ERR_PREFIX);
            printf("%s", func);
            printf(" has failed\n");
            break;

        case ERR_FILE_CANT_OPEN : //ARG
            printf(ERR_PREFIX);
            printf("File doesn't exist or cannot be opened\n");
            break;


        case ERR_INV_CMD :
            printf(ERR_PREFIX);
            printf("invalid command\n");
            break;


        case ERR_UNDO_CANT :
            printf(ERR_PREFIX);
            printf("cannot undo previous move!\n");
            break;

        case ERR_GAME_OVER :
            printf(ERR_PREFIX);
            printf("the game is over\n");
            break;
    }

    fflush(stdout);

}


void assertAndPrintFuncErr(bool condition, const char *func) {
    if (!condition)
        printErr(ERR_FUNC_FAILED, func);
}