#pragma once

  
#define KEY_STATE_0 0 
#define  KEY_TIME_0 1
#define KEY_STATE_1 2
#define  KEY_TIME_1 3
#define KEY_STATE_2 4
#define  KEY_TIME_2 5
#define KEY_STATE_3 6
#define  KEY_TIME_3 7

#define  MINS_IN_DAY      1440

#define  MAX_X            144

#define  GRAPH_BORDER_PX     4

  
// fullwidth grap 
#define  GRAPH_NUM_POINTS    4
#define  GRAPH_X_PX          (MAX_X - (2*GRAPH_BORDER_PX)) 
#define  GRAPH_X_MINS        1440 // 24 hours 
  
// half width grap 
// #define  GRAPH_NUM_POINTS    2
// #define  GRAPH_X_PX          70
// #define  GRAPH_X_MINS        (1440 * 3/4) // 24 hours 

  
  
#define  GRAPH_Y_PX          45
#define  GRAPH_NUM_HOZ_LINES 1 // 1 or 3 if you prefer

#define  SOLID_GRAPH      0    // if 0, plot a graph with GRAPH_NUM_POINTS points
#define  BLACK_ON_WHITE 0 // TODO - 1 doesnt work  
  
// #define DEBUG_TIME_NOW_MINS  1020  // 17:00
// #define DEBUG_TIME_NOW_MINS  1316  // 21:56
// #define DEBUG_TIME_NOW_MINS  870  // 14:30
  
#define  TIDE_PHONE_POLL_MINS   30
 

  
  