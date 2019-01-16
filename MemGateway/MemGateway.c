#include "MemGateway.h"



void* myMalloc(size_t size,
		const char* func, int line) {

	void* res = malloc(size);
	if (res == NULL) printErr(ERR_ALLOC_FAILED, __func__);

	myMemReportPrinter(func, line, __func__, res);

	return res;

}


void* myCalloc(size_t numOfElements, size_t sizeOfElements,
		const char* func, int line) {

	void* res = calloc(numOfElements, sizeOfElements);
	if (res == NULL) printErr(ERR_ALLOC_FAILED, __func__);

	myMemReportPrinter(func, line, __func__, res);


	return res;
}


void myFree(void* memory,
		const char* func, int line) {

	myMemReportPrinter(func, line, __func__, memory);

	free (memory);

	return;
}


void myMemReportPrinter(const char* originFunc, int line,
		const char* memFunc, const void* ptr){
	//FILE* file = fopen("MemGatewayReport.txt", "w"); // Overwrite or create new

	if (false) {
		fprintf(stderr, "=========START**%s**%s**============\n\n\n", __DATE__, __TIME__);
		fprintf(stderr, "originfunc\tline\tmemfunc\taddress\n\n");
		fprintf(stderr, "%s\t%d\t%s\t%p\n", originFunc, line, memFunc, ptr);
	}

	fflush(stderr);

}


