#pragma once
#define MAP_NOSUCH 1
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
typedef struct {
    void* key;
    void* value;
} Entry;
typedef struct {
    Entry* entries;
    unsigned int (*hashf)(void*);
    // used for comparison between keys, if they are equal, it should return 0, if different, it should be != 0;
    int (*compare)(void*, void*);
    // used to free data storage.    
    void (*free)(Entry*);
    unsigned int length;
    unsigned int occupied;
} HashMap;
/* returns a HashMap* equipped with the specified capacity and functions. 
*  one should not use multiple types inside one HashMap, since it is very unsafe.*/
HashMap* createMap(int length, unsigned int (*hash)(void*), int(*compare)(void*,void*), void (*free)(Entry*));
/* rehashes the attached HashMap, will improve performance after many deletions, it is not automatically called */
void rehash(HashMap* map);
/*grows map by the provided int, automatic grows are set to when a HashMap is more than 3/4 full*/
void growMap(HashMap* map, unsigned int inc);
/* adds the Key/Value pair inputted, assuming heap allocation of contents.
* returns 1 if the key existed, else it returns 0
* data WILL most likely CORRUPT if it is allocated on the stack, and used after the scope it was introduced in ends.
*/
int addPair(HashMap* map, void* key, void* val);
/* removes the specified key, freeing the memory using the void(*free)(Entry) function and flagging the key pointer of it's entry as NULL, permitting reassigning to the slot.
* returns 0 if key was deleted, and returns 1 if an equal key was not found using int(compare)(void*,void*)
*/
int removeKey(HashMap* map, void* key);
/*returns the value associated with the key (as a void*).
*if an equal key was not found with int(compare)(void*,void*), returns (void*) NULL*/
void* getValue(HashMap* map, void* key);
/* frees all the entries of a HashMap, but keeps the map allocated. */
void clearMap(HashMap* map);
/*frees all memory in the map, including all assigned key value pairs.*/
void discardMap(HashMap* map);
// very shitty default hash function that should return on anything.
unsigned int defHash(void* key);
// function to hash null terminated strings.
unsigned int strHash(void* key);
void defaultFree(Entry* entry);
void debugPrintMap(HashMap* map, void (*printEntry)(Entry*), int verbosity);
void addressPrint(Entry* pair);
