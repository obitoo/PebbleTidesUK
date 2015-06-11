#include <pebble.h>

static char state_cache[4][8];   // "hi" | "lo"
static char time_cache[4][6];    // "23:44"
static int  height_cache[5];    // "56"  = 5.6m 
static char portname_cache[31];    //   "Southend-on-sea"


void cache_set_cachekey(char* cstring){
    snprintf(portname_cache, sizeof(portname_cache), "%s", cstring);
}

void cache_set_state (int key, char *cstring) {
    snprintf(state_cache[key], sizeof(state_cache[key]), "%s", cstring);
}

void cache_set_time (int key, char *cstring) {
    snprintf(time_cache[key], sizeof(time_cache[key]), "%s", cstring);
}

void cache_set_height (int key, int intval){
    height_cache[key] = intval;
}




char  (*cache_get_state_buf())[8]{
  return state_cache;
}
char  (*cache_get_time_buf())[6]{
  return time_cache;
}
int* cache_get_height_buf(){
  return height_cache;
} 
char* cache_get_portname_buf(){
  return portname_cache;
}
