#pragma once


#define MSG_TYPE    0 
  
// js message keys  
#define KEY_STATE_0 1 
#define  KEY_TIME_0 2
#define KEY_STATE_1 3
#define  KEY_TIME_1 4
#define KEY_STATE_2 5
#define  KEY_TIME_2 6
#define KEY_STATE_3 7
#define  KEY_TIME_3 8
#define  KEY_HEIGHT_0 9
#define  KEY_HEIGHT_1 10
#define  KEY_HEIGHT_2 11
#define  KEY_HEIGHT_3 12
#define  CFG_INVERT_COL   13
#define  CFG_SHOW_HEIGHTS 14
#define  CFG_LINE_GRAPH   15
#define  CFG_PORT         16
  
  
  

  
  // display options
//#define  BLACK_ON_WHITE 1   // TODO - 1 doesnt work  

  
// these dont change whatever the size of the graph window
#define  MINS_IN_DAY            1440    
#define  TIDE_PHONE_POLL_MINS   10  // approx every x-pixel
#define  GRAPH_Y_PX             45
#define  GRAPH_NUM_HOZ_LINES    3 // 1 or 3 if you prefer  
#define  MAX_X                  144
#define  GRAPH_BORDER_PX        4
#define  GRAPH_Y_LOWPOINT       6 // from wherever the border is 
#define  GRAPH_EXAGGERATE_Y     2 // Factor to exaggerate the height differences 
  
#define  NUM_TIDES_BACKGROUND   4 // loop to 4 

  

  
// these replace the above
#define  CGRAPH_NUM_POINTS    0
#define  CGRAPH_X_PX          1  
#define  CGRAPH_X_LOGICAL_MAX 2
#define  CGRAPH_X_MINS        3 
  


   // debug 
// #define DEBUG_TIME_NOW_MINS  1020  // 17:00
// #define DEBUG_TIME_NOW_MINS  1316  // 21:56
// #define DEBUG_TIME_NOW_MINS  870  // 14:30
  

 

  
  