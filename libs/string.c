#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "string.h"
#define FORCE_BREAK 2
int growStr(String* str, size_t inc){
	size_t newL = inc + str->maxCapacity;
	char* nStr = NULL; 
	char count = 0;
	while ((nStr = (char*)malloc(newL)) == NULL && count < 3){
		count++;
	}
	if (count == 3) return 1;
	memcpy(nStr, str->string, str->length);
	free(str->string);
	str->string = nStr;
  str->maxCapacity = newL;
	str->string[str->length] = '\0';
	return 0;
}
int growStrClean(String* str, size_t inc){
	size_t newL = inc + str->maxCapacity;
	char* nStr = (char*)calloc(newL, newL);
	if (nStr == NULL){
		return 1;
	}
	memcpy(nStr, str->string, str->length);
	free(str->string);
	str->string = nStr;
    str->maxCapacity = newL;
	str->string[str->length] = '\0';
	return 0;
}
/* creates an empty (length 0, string[0] == '\0') string with allocSize */
String* emptyStr(size_t allocSize){
	String* string  = (String*)malloc(sizeof(struct string));
	if (string == NULL){
		return NULL;
	}
	string->maxCapacity = allocSize;
	string->length = 0;
	string->string = (char*)malloc(string->maxCapacity);
	if (string->string == NULL){
		free(string);
		return NULL;
	}
	return string;
}

/* converts a null terminated char* to a String */
String* ptrToStr(char* ptr){
	String* toRet = emptyStr(strlen(ptr));
	if (toRet == NULL){
		return NULL;
	}
	toRet->length = toRet->maxCapacity;
	memcpy(toRet->string, ptr, toRet->maxCapacity);
	return toRet;
}

/* creates a String using a char* with rawStrLen, USING THE POINTER PROVIDED BEWARE.*/
String* initStr(char* rawStr, size_t rawStrLen){
	String* string  = (String*)malloc(sizeof(struct string));
	string->maxCapacity = rawStrLen+1;
	string->length = rawStrLen;
	string->string = rawStr;
	return string;
}

/* builds a String with spare capacity from the char* with length*/ 
String* buildStr(char* pointer, size_t length){
		String* string  = (String*)malloc(sizeof(struct string));
		string->maxCapacity = length*1.5+1;
		string->length = length;
		string->string = (char*)malloc(string->maxCapacity);
		memcpy(string->string, pointer, length);
		string->length += length;
	string->string[string->length] = '\0';
	return string;
}
/* appends only part of a pointer, determined by start and end, does not stop at null terminators.*/
void appendSubPtr(String* str, char* ptr, size_t start, size_t end){
	if (str->maxCapacity < str->length + (end-start)+1){
		 growStr(str, (end-start) * 1.5 + 2);
	}	
	memcpy(&str->string[str->length], &ptr[start], end - start);
	str->length += end - start;
	str->string[str->length] = '\0';
}
int prependSubPtr(String* str, char* ptr, size_t start, size_t end){
	if (str->maxCapacity < str->length + (end-start)+1){
		if (growStr(str, (end-start) * 1.5 + 2)){
            return 1;    
        }
	}	
	memcpy(&str->string[end-start], &str->string[0], str->length);
	memcpy(&str->string[0], &ptr[start], end-start);
	str->length += end - start;
	str->string[str->length] = '\0';
  return 0;
}

