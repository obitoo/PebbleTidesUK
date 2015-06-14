#include <pebble.h>

static char state_cache[4][8];   // "hi" | "lo"
static char time_cache[4][6];    // "23:44"
static int  height_cache[5];    // "56"  = 5.6m 
static char portname_cache[31];    //   "Southend-on-sea"


#define STATE_KEY 1
#define HEIGHT_KEY 2
#define TIME_KEY 3
#define PORTNAME_KEY 4
  
  
// todo - writing out too many times. Slow?
void cache_set_state (int key, char *cstring) {
    APP_LOG(APP_LOG_LEVEL_INFO, "          cache_set_state()");
    snprintf(state_cache[key], sizeof(state_cache[key]), "%s", cstring);
    persist_write_data(STATE_KEY, state_cache, sizeof(state_cache));
}
void cache_set_time (int key, char *cstring) {
    APP_LOG(APP_LOG_LEVEL_INFO, "          cache_set_time()");
    snprintf(time_cache[key], sizeof(time_cache[key]), "%s", cstring);
    persist_write_data(TIME_KEY, time_cache, sizeof(time_cache));
}
void cache_set_height (int key, int intval){
    APP_LOG(APP_LOG_LEVEL_INFO, "          cache_set_height()");
    height_cache[key] = intval;
    persist_write_data(HEIGHT_KEY, height_cache, sizeof(height_cache));
}
void cache_set_cachekey(char* cstring){
    snprintf(portname_cache, sizeof(portname_cache), "%s", cstring);
    persist_write_string(PORTNAME_KEY, portname_cache);
}



char  (*cache_get_state_buf())[8]{
    APP_LOG(APP_LOG_LEVEL_INFO, "          cache_get_state_buf()");

    static int first_time = 1; 

    if (persist_exists(STATE_KEY)){
      persist_read_data(STATE_KEY, state_cache, sizeof(state_cache));
    }
  
//       if (first_time){
        
//       persist_read_data(TIME_KEY, time_cache, sizeof(time_cache));
//       persist_read_data(HEIGHT_KEY, height_cache, sizeof(height_cache));

//       first_time = 0;
//       APP_LOG(APP_LOG_LEVEL_WARNING, "          cache_get_state_buf - DELETE first item");
//       int i;
//       for (i=0; i<4; i++){
//         strcpy(state_cache[i], state_cache[i+1]);
//         strcpy(time_cache[i], time_cache[i+1]);
//         height_cache[i]= height_cache[i+1];
//         APP_LOG(APP_LOG_LEVEL_WARNING, "          time_cache[%d] = %s", i, time_cache[i]);

//       }
//       strcpy (state_cache[i],"");
//       strcpy (time_cache[i],"");
//       height_cache[i] = 0;
//             persist_write_data(HEIGHT_KEY, height_cache, sizeof(height_cache));
//     persist_write_data(TIME_KEY, time_cache, sizeof(time_cache));
//             persist_write_data(STATE_KEY, state_cache, sizeof(state_cache));


//     }
  
    return state_cache;
}

char  (*cache_get_time_buf())[6]{
    APP_LOG(APP_LOG_LEVEL_INFO, "          cache_get_time_buf()");

    if (persist_exists(TIME_KEY)){
      persist_read_data(TIME_KEY, time_cache, sizeof(time_cache));
    }
    return time_cache;
}

int* cache_get_height_buf(){
    APP_LOG(APP_LOG_LEVEL_INFO, "          cache_get_height_buf()");

    if (persist_exists(HEIGHT_KEY)){
      persist_read_data(HEIGHT_KEY, height_cache, sizeof(height_cache));
    }
    return height_cache;
} 

char* cache_get_portname_buf(){
  APP_LOG(APP_LOG_LEVEL_INFO, "          cache_get_portname_buf()");

  if (persist_exists(PORTNAME_KEY)){
    persist_read_string(PORTNAME_KEY, portname_cache, sizeof(portname_cache));
  }

  return portname_cache;
}


  

 

