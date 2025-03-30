#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
int mapErr = 0;
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
HashMap* createMap(int length, unsigned int (*hash)(void*), int(*compare)(void*,void*),void (*freefn)(Entry*)){
    HashMap* ret = malloc(sizeof(HashMap));
	if (ret == NULL){
		mapErr = MALLOC_FAIL;
		return ret;
	}
    Entry* entries = malloc(sizeof(Entry) * length);
		if (entries == NULL){
			mapErr = MALLOC_FAIL;
			free(ret);
			return NULL;
		}
    // sanitizing data, should be faster than calling calloc().
    for (unsigned int i = 0; i < length; i++){
        entries[i].key = NULL;
    }
    ret->entries = entries;
    ret->hashf = hash;
    ret->free = freefn;
    ret->compare = compare;
    ret->length = length;
    ret->occupied = 0;
    return ret;
}
/* it is a good idea to rehash a map after deleting an entry, since that will most likely alter the hashcode of the object*/
int rehash(HashMap* map){
	// cannot do in-place, since it will probably collide with things that may move after
    unsigned int j = 0;
	Entry* newE = malloc(sizeof(Entry)* map->length);
	if (newE == NULL){
		mapErr = MALLOC_FAIL;
		return 1;
	}
	for (unsigned int i = 0; i < map->length; i++){
        	newE[i].key = NULL;
   	}
	for (unsigned int i = 0; i < map->length; i++){
		if (map->entries[i].key != NULL){
           		j = map->hashf(map->entries[i].key) % map->length;
            		while (newE[j].key != NULL){
                		if (j == map->length){
                    			j=0;
                		}
                		j++;
            		}
            //printf("key at %d  moved to %d\n",i,j);
            newE[j].key = map->entries[i].key;
            newE[j].value = map->entries[i].value;
       		 }
    	}
	free(map->entries);
	map->entries = newE;
	return 0;
}
int growMap(HashMap* map, unsigned int inc){
    unsigned int newL = map->length+inc;
    Entry* newE = malloc(sizeof(Entry)* newL);
		if (newE == NULL){
			mapErr = MALLOC_FAIL;
			return 1;
		}
    // sanitizing data, should be faster than calling calloc().
    for (unsigned int i = 0; i < newL; i++){
        newE[i].key = NULL;
    }
    unsigned int j = 0;
    for (unsigned int i = 0; i < map->length; i++){
        if (map->entries[i].key != NULL){
            j = map->hashf(map->entries[i].key) % newL;
            while (newE[j].key != NULL){
                if (j == newL){
                    j=0;
                }
                j++;
            }
            //printf("key at %d  moved to %d\n",i,j);
            newE[j].key = map->entries[i].key;
            newE[j].value = map->entries[i].value;
        }
    }
    free(map->entries);
    map->entries = newE;
    map->length = newL;
		return 0;
}
int addPair(HashMap* map, void* key, void* val){
    // if (map->occupied > 3 / 4 * map->length)
    if (4 * map->occupied > 3 * map->length-1){
			char rep = 0;
      while (growMap(map, map->length) && rep < 3){
				rep++;
			}
			if (rep == 3){
				return 1;
			} else {
				mapErr = 0;
			}
    }
    int index = map->hashf(key) % map->length;
    while (map->entries[index].key != NULL){
      	if (map->compare(map->entries[index].key, key) == 0){
					map->entries[index].key = key;
    			map->entries[index].value = val;
					// this should give better order
				}
				index++;
        if (index == map->length){
            index = 0;
        }	
    }
			//printf("added key at index %d\n", index);
    map->entries[index].key = key;
    map->entries[index].value = val;
    map->occupied++;
		return 0;
}
unsigned int hasKey(HashMap* map, void* key){
    unsigned int index = map->hashf(key) % map->length;
    unsigned int start = index;
    while (map->compare(map->entries[index].key, key) != 0){
        index++;
        if (index == map->length){
            index = 0;
        }
        if (index == start){
            //printf("Invalid or nonexistant key");
            return -1; //auto convert to uint max
				}
    }
    return index;
}
unsigned int removeKey(HashMap* map, void* key){
    unsigned int index = map->hashf(key) % map->length;
    unsigned int start = index;
    while (map->compare(map->entries[index].key, key) != 0){
        index++;
        if (index == map->length){
            index = 0;
        }
        if (index == start){
            //printf("Invalid or nonexistant key");
            return 1;
        }
    }
    map->occupied--;
    map->free(&map->entries[index]);
    map->entries[index].key = NULL;
		rehash(map);
    return 0;
}
void* getValue(HashMap* map, void* key){
    int index = map->hashf(key) % map->length;
    int start = index;
    while (map->compare(map->entries[index].key, key) != 0){
        index++;
        if (index == map->length){
            index = 0;
        }
        if (index == start){
						mapErr = MAP_NOSUCH;
            return (void*)NULL;
        }
    }
    return map->entries[index].value;
}
void clearMap(HashMap* map){
    for (unsigned int i = 0; i < map->length; i++){
        if (map->entries[i].key != NULL){
            map->free(&map->entries[i]);
        }
    }
}
void discardMap(HashMap* map){
    for (unsigned int i = 0; i < map->length; i++){
        if (map->entries[i].key != NULL){
            map->free(&map->entries[i]);
        }
    }
    free(map->entries);
    free(map);
}
// very shitty default hash function that should return on anything.
unsigned int defHash(void* key){
	return (int) ((char*)key)[0];
}

// function to hash null terminated strings.
unsigned int strHash(void* key){
	unsigned int value;
	unsigned int charSize = sizeof(char);
	charSize = charSize * 8;
    unsigned int i = 0;
	while (((char*)key)[i] != '\0'){
		// the bit shift by charSize is to grant compatibility with other charset, such as UTF-16. 
		value = ((value << charSize) | ((char*)key)[i]) & 65033;
		i++;
	}
	return value;
}
void defaultFree(Entry* entry){
	free(entry->key);
	free(entry->value);
}
/*verbosity indicates what to print.
 * 0 entry print for all entries
 * 1 prints hashmap entry fill rate.
 * 2 also prints hashmap address and entry adress
 * */
void debugPrintMap(HashMap* map, void (*printEntry)(Entry*), int verbosity){
	printf("-  -  -  -\n");
	if (verbosity > 1){
		printf("hashmap debug info for map at: %p, which has it's entries at:%p\n", (void*)map, (void*)&map->entries);
	} else {
		printf("hashmap debug info:\n");
	}
	if (verbosity > 0) {
		printf("it has %u entries, of which %u are full\n", map->length, map->occupied);
	}
	printf("this is it's entry data:\n");
	for (unsigned int i = 0; i < map->length; i++){
        if (map->entries[i].key != NULL){
		    printEntry(&map->entries[i]);
        } else {
            printf("NULL ENTRY\n");        
        }
	}
}
// there's not much I can do about printing, since there can be near infinite pairs, uh have this generic function.
void addressPrint(Entry* pair){
	printf("key at %p and val at %p", pair->key, pair->value);
}
