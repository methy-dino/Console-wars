//header automatically generated by autoHead
#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
extern int mapErr;
#define MAP_NOSUCH 1
#define MALLOC_FAIL 2
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
HashMap* createMap(int length, unsigned int (*hash)(void*), int(*compare)(void*,void*),void (*freefn)(Entry*));
/* it is a good idea to rehash a map after deleting an entry, since that will most likely alter the hashcode of the object*/
int rehash(HashMap* map);
int growMap(HashMap* map, unsigned int inc);
int addPair(HashMap* map, void* key, void* val);
unsigned int hasKey(HashMap* map, void* key);
unsigned int removeKey(HashMap* map, void* key);
void* getValue(HashMap* map, void* key);
void clearMap(HashMap* map);
void discardMap(HashMap* map);
// very shitty default hash function that should return on anything.
unsigned int defHash(void* key);
// function to hash null terminated strings.
unsigned int strHash(void* key);
void defaultFree(Entry* entry);
/*verbosity indicates what to print.
 * 0 entry print for all entries
 * 1 prints hashmap entry fill rate.
 * 2 also prints hashmap address and entry adress
 * */
void debugPrintMap(HashMap* map, void (*printEntry)(Entry*), int verbosity);
// there's not much I can do about printing, since there can be near infinite pairs, uh have this generic function.
void addressPrint(Entry* pair);