int appendChar(String* str, char ch){
	if (str->length == str->maxCapacity-1){
		if(growStr(str, (str->length+1) / 2)){
			return 1;
		}
	}
	str->string[str->length] = ch;
	str->length++;
	str->string[str->length] = '\0';
	return 0;
}
int appendNoLen(String* str, char* ptr, size_t max){
	size_t i = 0;
	while (ptr[i] != '\0'){
		if (str->length == str->maxCapacity){
			if(growStr(str, (str->length+1) / 2)){
				return 1;
			}
		}
		str->string[str->length] = ptr[i];
		str->length++;
		i++;
		if (max != 0 && i == max){
			return FORCE_BREAK;
		}
	}
	return 0;
}
int appendPtr(String* str, char* ptr, size_t ptrLen){
	if (str->maxCapacity < str->length + ptrLen){
		if (growStr(str, (str->length+1) / 2)){
		 	return 1;
		}
	}
	memcpy(str->string + str->length, ptr, ptrLen);
	str->length += ptrLen;
	str->string[str->length] = '\0';
	return 0;
}
int prependPtr(String* str, char* ptr, size_t ptrLen){
	if (str->maxCapacity < str->length + ptrLen){
		if (growStr(str, ptrLen * 1.5 + 2)){
            return 1;    
        }       
	}	
	memcpy(&str->string[ptrLen], &str->string[0], str->length);
	memcpy(&str->string[0], ptr, ptrLen);
	str->length += ptrLen;
	str->string[str->length] = '\0';
    return 0;
}
void appendHeapPtr(String* str, char* ptr, size_t ptrLen){
	if (str->maxCapacity < str->length + ptrLen){
		 growStr(str, ptrLen * 1.5 + 1);
	}
		memcpy(&str->string[str->length], ptr, ptrLen);
		str->length += ptrLen;
	str->string[str->length] = '\0';
	free(ptr);
}

int appendStr(String* str, String* toAppend){
	/* avoid unnecessary grow checks */
	if (str->maxCapacity < str->length + toAppend->length){
	 if (growStr(str, toAppend->length * 1.5)){
	 	return 1;
	 }
	}
	memcpy(str->string + str->length, toAppend->string, toAppend->length);
	str->length += toAppend->length;
	str->string[str->length] = '\0';	
	return 0;
}

String* concatStr(String* str, String* toAppend){
	if (str->maxCapacity < str->length + toAppend->length){
	 	if (growStr(str, toAppend->length * 1.5)){
			return NULL;		 
	 	}
	}
	memcpy(str->string + str->length, toAppend->string, toAppend->length);
	str->length	+= toAppend->length;
	str->string[str->length] = '\0';
	free(toAppend->string);
	free(toAppend);
	return str;
}
void toUpperCase(String* str){
	/* storing the character codes prevents errors due to different standards.*/
	int Acode = 'A';
	int aCode = 'a';
	size_t i = 0;
	for (i = 0; i < str->length; i++){
		if (aCode <= str->string[i] && str->string[i] < aCode + 26){
			str->string[i] = Acode + (str->string[i] - aCode);
		}
	}
}
void toLowerCase(String* str){
	/* storing the character codes prevents errors due to different standards.*/
	int Acode = 'A';
	int aCode = 'a';
	size_t i = 0;
	for (i = 0; i < str->length; i++){
		if (Acode <= str->string[i] && str->string[i] < Acode + 26){
			str->string[i] = aCode + (str->string[i] - Acode);
		}
	}
}
/* start inclusive, end exclusive, returns string built with exact capacity.
*/
String* subStr(String* str, size_t start, size_t end){
	start = start % str->length;
	end = end % str->length;
	String* ret = malloc(sizeof(String));
	if (ret == NULL){
		return NULL;
	}
	ret->length = end - start;
	ret->maxCapacity = ret->length;
	ret->string = (char*) malloc(sizeof(char) * ret->length);
	if (ret->string == NULL){
		free(ret);
		return NULL;
	}
	memcpy(ret->string, str->string + start, ret->length);
	return ret;
}
/* start is inclusive, end is exclusive, as by default.
 * not to be confused with removeStr(String*, String*)
 * as a safe guard, ints are converted to % str->length 
 */
void removeSubStr(String* str, size_t start, size_t end){
	start = start % str->length;
	end = end % str->length;
  str->length -= end-start;	
	memcpy(str->string + start, str->string + end, str->length);
	str->string[str->length] = '\0';
}

void removeCharAt(String* str, size_t index){
		memcpy(str->string + index, str->string + index + 1, str->length - index);
		str->length--;
}

void removeChar(String* str, char character){
	size_t removed = 0;
	size_t i = 0;
	for (i = 0; i < str->length-removed; i++){
		while (str->string[i+removed] == character){
			removed++;
		}
		str->string[i] = str->string[i+removed];
	}
	str->length -= removed;
	str->string[str->length] = '\0';
}

