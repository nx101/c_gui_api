
#ifndef CHESSPROG_BASICERROSHANDLING_H
#define CHESSPROG_BASICERROSHANDLING_H

#include <stdio.h>
#include <stdbool.h>

/** ==1==  - INTERNAL ERROR MESSAGES (SPECIAL SUBSECTION of MAIN MSGS)


    * Note: Error Msgs can be used directly (printErr),
    * or indirectly by some main Msgs below (printMsg*).

    * All invalid commands are treated


    * SOME ERROR CASES:
    * Mem Alloc
    * File I/O
    * Safe Programming
    * SDL
    * Any Additional In-Library Faults
    *

    * On Error: Print to console, "ERROR:" prefix + msg
    * The program will disregard the fault and continue to run
    * Termination cases: no other course of action exists
    *


*/


#define ERR_PREFIX "ERROR: "

#define ASRT(x) assertAndPrintFuncErr((x), __func__,__LINE__);

typedef enum err_types_t {
    ERR_ALLOC_FAILED,
    ERR_FUNC_FAILED,
    ERR_FILE_CANT_OPEN,
    ERR_UNDO_CANT,
    ERR_INV_CMD,
    ERR_GAME_OVER,
} ERRTYPE;



/**
 * A concise Error Messages Printer for various cases
 * (Under normal conditions would be a module of its own)
 * @param err - The error type, ERRTYPE
 * @param func - an optional string to specify function name
 */
void printErr(ERRTYPE err, const char* func);

/**
 * assert condition.
 * in case of false, print error-function about given function.
 */
void assertAndPrintFuncErr(bool condition, const char *func);


#endif //CHESSPROG_BASICERROSHANDLING_H
