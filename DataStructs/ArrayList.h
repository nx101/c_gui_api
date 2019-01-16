#ifndef ARRAYLIST_H_
#define ARRAYLIST_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../IO/BasicErrorsHandling.h"


/**
 * ArrayList summary:
 *
 * A generic container that represents a linked list. The capcity of the list
 * and the size of elements are specified at the creation.
 * The container supports typical list functionalities with the addition of random access as in arrays.
 * Upon insertion, if the maximum capacity is reached then an error message is
 * returned and the list is not affected. A summary of the supported functions
 * is given below:
 *
 * ArrayListCreate       - Creates an empty array list with a specified
 *                           max capacity amd element size.
 * ArrayListCopy         - Creates an exact copy of a specified array list.
 * ArrayListDestroy      - Frees all memory resources associated with an array
 *                           list.
 * ArrayListClear        - Clears all elements from a specified array list.
 * ArrayListAddAt        - Inserts an element at a specified index, elements
 *                           will be shifted to make place.
 * ArrayListAddFirst     - Inserts an element at the beginning of the array
 *                           list, elements will be shifted to make place.
 * ArrayListAddLast      - Inserts an element at the end of the array list.
 * ArrayListRemoveAt     - Removes an element at the specified index, elements
 *                           elements will be shifted as a result.
 * ArrayListRemoveFirst  - Removes an element from the beginning of the array
 *                           list, elements will be shifted as a result.
 * ArrayListRemoveLast   - Removes an element from the end of the array list
 * ArrayListGetAt        - Accesses the element at the specified index.
 * ArrayListGetFirst     - Accesses the first element of the array list.
 * ArrayListGetLast      - Accesses the last element of the array list.
 * ArrayListMaxCapcity   - Returns the maximum capcity of the array list.
 * ArrayListSize         - Returns the number of elements in that array list.
 * ArrayListIsFull       - Returns if the array list reached its max capacity.
 * ArrayListIsEmpty      - Returns true if the array list contains no elements.
 */

typedef struct array_list_t {
	void* elements;             // Dynamically allocated Array of elements of type void*
	unsigned elemSize;          // The Size of a single element
	unsigned actualNumElems;    // Number of actual elements (valid entries in the array)
	unsigned maxNumElems;       // Number of maximum elements (for which mem was allocated)
} ArrayList;

/**
 * A type used for errors
 */
typedef enum t_array_list_report {
    ARRAY_LIST_SUCCESS,
	ARRAY_LIST_INVALID_ARGUMENT,
	ARRAY_LIST_FULL,
	ARRAY_LIST_EMPTY
} ARRAY_LIST_REPORT;



/**
 *  Creates an empty array list with the ecified maximum capacity.
 *  @param maxSize - the maximum capacity of the target array list.
 *  @param elemSize - size of each element in lise
 *  @return
 *  NULL, if an allocation error occurred or maxSize  <= 0.
 *  An instant of an array list otherwise.
 */
ArrayList* ArrayListCreate(int maxNumElems, int elemSize);


/**
 *	Creates an exact copy of the src array list. Elements in the new copy will
 *	be in the same order as they appeared in the source list.
 *	@param src - the source array list.
 *	@return
 *	NULL if either an allocation error occurs or src == NULL.
 *	A new copy of the source array list, otherwise.
 */
ArrayList* ArrayListCopy(ArrayList* list);

/**
 * Frees all memory resources associated with the source array list. If the
 * source array is NULL, then the function does nothing.
 * @param src - the source array list
 */
void ArrayListDestroy(ArrayList* list);

/**
 * Clears all elements from the source array list. After invoking this function,
 * the size of the source list will be reduced to zero and maximum capacity is
 * not affected.
 * @param src - the source array list
 * @return
 * _ARRAY_LIST_INVALID_ARGUMENT if src == NULL
 * _ARRAY_LIST_SUCCESS otherwise
 */
ARRAY_LIST_REPORT ArrayListClear(ArrayList* list);


// Add Elements
/**
 * Inserts element at a ecified index. The elements residing at and after the
 * ecified index will be shifted to make place for the new element. If the
 * array list reached its maximum capacity and error message is returned and
 * the source list is not affected
 * @param src   - the source array list
 * @param elem  - pointer to the new element to be inserted
 * @param index - the index where the new element will be placed. The index is
 *                0-based.
 * @return
 * _ARRAY_LIST_INVALID_ARGUMENT - if src == NULL or the index is out of bound
 * _ARRAY_LIST_FULL - if the source array list reached its maximum capacity
 * _ARRAY_LIST_SUCCESS - otherwise
 */
ARRAY_LIST_REPORT ArrayListAddAt(ArrayList* list, const void* elem, int index);

/**
 * Inserts element at a the beginning of the source element. The elements
 * will be shifted to make place for the new element. If the
 * array list reached its maximum capacity and error message is returned and
 * the source list is not affected
 * @param src   - the source array list
 * @param elem  - pointer to the new element to be inserted
 * @return
 * _ARRAY_LIST_INVALID_ARGUMENT - if src == NULL or the index is out of bound
 * _ARRAY_LIST_FULL - if the source array list reached its maximum capacity
 * _ARRAY_LIST_SUCCESS - otherwise
 */
