/*header automatically generated by autoHead*/
#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
extern int mapErr;
#define MAP_NOSUCH 1
#define MALLOC_FAIL 2
typedef struct {
    void* key;
    void* value;
} Entry;
typedef struct node {
	void* key;
	void* val;
	struct node* next;
} node;
node* b_node(void* key, void* val);
typedef struct {
    Entry* entries;
    size_t (*hashf)(void*);
    /* used for comparison between keys, if they are equal, it should return 0, if different, it should be != 0*/
    int (*compare)(void*, void*);
    /* used to free data storage.*/    
    void (*free)(Entry*);
    size_t length;
    size_t occupied;
} HashMap;
HashMap* createMap(size_t length, size_t (*hash)(void*), int(*compare)(void*,void*),void (*freefn)(Entry*));
    /* sanitizing data, should be faster than calling calloc().*/
/* it is a good idea to rehash a map after deleting an entry, since that will most likely alter the hashcode of the object*/
int rehash(HashMap* map);
	/* cannot do in-place, since it will probably collide with things that may move after */
int growMap(HashMap* map, size_t inc);
    /* sanitizing data, should be faster than calling calloc().*/
int addPair(HashMap* map, void* key, void* val);
    /*if (map->occupied > 3 / 4 * map->length)*/
size_t hasKey(HashMap* map, void* key);
int removeKey(HashMap* map, void* key);
void* getValue(HashMap* map, void* key);
void clearMap(HashMap* map);
void discardMap(HashMap* map);
/* very shitty default hash function that should return on anything.*/
size_t defHash(void* key);
/* function to hash null terminated strings.*/
size_t strHash(void* key);
		/* the bit shift by charSize is to grant compatibility with other charset, such as UTF-16.*/ 
int strcmpWrap(void* strA, void* strB);
void defaultFree(Entry* entry);
/*verbosity indicates what to print.
 * 0 entry print for all entries
 * 1 prints hashmap entry fill rate.
 * 2 also prints hashmap address and entry adress
 * */
void debugPrintMap(HashMap* map, void (*printEntry)(Entry*), int verbosity);
/* there's not much I can do about printing, since there can be near infinite pairs, uh have this generic function.*/
void addressPrint(Entry* pair);
