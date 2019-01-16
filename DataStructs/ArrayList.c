#include "ArrayList.h"





// === List Structure Handlers



ArrayList* ArrayListCreate(int maxNumElems, int elemSize){
	if (maxNumElems<=0) return NULL;
    if (elemSize<=0) return NULL;

	ArrayList* list = (ArrayList*) malloc(sizeof(ArrayList));
	if (list == NULL) {
		printErr(ERR_ALLOC_FAILED, __func__);
		return NULL;
	}

	list->elements = calloc((unsigned) maxNumElems, (unsigned) elemSize);
	if (list->elements == NULL){
		printErr(ERR_ALLOC_FAILED, __func__);
		free(list);
		return NULL;
	}

    // printf("created list :%p\n",(void*) &list);
    list->elemSize = (unsigned) elemSize;
	list->maxNumElems = (unsigned) maxNumElems;
	list->actualNumElems = 0;
	return list;
}



ArrayList* ArrayListCopy(ArrayList* list){
	if(list==NULL) return NULL;

    // Allocate copy list
    ArrayList* cpy = ArrayListCreate(list->maxNumElems, list->elemSize);
	if (cpy==NULL) return NULL;

    // Copy elements
    memcpy(cpy->elements, list->elements, list->elemSize*list->maxNumElems); // Copy elements

    // Copy fields
    cpy->actualNumElems = list->actualNumElems;

    return cpy;
}



void ArrayListDestroy(ArrayList* list){
	if (list==NULL) return;
	free(list->elements);
	free(list);
}



ARRAY_LIST_REPORT ArrayListClear(ArrayList* list){
	if (list==NULL) return ARRAY_LIST_INVALID_ARGUMENT;
	list->actualNumElems = 0;
	return ARRAY_LIST_SUCCESS;

}






// === Add Elements


ARRAY_LIST_REPORT ArrayListAddAt(ArrayList* list, const void* elemPtr, int index){
	if (list==NULL || elemPtr==NULL) return ARRAY_LIST_INVALID_ARGUMENT;
	if (list->actualNumElems == list->maxNumElems)
		return ARRAY_LIST_FULL;
	if (index < 0 || (unsigned) index > list->maxNumElems-1 || (unsigned) index > list->actualNumElems)
		return ARRAY_LIST_INVALID_ARGUMENT;

    // Set insertion pos pointer
    // using char pointers (1-Byte length) because arithmetic pointers is forbidden in void pointers.
	char* cur = (char*) list->elements + index*list->elemSize;

	// Swapper Loop - insert new and copy forward all following array elements
    void* writeDataPtr = malloc(list->elemSize);
	memcpy(writeDataPtr, elemPtr, list->elemSize);
	void* tmpDataPtr = malloc(list->elemSize);



	for (unsigned i=(unsigned) index; i<1+list->actualNumElems; i++){
		memcpy(tmpDataPtr, cur, list->elemSize);            // Backup: current in tmp
		memcpy(cur, writeDataPtr, list->elemSize);          // Write: writeData to cur
		memcpy(writeDataPtr, tmpDataPtr, list->elemSize);   // Update: writeData from tmp
        cur += list->elemSize;                              // Progress: current pointer to next elem
	}

    // Update list's num of active elements
    list->actualNumElems++;

    // Free allocated
    free(tmpDataPtr);
	free(writeDataPtr);

	return ARRAY_LIST_SUCCESS;
}



ARRAY_LIST_REPORT ArrayListAddFirst(ArrayList* list, const void* elem){
	return ArrayListAddAt(list, elem, 0);
}


ARRAY_LIST_REPORT ArrayListAddLast(ArrayList* list, const void* elem){
    if (list == NULL) return ARRAY_LIST_INVALID_ARGUMENT;
	return ArrayListAddAt(list, elem, list->actualNumElems);
}






// === Remove Elements


ARRAY_LIST_REPORT ArrayListRemoveAt(ArrayList* list, int index){
	if (list==NULL) return ARRAY_LIST_INVALID_ARGUMENT;
	if(list->actualNumElems==0)
		return (ARRAY_LIST_EMPTY);
	if(index < 0 || (unsigned) index > list->actualNumElems-1)
		return ARRAY_LIST_INVALID_ARGUMENT;

    // Set removal pos pointer
    // using char pointers (1-Byte length) because arithmetic pointers is forbidden in void pointers.
    char* cur  = (char*) list->elements + index*list->elemSize;

	// Swapper Loop -  cur=next
	for(unsigned i=(unsigned) index; i<list->actualNumElems-1; i++){ // loop until one before last
		memcpy(cur, cur+list->elemSize, list->elemSize);
		cur += list->elemSize;
	}

	// Delete last elem (by marking actual):
	list->actualNumElems--;

	return ARRAY_LIST_SUCCESS;

}

ARRAY_LIST_REPORT ArrayListRemoveFirst(ArrayList* list){
	return ArrayListRemoveAt(list, 0);
}


ARRAY_LIST_REPORT ArrayListRemoveLast(ArrayList* list){
    if (list == NULL) return ARRAY_LIST_INVALID_ARGUMENT;
    return ArrayListRemoveAt(list, list->actualNumElems-1);
}


void* ArrayListGetAt(ArrayList* list, int index){
	if(list==NULL || index < 0 || (unsigned) index > list->actualNumElems-1) return NULL;
    // using char pointers (1-Byte length) because arithmetic pointers is forbidden in void pointers.
    char* ptr = (char*) list->elements + index*list->elemSize;
	return ptr;
}

void* ArrayListGetFirst(ArrayList* list){
	return ArrayListGetAt(list, 0);
}

void* ArrayListGetLast(ArrayList* list){
    if (list == NULL) return NULL;
    return ArrayListGetAt(list, list->actualNumElems-1);
}




// === Get List Properties


int ArrayListMaxCapacity(ArrayList* list){
	if (list==NULL) return -1;
	return list->maxNumElems;
}

int ArrayListSize(ArrayList* list){
	if (list==NULL) return -1;
	return list->actualNumElems;
}


bool ArrayListIsFull(ArrayList* list){
	if (list==NULL) return -1;
	return (list->actualNumElems == list->maxNumElems);
}

bool ArrayListIsEmpty(ArrayList* list){
	if(list==NULL) return false;
	if(list->actualNumElems==0) return true;
	return false;
}