ARRAY_LIST_REPORT ArrayListAddFirst(ArrayList* list, const void* elem);


/**
 * Inserts element at a the end of the source element. If the array list
 * reached its maximum capacity and error message is returned and the source
 * list is not affected.
 * @param src   - the source array list
 * @param elem  - pointer to the new element to be inserted
 * @return
 * _ARRAY_LIST_INVALID_ARGUMENT - if src == NULL or the index is out of bound
 * _ARRAY_LIST_FULL - if the source array list reached its maximum capacity
 * _ARRAY_LIST_SUCCESS - otherwise
 */
ARRAY_LIST_REPORT ArrayListAddLast(ArrayList* list, const void* elem);


// Remove Elements

/**
 * Removes an element from a specified index. The elements residing after the
 * specified index will be shifted to make to keep the list continuous. If the
 * array list is empty then an error message is returned and the source list
 * is not affected
 * @param src   - The source array list
 * @param index - The index from where the element will be removed.
 *                The index is 0-based.
 * @return
 * _ARRAY_LIST_INVALID_ARGUMENT - if src == NULL or the index is out of bound
 * _ARRAY_LIST_EMPTY - if the source array list is empty
 * _ARRAY_LIST_SUCCESS - otherwise
 */
ARRAY_LIST_REPORT ArrayListRemoveAt(ArrayList* list, int index);

/**
 * Removes an element from a the beginning of the list.
 * The elements will be shifted to make to keep the list continuous. If the
 * array list is empty then an error message is returned and the source list
 * is not affected
 * @param src   - The source array list
 * @param elem  - The new element to be inserted
 * @return
 * _ARRAY_LIST_INVALID_ARGUMENT - if src == NULL
 * _ARRAY_LIST_EMPTY - if the source array list is empty
 * _ARRAY_LIST_SUCCESS - otherwise
 */
ARRAY_LIST_REPORT ArrayListRemoveFirst(ArrayList* list);

/**
 * Removes an element from a the end of the list.
 * The elements will be shifted to make to keep the list continuous. If the
 * array list is empty then an error message is returned and the source list
 * is not affected
 * @param src   - The source array list
 * @param elem  - The new element to be inserted
 * @return
 * _ARRAY_LIST_INVALID_ARGUMENT - if src == NULL
 * _ARRAY_LIST_EMPTY - if the source array list is empty
 * _ARRAY_LIST_SUCCESS - otherwise.
 */
ARRAY_LIST_REPORT ArrayListRemoveLast(ArrayList* list);

// Get Elements

/**
 * Returns the element at the specified index. The function is called
 * with the assertion that all arguments are valid. If any of the arguments is
 * invalid then an undefined value is returned.
 * @param src - the source array list
 * @param index - the specified index, the index is 0-based.
 * @return
 * Undefined value if either src == NULL or index out of bound.
 * Otherwise, the element at the specified index is returned.
 */
void* ArrayListGetAt(ArrayList* list, int index);

/**
 * Returns the element at the beginning of the list. The function is called
 * with the assertion that all arguments are valid. If any of the arguments is
 * invalid then an undefined value is returned.
 * @param src - the source array list
 * @return
 * Undefined value if either src == NULL or the list is empty
 * Otherwise, the element at the beginning of the list is returned.
 */
void* ArrayListGetFirst(ArrayList* list);

/**
 * Returns the element at the end of the list. The function is called
 * with the assertion that all arguments are valid. If any of the arguments is
 * invalid then an undefined value is returned.
 * @param src - the source array list
 * @return
 * Undefined value if either src == NULL or the list is empty
 * Otherwise, the element at the end of the list is returned.
 */
void* ArrayListGetLast(ArrayList* list);


// Get List Properties

/**
 * Returns the maximum capacity of the list. The function is called
 * with the assertion that all arguments are valid. If any of the arguments is
 * invalid then an undefined value is returned.
 * @param src - the source array list
 * @return
 * Undefined value if either src == NULL
 * Otherwise, the maximum capacity of the list is returned.
 */
int ArrayListMaxCapacity(ArrayList* list);

/**
 * Returns the number of elements in the list. The function is called
 * with the assertion that all arguments are valid. If any of the arguments is
 * invalid then an undefined value is returned.
 * @param src - the source array list
 * @return
 * Undefined value if either src == NULL
 * Otherwise, the number of the elements in the list is returned.
 */
int ArrayListSize(ArrayList* list);

/**
 * Returns true if the list is full, that is the number of elements in the list
 * equals its maximum capacity.
 * @param src - the source array list
 * @return
 * false if either src == NULL or the number of elements in the list is less
 * than its maximum capacity.
 * Otherwise, true is returned.
 */
bool ArrayListIsFull(ArrayList* list);

/**
 * Returns true if the list is empty, that is the number of elements in the list
 * equals to zero.
 * @param src - the source array list
 * @return
 * false if either src == NULL or the number of elements in the list is not zero.
 * Otherwise, true is returned.
 */
bool ArrayListIsEmpty(ArrayList* list);



#endif