void removeStr(String* str, String* subStr){
	size_t j = 0;
	size_t removed = 0;
	size_t i = 0;
	for (i = 0; i < str->length; i++){
		j = 0;
		while (str->string[i+j] == subStr->string[j]){	
			j++;
			if (subStr->string[j+1] == '\0'){
				i += j+1;
				removed+= subStr->length;
			}
		}
		str->string[i-removed] = str->string[i]; 	
	}
	str->length-=removed;
	str->string[str->length] = '\0';
}
void removeFirstStr(String* str, String* subStr){
	size_t j = 0;
	size_t removed = 0;
	size_t i = 0;
	for (i = 0; i < str->length; i++){
		j = 0;
		while (str->string[i+j] == subStr->string[j]){	
			j++;
			if (j+1 == subStr->length){
				i += j+1;
				removed+= subStr->length;
				while (i < str->length){
					str->string[i-removed] = str->string[i];
				}
				str->length -= removed;
				str->string[str->length] = '\0';
			}
		}
	}
}
void removeLastStr(String* str, String* subStr){
	size_t j = 1;
	size_t removed = 0;
	size_t i = 0;
	for (i = str->length; i > subStr->length - 1; i--){
		while (str->string[i - j] == subStr->string[subStr->length - j]){
			j++;
			if (subStr->length == j){
				removed += subStr->length;
				while (i < str->length){
					str->string[i-removed] = str->string[i];
					i++;
				}
				str->length-=removed;
				str->string[str->length] = '\0';
				/* loop break.*/
				return;
			}
		}
	}
}
size_t indexOfChar(String* str, char character, size_t startIndex){
	size_t start = (str->length + startIndex) % str->length;
	while (start < str->length){
		if (str->string[start] == character){
			return start;
		}
		start++;
	}
	return -1;
}

size_t lastIndexOfChar(String* str, char character, size_t endOffset){
	size_t start = (str->length + endOffset - 1) % str->length;
	while (start > 0){
		if (str->string[start] == character){
			return start;
		}
		start--;
	}
	return (str->string[0] == character) - 1;
}

size_t indexOfStr(String* str, String* subStr, size_t startIndex){
	size_t start = (str->length + startIndex) % str->length;
	size_t i = 0;
	while (start < str->length){
/* must be done in this way, for cases like ("abaabaac", "abaac")
* since the string start can be messed up by a check advancing after it
*  It's possible to check if the start is seen, but it has barely any benefit
*/
		while (str->string[start+i] == subStr->string[i]){
			i++;
			if (i+1 == subStr->length){
				return start;
			}
		}
		i = 0;
		start++;
	}
	return -1;
}

