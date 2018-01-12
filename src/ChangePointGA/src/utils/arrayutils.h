/*
 ============================================================================
 Name        : arrayutils.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : Array utils functions
 ============================================================================
 */

#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

#include <string.h>

static void addElement(void* array, int index, int size, size_t elemSize){
	char* dst = (char*)array;
	memmove(&dst[(index + 1) * elemSize], &dst[index * elemSize], (size - index) * elemSize);
}

static void removeElement(void* array, int index, int size, size_t elemSize){
	char* dst = (char*)array;
	memmove(&dst[index * elemSize], &dst[(index + 1) * elemSize], (size - 1 - index) * elemSize);
}

static void mergeElement(const void* dst,
		const void* src1, const void* src2,
		int startIndex1, int size1,
		int startIndex2, int size2,
		size_t elemSize){

	char* dstc = (char*)dst;
	char* src1c = (char*)src1;
	char* src2c = (char*)src2;

	memcpy(dstc, &src1c[startIndex1 * elemSize], size1 * elemSize);
	memcpy(&dstc[size1 * elemSize], &src2c[startIndex2 * elemSize],size2* elemSize);

}

#endif
