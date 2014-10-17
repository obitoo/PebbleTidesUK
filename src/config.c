#include <pebble.h>
#include <graph.h>
  
  
  
// on: return 1
// off: return 0
int config_bool(int conf_item){
  
  char value[]="off"; 
  
  if (persist_exists(conf_item)) {
      persist_read_string(conf_item, value, 1+strlen(value));
  }

  APP_LOG(APP_LOG_LEVEL_WARNING, "config_bool(%d) = %s",conf_item, value );
  return strcmp (value, "off");
}