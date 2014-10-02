#pragma once

  
#define KEY_STATE_0 0 
#define  KEY_TIME_0 1
#define KEY_STATE_1 2
#define  KEY_TIME_1 3
#define KEY_STATE_2 4
#define  KEY_TIME_2 5
#define KEY_STATE_3 6
#define  KEY_TIME_3 7
#define  KEY_HEIGHT_0 8
#define  KEY_HEIGHT_1 9
#define  KEY_HEIGHT_2 10
#define  KEY_HEIGHT_3 11

  
#define  MINS_IN_DAY      1440
#define  TIDE_PHONE_POLL_MINS   30
  
  // display options
#define  SOLID_GRAPH    0   // if 0, plot a line graph with GRAPH_NUM_POINTS hi/lo points / if 1, plot solid curve
#define  BLACK_ON_WHITE 0   // TODO - 1 doesnt work  

  
// these dont change whatever the size of the graph window
#define  GRAPH_Y_PX          45
#define  GRAPH_NUM_HOZ_LINES 3 // 1 or 3 if you prefer  
#define  MAX_X            144
#define  GRAPH_BORDER_PX     4
#define  GRAPH_Y_LOWPOINT    6 // from wherever the border is 
#define  GRAPH_EXAGGERATE_Y  3 // Factor to exaggerate the height differences 
  
#define  NUM_TIDES_BACKGROUND  4 // process 4, display less

  
// // different graph window widths
// //     // fullwidth grap 
// #define  GRAPH_NUM_POINTS    4
// #define  GRAPH_X_PX          (MAX_X - (2*GRAPH_BORDER_PX)) 
// #define  GRAPH_X_LOGICAL_MAX 144
// #define  GRAPH_X_MINS        1440 // 24 hours 
  
//       3/4 width grap - with tide heights
#define  GRAPH_NUM_POINTS    3
#define  GRAPH_X_PX          102
#define  GRAPH_X_LOGICAL_MAX 144
#define  GRAPH_X_MINS        (1440 * 3/4) // 18 hours 
  
//     // half width grap 
// #define  GRAPH_NUM_POINTS    2
// #define  GRAPH_X_PX          70
//   #define  GRAPH_X_LOGICAL_MAX 144
// #define  GRAPH_X_MINS        (1440 / 2) // 12 hours 


   // debug 
// #define DEBUG_TIME_NOW_MINS  1020  // 17:00
// #define DEBUG_TIME_NOW_MINS  1316  // 21:56
// #define DEBUG_TIME_NOW_MINS  870  // 14:30
  

 

  
  