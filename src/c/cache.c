#include <pebble.h>
#include "graph.h"

static char state_cache[4][8];   // "hi" | "lo"
static char time_cache[4][6];    // "23:44"
static int  height_cache[5];    // "56"  = 5.6m 
static char portname_cache[31];    //   "Southend-on-sea"
static char hh_mm[]="00:00";    // last update time


#define STATE_KEY          1
#define HEIGHT_KEY         2
#define TIME_KEY           3
#define PORTNAME_KEY       4
#define TIME_INIT_KEY      5
#define TIME_INIT_KEY_HHMM 6
  
  
  
extern void app_log_ts(int level, char* fmt, ...);
  
// Call once from init()/deinit()
void cache_init()
{
    app_log_ts(APP_LOG_LEVEL_DEBUG, "         cache_init()  ");

    if (persist_exists(STATE_KEY)){
      persist_read_data(STATE_KEY, state_cache, sizeof(state_cache));
    }
    if (persist_exists(TIME_KEY)){
      persist_read_data(TIME_KEY, time_cache, sizeof(time_cache));
    }
    if (persist_exists(HEIGHT_KEY)){
      persist_read_data(HEIGHT_KEY, height_cache, sizeof(height_cache));
    }
    if (persist_exists(PORTNAME_KEY)){
      persist_read_string(PORTNAME_KEY, portname_cache, sizeof(portname_cache));
    }
    app_log_ts(APP_LOG_LEVEL_DEBUG, "         cache_init() - exit  ");

}

void cache_deinit()
{
   // APP_LOG(APP_LOG_LEVEL_DEBUG, "         cache_deinit()  ");

    persist_write_data(STATE_KEY, state_cache, sizeof(state_cache));
    persist_write_data(TIME_KEY, time_cache, sizeof(time_cache));
    persist_write_data(HEIGHT_KEY, height_cache, sizeof(height_cache));
    persist_write_string(PORTNAME_KEY, portname_cache); 

   // APP_LOG(APP_LOG_LEVEL_DEBUG, "         cache_deinit() - exit ");

}

  
//
// Write
//
void cache_set_state (int key, char *cstring) {
    snprintf(state_cache[key], sizeof(state_cache[key]), "%s", cstring);
}
void cache_set_time (int key, char *cstring) {
    snprintf(time_cache[key], sizeof(time_cache[key]), "%s", cstring);
}
void cache_set_height (int key, int intval){
    height_cache[key] = intval;
}
void cache_set_cachekey(char* cstring){
    snprintf(portname_cache, sizeof(portname_cache), "%s", cstring);
}



//
// Read
//
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



//
//  Ageing logic
//
int cache_stale(){
  // get mins since epoch 
  time_t epoch = time(NULL);
  uint16_t now_mins = epoch / 60;
  
  // retrieve time cache was initialised 
  uint16_t cache_init = persist_read_int (TIME_INIT_KEY);
  
  uint16_t cache_age = now_mins-cache_init; 
  
  if (cache_age > CACHE_MAX_MINS) {
//     APP_LOG(APP_LOG_LEVEL_INFO, "         cache_stale() = 1   now= %u, cache_init = %u", (unsigned int)now_mins, (unsigned int) cache_init);
    return 1;
  }
//   APP_LOG(APP_LOG_LEVEL_INFO, "         cache_stale() = 0   now= %u, cache_init = %u", (unsigned int)now_mins, (unsigned int) cache_init);
  return 0;
}

void cache_set_refreshed(){
  // get time since epoch and store
  time_t epoch = time(NULL);
  uint16_t epoch_mins = epoch / 60;
  APP_LOG(APP_LOG_LEVEL_WARNING, "         cache_set_refreshed()  epoch_mins= %u", (unsigned int)epoch_mins);
  
  persist_write_int (TIME_INIT_KEY, epoch_mins);
  
  // also store as a 24h hh:mm string - will display when the cache eventually goes stale
  struct tm *tick_time = localtime(&epoch);
  static char buffer[] = "00:00";
  strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  persist_write_string(TIME_INIT_KEY_HHMM, buffer);
  
  APP_LOG(APP_LOG_LEVEL_WARNING, "         cache_set_refreshed()  hh_mm= %s", buffer);
  return;
}

// retrieve time cache was initialised 

char *cache_last_update() {
  if (persist_exists(TIME_INIT_KEY_HHMM)){
     persist_read_string(TIME_INIT_KEY_HHMM, hh_mm, sizeof(hh_mm));
  }
  return hh_mm;
}


  

 

