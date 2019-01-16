
#ifndef GUIAPI_MEMGATEWAY_H_
#define GUIAPI_MEMGATEWAY_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "../IO/BasicErrorsHandling.h"

void* myMalloc(size_t size,
		const char* func, int line);

void* myCalloc(size_t numOfElements, size_t sizeOfElements,
		const char* func, int line);

void myFree(void* memory,
		const char* func, int line);

void myMemReportPrinter(
		const char* originFunc, int line,
		const char* memFunc, const void* ptr);



#endif /* GUIAPI_MEMGATEWAY_H_ */
