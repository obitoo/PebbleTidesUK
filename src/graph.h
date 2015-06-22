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
#pragma once

#define  RELEASE_VSN   3.0
  
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
#define  CFG_TIME         17
#define  CFG_VERSION      18
#define  CFG_PORTNAME     19
#define  CFG_DST          20
#define  CFG_OFFSET       21
#define  KEY_PORTNAME     22
#define  CFG_DATE         23 
#define  CFG_SHOW_FEET    24

  
  

  
// these dont change whatever the size of the graph window
#define  MINS_IN_DAY            1440    
#define  TIDE_PHONE_POLL_MINS   30 //  10  // approx every x-pixel
#define  GRAPH_Y_PX             45
#define  GRAPH_NUM_HOZ_LINES    3 // 1 or 3 if you prefer  
#define  MAX_X                  144
#define  GRAPH_BORDER_PX        0
#define  GRAPH_Y_LOWPOINT       4 // from wherever the border is 
#define  GRAPH_EXAGGERATE_Y     4 // TODO 
  
#define  NUM_TIDES_BACKGROUND   4 // loop to 4 

#define LINE_GRAPH_WIDTH_PX     1
#define PORTNAME_MAX_CHARS      22  // for 18 font     

  
// these are for config.c 
#define  CGRAPH_NUM_POINTS    0
#define  CGRAPH_X_PX          1  
#define  CGRAPH_X_LOGICAL_MAX 2
#define  CGRAPH_X_MINS        3 
  
// #define  CACHE_MAX_MINS       360  // 6 hours 
#define  CACHE_MAX_MINS       720  // 12 hours 


   // debug 
// #define DEBUG_TIME_NOW_MINS  1020  // 17:00
// #define DEBUG_TIME_NOW_MINS  1319  // 21:59
// #define DEBUG_TIME_NOW_MINS  1439  // 23:59
// #define DEBUG_TIME_NOW_MINS  1     // 00:01
// #define DEBUG_TIME_NOW_MINS  181     // 03:01
// #define DEBUG_TIME_NOW_MINS  241     // 04:01
// #define DEBUG_TIME_NOW_MINS  690  // 11:30
// #define DEBUG_TIME_NOW_MINS  870  // 14:30
  

 

  
  