size_t lastIndexOfStr(String* str, String* subStr, size_t endOffset){
	size_t start = str->length - (endOffset % str->length);
	size_t i = subStr->length;
	while (start > subStr->length - 1){
		while (str->string[start-i] == subStr->string[subStr->length - i]){
			i--;
			if (i == 0){
				return start-subStr->length;
			}	
		}
		start--;
		i = subStr->length;
	}
	return -1;
}
void replaceChar(String* str, char target, char sub){
	size_t i = 0;
	for (i = 0; i < str->length; i++){
		if (str->string[i] == target){
			str->string[i] = sub;
		}
	}
}
void replaceStr(String* str, String* target, String* sub){
	size_t j = 0;
	size_t i = 0;
	for (i = 0; i < str->length - target->length; i++){
		j = 0;
		while (str->string[i+j] == target->string[j]){
			j++;
			if (j == target->length){
				if (str->maxCapacity < str->length + sub->length - target->length){
					/* allocate extra space for less reallocs */
					growStr(str, (sub->length - target->length)*2);
				}
				str->length += sub->length - target->length;
				str->string[str->length] = '\0';
				/* has to be backwards since we are moving chars further.*/
				size_t k = 0;
				for (k = str->length-1; k > i+sub->length-1; k--){
					str->string[k] = str->string[k - sub->length + target->length];
				}
				for (k = 0; k < sub->length; k++){
					str->string[i+k] = sub->string[k];
				}
				i += sub->length - target->length;
				
			}
		}
	}
	str->string[str->length] = '\0';
}
void replaceFirstStr(String* str, String* target, String* sub){
	size_t j = 0;
	size_t i = 0;
	for (i = 0; i < str->length - target->length; i++){
		j = 0;
		while (str->string[i+j] == target->string[j]){
			j++;
			if (j == target->length){
				if (str->maxCapacity < str->length + sub->length - target->length){
					/* there is no need for extra allocation.*/
					growStr(str, (sub->length - target->length));
				}
				str->length += sub->length - target->length;
				str->string[str->length] = '\0';
				size_t k = 0;
				for (k = str->length-1; k > i+sub->length-1; k--){
					str->string[k] = str->string[k - sub->length + target->length];
				}
				for (k = 0; k < sub->length; k++){
					str->string[i+k] = sub->string[k];
				}
				i += sub->length - target->length;
				return;
			}
		}
	}
	str->string[str->length] = '\0';
}
void replaceLastStr(String* str, String* target, String* sub){
	size_t j = 1;
	size_t i = 0;
	for (i = str->length; i > target->length-1; i--){
		j = 1;
		while (str->string[i-j] == target->string[target->length - j]){
			j++;
			if (j == target->length+1){
				if (str->maxCapacity < str->length + sub->length - target->length){
					growStr(str, (sub->length - target->length));
				}
				str->length += sub->length - target->length;
				i -= target->length;
				str->string[str->length] = '\0';
				size_t k = 0;
				for (k = str->length-1; k > i+sub->length-1; k--){
					str->string[k] = str->string[k - sub->length + target->length];
				}
				for (k = 0; k < sub->length; k++){
					str->string[i+k] = sub->string[k];
				}
				return;
			}
		}
	}
	str->string[str->length] = '\0';
}
/* returns 1 if the strings are equal, otherwise returns 0.
 * does not compare after String.length, it may contain trash data after that.
 * trash data is non-zeroed and unsanitized.
 */
int strEqual(String* str1, String* str2){
	if (str1->length == str2->length){
		size_t i = 0;
		for (i = 0; i < str1->length; i++){
			if (str1->string[i] != str2->string[i]){
			return 0;
			}
		}
		return 1;
	}
	return 0;
}
size_t evaluateStr(String* str){
	size_t value = 0;
	size_t i = 0;
	for (i = 0; i < str->length; i++){
		value += str->string[i];
	}
	return value;
}
long long strCompare(String* str1, String* str2){
	size_t str1Val = 0;
	size_t str2Val = 0;
	size_t i;
	if (str1->length > str2->length){
		i = str1->length  - 1;
		while (i > str2->length){
			str1Val += str1->string[i];
			i--;
		}
		while (i > 0){
			str1Val += str1->string[i];
			str2Val += str2->string[i];
			i--;
		}
		str1Val += str1->string[0];
		str2Val += str2->string[0];
	} else {
		i = str2->length  - 1;
		while (i > str1->length){
			str2Val += str2->string[i];
			i--;
		}
		while (i > 0){
			str1Val += str1->string[i];
			str2Val += str2->string[i];
			i--;
		}
		str1Val += str1->string[0];
		str2Val += str2->string[0];
	}
	return str1Val - str2Val;
}
/* clones a string, will not clone content after String.length (Incase you are storing data there) */
String* cloneStr(String* str){
	String* nStr = (String*) malloc(sizeof(String));
	nStr->length = str->length;
	nStr->maxCapacity = str->maxCapacity;
	nStr->string = (char*) malloc(sizeof(char) * nStr->maxCapacity);
	memcpy(nStr->string, str->string, str->length);
	nStr->string[nStr->length] = '\0';
	return nStr;
}
size_t hashStr(void* str){
	size_t value;
	unsigned int charSize = sizeof(char);
	charSize = charSize * 8;
	String* string = (String*) str;
	size_t i = 0;
	for (i = 0; i < string->length; i++){
		/* the bit shift by charSize is to grant compatibility with other charset, such as UTF-16.*/ 
		value = ((value << charSize) | string->string[i]) & 1000000007;
	}
	return value;
}
String* joinStr(String** strings, size_t len, String* separator){
	String* joined;
	size_t sizes = 0;
	size_t i = 0;
	for (i = 0; i < len; i++){
		sizes += strings[i]->length;
	}
	sizes += (separator->length - 1) * (len - 1);
	joined = emptyStr(sizes);
	for (i = 0; i < len-1; i++){
		appendStr(joined, strings[i]);
		appendStr(joined, separator);
	}
	appendStr(joined, strings[len-1]);
	return joined;
}

