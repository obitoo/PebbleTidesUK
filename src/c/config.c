/* ---------------------------------------------------------
    Pebble Tides UK
    Copyright (C) 2014 Owen Bullock 
                       owen.skimapp@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ---------------------------------------------------------*/

#include <pebble.h>
#include "graph.h"
  
char *config_get_string(int );

  
// on: return 1
// off: return 0   (defaults defined in next fn)
int config_get_bool(int conf_item){
  
  char value[4];
  strcpy (value, config_get_string (conf_item));

//   if (CFG_PORTNAME == conf_item)
//       // APP_LOG(APP_LOG_LEVEL_ERROR, "config_get_bool CFG_PORTNAME is %s which is %d",value,strcmp (value, "off"));

  return strcmp (value, "off");
}


char *config_get_string(int conf_item){
    static char c_port[10];
    static char c_invert[] ="off";
    static char c_line[]   ="off";
    static char c_heights[]="off";
    static char c_portname[4];
    static char c_dst[]     ="off";
    static char c_feet[]    ="off";
  
  
    strcpy (c_port    , "263:0110");
    strcpy (c_portname, "on");
    

  
    char *tmp= NULL;
    int  maxlen = 4;
  
    switch(conf_item){

      case CFG_PORT:
          tmp = c_port;
          maxlen = 10;
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
      case CFG_PORTNAME:
          tmp = c_portname;
          break;
      case CFG_DST:
          tmp = c_dst;
          break;
      case CFG_SHOW_FEET:
          tmp = c_feet;
          break;
      
      default:
         APP_LOG(APP_LOG_LEVEL_ERROR, "config_get_string: Unknown conf_item:%d",conf_item);
    }
  
    if (persist_exists(conf_item)) {
             persist_read_string(conf_item,  tmp, maxlen);
    }
    return tmp;
}




int config_get_intval (int conf_item){
    // return values (previously #defined) for full width or 3/4 width graph. 
    // (controlled by the 'show heights' param, because in 3/4 mode it shows the 
    //   tide heights to the right.)
    int full_width = (config_get_bool(CFG_SHOW_HEIGHTS) ? 0 : 1);
    int retval = 0;
  
    static int c_offset  =0;

    
    switch(conf_item){
      case   CGRAPH_NUM_POINTS    :
          retval = (full_width ? 4 : 3); 
          break;
      
      case   CGRAPH_X_PX          :  
          retval = (full_width ? (MAX_X - (2*GRAPH_BORDER_PX))  : 102); 

          break;
      
      case   CGRAPH_X_LOGICAL_MAX :
          retval = 144;
          break;
      
      case   CGRAPH_X_MINS        :
          retval = (full_width ? 1440 : (1440 * 3/4) ); 
          break;
     
      case CFG_OFFSET:
          retval = ( persist_exists(conf_item) ? persist_read_int(conf_item) :c_offset);
          break;
      

      default:
         APP_LOG(APP_LOG_LEVEL_ERROR, "config_get_intval:  Unknown conf_item:%d",conf_item);
    }
    return retval; 
}


void config_save_string (int conf_item, char *value){
  persist_write_string(conf_item, value);
 // APP_LOG(APP_LOG_LEVEL_WARNING, "config string saved (%d = %s)",conf_item, value );
 // APP_LOG(APP_LOG_LEVEL_ERROR  , "config string check: (%d = %s)",conf_item, config_get_string(conf_item) );

  
}
void config_save_int (int conf_item, int value){
  persist_write_int(conf_item, value);
 // APP_LOG(APP_LOG_LEVEL_WARNING, "config INT saved (%d = %d)",conf_item, value );
}


