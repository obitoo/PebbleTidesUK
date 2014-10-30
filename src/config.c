#include <pebble.h>
#include <graph.h>
  
  
  
// on: return 1
// off: return 0 - default
int config_get_bool(int conf_item){
  
  char value[]="off"; 
  
  if (persist_exists(conf_item)) {
      persist_read_string(conf_item, value, 1+strlen(value));
  }

//   APP_LOG(APP_LOG_LEVEL_INFO, "config_bool(%d) = %s",conf_item, value );
  int v = strcmp (value, "off");
//     APP_LOG(APP_LOG_LEVEL_INFO, "config_bool(%d) = %s  returns %d",conf_item, value, v );
  return v;
}

char *config_get_string(int conf_item){
//     APP_LOG(APP_LOG_LEVEL_INFO, "config_get_string() - entry");

    static char c_port[]   ="0110";
    static char c_invert[] ="off";
    static char c_line[]   ="off";
    static char c_heights[]="off";
  
    char *tmp= NULL;
  
    switch(conf_item){
      case CFG_PORT:
          tmp = c_port;
          break;
      
      case CFG_LINE_GRAPH:
          tmp = c_line;
          break;
      
      case CFG_INVERT_COL:
          tmp = c_invert;
          break;
      
      case CFG_SHOW_HEIGHTS:
          tmp = c_heights;
          break;
      
      default:
          APP_LOG(APP_LOG_LEVEL_ERROR, "config_get_string: Unknown conf_item:%d",conf_item);
    }
  
    if (persist_exists(conf_item)) {
             persist_read_string(conf_item,  tmp, 1+strlen(tmp));
    }
//     APP_LOG(APP_LOG_LEVEL_INFO, "config string got (%d = %s)",conf_item, tmp );
    return tmp;
}

int config_get_intval (int conf_item){
  
    // return values (previously #defined) for full width or 3/4 width graph. 
    // - controlled by the 'show heights' param, becausse in 3/4 mode it shows the 
    //   tide heights to the right. 
    
    int full_width = (config_get_bool(CFG_SHOW_HEIGHTS) ? 0 : 1);
//     APP_LOG(APP_LOG_LEVEL_INFO, "config_get_intval, show_heights = %d)", full_width );

    int retval = 0;
    
    switch(conf_item){
      case   CGRAPH_NUM_POINTS    :
          retval = (full_width ? 4 : 3); 
          break;
      
      case   CGRAPH_X_PX          :  
          retval = (full_width ? (MAX_X - (2*GRAPH_BORDER_PX))  : 102); 
          APP_LOG(APP_LOG_LEVEL_WARNING, "config_get_intval, full_width = %d, xpix = %d)", full_width, retval );

          break;
      
      case   CGRAPH_X_LOGICAL_MAX :
          retval = 144;
          break;
      
      case   CGRAPH_X_MINS        :
          retval = (full_width ? 1440 : (1440 * 3/4) ); 
          break;
      
      default:
          APP_LOG(APP_LOG_LEVEL_ERROR, "config_get_intval:  Unknown conf_item:%d",conf_item);
    }
  
    //APP_LOG(APP_LOG_LEVEL_INFO, "config_get_intval returns (%d = %d)",conf_item, retval );
    return retval; 
}

void config_save_string (int conf_item, char *value){
    
  persist_write_string(conf_item, value);
  APP_LOG(APP_LOG_LEVEL_WARNING, "config string saved (%d = %s)",conf_item, value );
}