/* splits the String* str by String* divisor, writing the quantity of strings after the split to int* len. */
String* splitByStr(String* str, String* divisor, size_t* len){
	size_t i = 0;
	size_t j = 0;
	size_t prev = 0;
	String* toRet = (String*) malloc(sizeof(String) * 8);
	size_t alloc = 8;
	*len = 0;
	while (i < str->length - divisor->length + 1){
        j = 0;
		while(str->string[i+j] == divisor->string[j]){
			j++;
			if (divisor->string[j] == '\0'){                
                String* temp = subStr(str, prev, i);
				toRet[*len] = *temp;
                free(temp);
				i += j;
				prev = i;
				*len += 1;
				if (*len == alloc){
				    alloc += 4;
				    String* newRet = (String*) malloc(sizeof(String) * alloc);
						size_t k = 0;
				    for(k = 0; k < *len; k++){
					    newRet[k] = toRet[k];
				    }
				    free(toRet);
				    toRet = newRet;
				}
                if (i == str->length){
                    return toRet;                
                }
                break;
			}
		}
        i++;
	}
    String* temp = subStr(str, prev, str->length);
	toRet[*len] = *temp;
    free(temp);
    *len += 1;
	return toRet;
}

/* reduces the String* str's memory allocation by reduction. */
void reduceStr(String* str, size_t reduction){
	size_t newL = str->maxCapacity - reduction;
	char* newString = (char*) malloc(newL);
	size_t i = 0;
	for (i = 0; i < newL; i++){
		newString[i] = str->string[i];
	}
	free(str->string);
	str->string = newString;
	str->maxCapacity = newL;
	str->length = newL-1;
	str->string[newL-1] = '\0';
}

/* sets the String* str's memory allocation to be exact with it's current contents*/
void trimEnd(String* str){
	reduceStr(str, str->maxCapacity - str->length);
}
/* it is a void* to easier integration to libs with need of free functions.
 * frees the String* str memory */
void discardStr(void* str){
	free(((String*)str)->string);
	free(str);
}
/* verbosity indicates what should be printed: 
 * 0 prints the string's contents before str->length, using default string printing.
 * 1 prints the string's address, and it's capacity/filled portion.
 * 2 will also print control characters by it's escape code, wrapped in ' (i.e '\n' instead of a new line), using a custom printer, and print the char* address.
 * 3 will print characters after str->length, up to str->maxCapacity*/
void debugPrintStr(String* str, int verbosity){
	printf("-  -  -  -\n");
	if (verbosity > 0){
		printf("details of String at: %p\n", (void*) str);
        printf("it's capacity is %u characters", str->maxCapacity);
        printf(" of which %u are within the string's length\n", str->length);
	} else {
		printf("details of a String\n");
	}
	if (verbosity < 2){
		printf("it's contents are: \"%s\"\n", str->string);
	} else {
		size_t i = 0;
		size_t limit = 0;
		if (verbosity == 2){
			limit = str->length + 1;
		} else {
			limit = str->maxCapacity;
		}
		printf("it has it's char pointer at: %p;\nand it's contents are: \"", (void*)str->string);
		while (i < limit){
			switch (str->string[i]){
				case '\0':
					printf("|\\0|");
					break;
				case '\n':
					printf("|\\n|");
					break;
				case '\r':
					printf("|\\r|");
					break;
				case '\t':
					printf("|\\t|");
					break;
				case '\v':
					printf("|\\v|");
					break;
				default: 
					printf("%c", str->string[i]);
					break;
			}
            i++;
		}
        printf("\"\n");
	}
	printf("-  -  -  -\n");
}
