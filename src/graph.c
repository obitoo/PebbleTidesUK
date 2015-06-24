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
#include <graph.h> 
#include <config.h> 
#include <stdarg.h>
 

static int calc_mins (char *, int *);
extern GColor colour_fg();
extern GColor colour_fg_dim();

extern GColor colour_bg();
  
static int draw_x[] = {0,0,0,0,0};  // relative  // GRAPH_NUM_POINTS
static int draw_y[] = {0,0,0,0,0};

extern  TextLayer   *s_tidetimes_text_layer;
extern  TextLayer   *s_tideheight_text_layer1;
extern  TextLayer   *s_tideheight_text_layer2;
extern  TextLayer   *s_portname_text_layer;


static void calc_graph_points (char (*p_state_buf)[8], char (*p_time_buf)[6], int *p_height_buf);


static void plot_quarter_line (GContext* ctx, int x1, int y1, int x2, int y2); // TODO - move
static int calc_y_range (char (*p_state_buf)[8],  int *p_height_buf, int *min_y, int *max_y);
static int calc_localtime_mins();

static void draw_box(GContext* ctx);
static void draw_tidepoints(GContext* ctx);
static void draw_sinewave (GContext* ctx);


static void print_tidetimes       (char (*p_state)[8], char (*p_time)[6]);
static void print_tideheights     (char (*p_state)[8], int *p_height, char *);
static void print_portname        (char *p_portname);
static char* left_side_of_string  (char* dest, int h);
static void convert_m_ft          (int, char *, int *);
static void print_tide_text_layers (char (*p_state_buf)[8], char (*p_time_buf)[6], int *p_height_buf,char *time_str,char *p_portname);

    
extern int cache_stale();

// The following for converting to feet+inches
#define RATIO               3.2808399
#define ROUND_NEAREST       3

extern char  (*cache_get_state_buf())[8];
extern char  (*cache_get_time_buf())[6];
extern  int* cache_get_height_buf();
extern char* cache_get_portname_buf();
extern char* cache_last_update();

void app_log_ts(int level, char* fmt, ...) {
  static char format_string[256];
  static char output_string[256];

  va_list argptr;
  va_start(argptr,fmt);
  va_end(argptr);

  time_t epoch = time(NULL);
//   snprintf (format_string, sizeof(format_string), "%u:%s",(unsigned int) epoch, fmt); 
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "format string: %s",format_string);
//   snprintf (output_string, sizeof(output_string), format_string,argptr ); 
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "output string: %s",output_string);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "%u",(unsigned int) epoch);
  APP_LOG(level, fmt, argptr);
  
}






  //
  // entry points  ==============================
  //
 
void gfx_layer_update_callback(Layer *me, GContext* ctx) {
  app_log_ts(APP_LOG_LEVEL_WARNING, "fn_entry:  0:   gfx_layer_update_callback()");

  if (1){
    // set stroke colour - here? every time? 
    //graphics_context_set_stroke_color(ctx, colour_fg_dim());

    // redo this every call - its time based
    calc_graph_points(cache_get_state_buf(), 
                      cache_get_time_buf(), 
                      cache_get_height_buf());
    app_log_ts(APP_LOG_LEVEL_WARNING, "         1:  gfx_layer_update_callback()");

    // text - used to be only on receipt of tides from phone (10 mins) but now we're doing 
    // it every redraw (err, how long?)
    print_tide_text_layers(cache_get_state_buf(), 
                           cache_get_time_buf(), 
                           cache_get_height_buf(), 
                           cache_last_update(), 
                           cache_get_portname_buf());

    // render
    app_log_ts(APP_LOG_LEVEL_WARNING, "         2:  gfx_layer_update_callback()");
    draw_box(ctx);
    app_log_ts(APP_LOG_LEVEL_WARNING, "         3:  gfx_layer_update_callback()");
    draw_tidepoints(ctx); 
    app_log_ts(APP_LOG_LEVEL_WARNING, "         4:  gfx_layer_update_callback()");

    draw_sinewave(ctx);
    
  }  
  app_log_ts(APP_LOG_LEVEL_WARNING, "fn_exit:  5:  gfx_layer_update_callback()");

}

void print_tide_text_layers (char (*p_state_buf)[8], 
                             char (*p_time_buf)[6], 
                             int *p_height_buf, 
                             char *time_str, 
                             char *p_portname) {
  
  // colour - setting it in main_set_colours() doesnt work for some reason
  text_layer_set_text_color(s_tidetimes_text_layer, colour_fg());

  // times
  print_tidetimes(p_state_buf, p_time_buf);
  
  // heights  
  if (config_get_intval(CGRAPH_NUM_POINTS)  < 4) 
    print_tideheights (p_state_buf, p_height_buf, time_str);
 
  // Portname
  print_portname (p_portname);
  
}




    //
    //  PRESENTATION ======================================================
    //

static void print_portname(char *p_portname)
{
  static char display_str[1+PORTNAME_MAX_CHARS];
  int offset = config_get_intval(CFG_OFFSET); 
  

  
  // append offset time to portname 
  if (offset > 0)
    snprintf (display_str,PORTNAME_MAX_CHARS,"%s+%d",p_portname, offset );
  else if (offset < 0)
    snprintf (display_str,PORTNAME_MAX_CHARS,"%s%d",p_portname, offset );
  else
    snprintf (display_str,PORTNAME_MAX_CHARS,"%s",p_portname );

  // output
  if (config_get_bool(CFG_PORTNAME)) {
      text_layer_set_text(s_portname_text_layer, display_str);
  }
  else {
     text_layer_set_text(s_portname_text_layer, "");
  }
}
                               



static void draw_box(GContext* ctx){
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  draw_box()");
  
#ifdef PBL_COLOR
  graphics_context_set_stroke_color(ctx, GColorPictonBlue);
  graphics_context_set_fill_color(ctx, GColorPictonBlue);
#else
  graphics_context_set_stroke_color(ctx, colour_fg());
#endif

  // nice border
  int _xpix = config_get_intval(CGRAPH_X_PX);  

#ifdef PBL_COLOR
  graphics_fill_rect	(	 	ctx, 
                       (GRect) {.origin = { 0, 0 }, .size = { 144 , GRAPH_Y_PX+3}},
                      0,GCornerNone);
  // midline(s)
  graphics_context_set_stroke_color(ctx, GColorBlack);
#else
  graphics_draw_rect	(	 	ctx, (GRect) {.origin = { GRAPH_BORDER_PX, GRAPH_BORDER_PX }, .size = { _xpix , GRAPH_Y_PX}});		
#endif


  int  y = GRAPH_BORDER_PX;
  for (; y < GRAPH_Y_PX; y += (GRAPH_Y_PX/(1+GRAPH_NUM_HOZ_LINES))  )
    graphics_draw_line(ctx, (GPoint){GRAPH_BORDER_PX, y},
                            (GPoint){GRAPH_BORDER_PX + _xpix, y} );


}

static void draw_tidepoints(GContext* ctx){
    APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  draw_tidepoints()");

    if (!config_get_bool(CFG_LINE_GRAPH))
      return;  
  
#ifdef PBL_COLOR
    return; // TODO
#endif

    graphics_context_set_fill_color(ctx, colour_fg());

    int i;  


    int blob_radius = 3;
    for (i=0; i< config_get_intval(CGRAPH_NUM_POINTS) ; i++)
        if ((draw_x[i] > 0) && (draw_y[i] > 0) && (draw_x[i] < config_get_intval(CGRAPH_X_PX))){//} - blob_radius)){
          if (cache_stale())
              graphics_draw_circle(ctx, (GPoint){draw_x[i], draw_y[i]} , blob_radius);      
          else
              graphics_fill_circle(ctx, (GPoint){draw_x[i], draw_y[i]} , blob_radius);   
      
//            APP_LOG(APP_LOG_LEVEL_INFO, "          draw_tidepoints, abs:(%d,%d)  ", draw_x[i], draw_y[i] );
    }

}




static void print_tidetimes(char (*p_state)[8], char (*p_time)[6]){
    APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  print_tidetimes()");

    static char text_layer_buffer[128];
  
    // handle server errors, 
    if (!strcmp (p_state[0],"404")){
      snprintf(text_layer_buffer, sizeof(text_layer_buffer), "Error: 404 (port %s)",p_time[0]);
      APP_LOG(APP_LOG_LEVEL_INFO, "404 Not found ");
      text_layer_set_text(s_tidetimes_text_layer, text_layer_buffer);
      return;
    }
    if (!strcmp (p_state[0],"1")){
      snprintf(text_layer_buffer, sizeof(text_layer_buffer), "%s","Server Error: timeout");
      APP_LOG(APP_LOG_LEVEL_INFO, "Server timeout error");
      text_layer_set_text(s_tidetimes_text_layer, text_layer_buffer);
      return;
    }
  
      
    // All ok, so write out the data.. but check if first is in the past 
    int style = config_get_intval(CGRAPH_NUM_POINTS);
    int offset;
    int time_now_mins = calc_localtime_mins();
    int prev_mins     = time_now_mins;
    int tidetime_mins = calc_mins (p_time[0], &prev_mins);
    if (tidetime_mins < time_now_mins) {
      style = 3;
      offset = 1;
    }
    tidetime_mins = calc_mins (p_time[1], &prev_mins);
    if (tidetime_mins < time_now_mins) {
      style = 2;
      offset = 2;
    }

    switch (style){
      case 4:
// #ifdef PBL_COLOR
        snprintf(text_layer_buffer, sizeof(text_layer_buffer), 
                 "%s:%s     %s:%s\n    %s:%s      %s:%s\n",
                 p_state[0], p_time[0] ,           p_state[2], p_time[2],
                 p_state[1], p_time[1] ,           p_state[3], p_time[3]);
// #else
//         snprintf(text_layer_buffer, sizeof(text_layer_buffer), 
//                  "%s: %s         %s: %s       \n       %s: %s         %s: %s\n",
//                  p_state[0], p_time[0] ,           p_state[2], p_time[2],
//                  p_state[1], p_time[1] ,           p_state[3], p_time[3]);

// #endif
        break;
      case 3:
        snprintf(text_layer_buffer, sizeof(text_layer_buffer), 
                 "%s: %s    %s: %s\n         %s: %s       ",
                 p_state[0+offset], p_time[0+offset] ,           p_state[2+offset], p_time[2+offset],
                 p_state[1+offset], p_time[1+offset]  );
        break;
      case 2:
        snprintf(text_layer_buffer, sizeof(text_layer_buffer), 
                 "%s: %s          \n         %s: %s       ",
                 p_state[0+offset], p_time[0+offset] ,          
                 p_state[1+offset], p_time[1+offset]  );
        break;
      default:
        strcpy (text_layer_buffer, "--error in print_tidetimes() --");
        break;     
    }
 
    text_layer_set_text(s_tidetimes_text_layer, text_layer_buffer);

}






static void print_tideheights(char (*p_state)[8], int *p_height, char *p_timestr){
    APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  print_tideheights()");
    static char text_layer_buffer1[32];
    static char text_layer_buffer2[32];
    char TODO_info[] = "      ";   //       char TODO_info[] = "Spring";
    int min_h, max_h;
  
 
    calc_y_range(p_state, p_height, &min_h, &max_h);

    // 3 lines - hi, lo heights plus a line in the middle (spring notification?? ):
    // need 2 layers to get the vertical alignment right
    if (config_get_bool(CFG_SHOW_FEET))
    {
        int i_inches;
        char s_feet[4];
        convert_m_ft (max_h, s_feet, &i_inches);
       
        snprintf(text_layer_buffer1, 
                 sizeof(text_layer_buffer1),
                 " %s'%d\"\n  %s", 
                 s_feet, i_inches, TODO_info);
      
        convert_m_ft (min_h, s_feet, &i_inches);
        snprintf(text_layer_buffer2, 
                 sizeof(text_layer_buffer2), 
                 " %s'%d\"\n", 
                 s_feet, i_inches);
    }
    else 
    {
        char left[4];
        snprintf(text_layer_buffer1, 
                 sizeof(text_layer_buffer1),
                 " %s.%dm\n  %s", 
                 left_side_of_string(left, max_h), abs(max_h%10), TODO_info);
        snprintf(text_layer_buffer2, 
                 sizeof(text_layer_buffer2),
                 " %s.%dm\n", 
                 left_side_of_string(left,min_h), abs(min_h%10));
    }
  
    // If we've lost comms to the phone, show last good update time
    if (cache_stale()){ 
        snprintf (text_layer_buffer1, sizeof(text_layer_buffer1)," Stale\n %s",p_timestr);
        strcpy (text_layer_buffer2, "");
    }

    text_layer_set_text(s_tideheight_text_layer1, text_layer_buffer1);
    text_layer_set_text(s_tideheight_text_layer2, text_layer_buffer2);
  
    APP_LOG(APP_LOG_LEVEL_INFO, "fn_exit:  print_tideheights()");
}




//  
static void convert_m_ft(int meters, char *s_feet, int *i_inches){
    double d_feet = meters * RATIO / 10;  // no decimal point in the string from the phone, so *10
    double d_inches = abs(12*(d_feet - (int)d_feet));

    if ( meters < 0)
      snprintf (s_feet,4,"-%d",(int)d_feet);
    else
      snprintf (s_feet,4,"%d",(int)d_feet);
  
  
    *i_inches = ((int) (d_inches/ROUND_NEAREST)) * ROUND_NEAREST; // eg nearest 3"

    APP_LOG(APP_LOG_LEVEL_WARNING, "        metres=%d,   feet_d=%f  inches_d=%f", meters, d_feet, d_inches);
    APP_LOG(APP_LOG_LEVEL_WARNING, "                     feet_s=%s  inches_i=%d",   s_feet, *i_inches);
}

/* 
-11 -> -1.1
010 ->  1.0 
-01 -> -0.1
*/
static char* left_side_of_string(char* dest, int h){
  if ( h < 0)
    snprintf (dest,4,"-%d",h/10);
  else
    snprintf (dest,4,"%d",h/10);
  return dest;
}












static void draw_sinewave (GContext* ctx){
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  draw_sinewave()");

//   int x = 0;
//   for (;x<=4; x++) 
//       APP_LOG(APP_LOG_LEVEL_INFO, "        point %d:  %d, %d", x, draw_x[x], draw_y[x]);
 
  graphics_context_set_stroke_color(ctx, colour_fg_dim());

  // this one is offscreen, so we guess a bit
  plot_quarter_line(ctx, 
                    draw_x[0]-draw_x[2]+draw_x[1], 
                    draw_y[3],
                    draw_x[0],
                    draw_y[0]);
  plot_quarter_line(ctx,
                    draw_x[0], 
                    draw_y[0],
                    draw_x[1],
                    draw_y[1]);
  plot_quarter_line(ctx,
                    draw_x[1], 
                    draw_y[1],
                    draw_x[2],
                    draw_y[2]);
  plot_quarter_line(ctx,
                    draw_x[2], 
                    draw_y[2],
                    draw_x[3],
                    draw_y[3]);
  // this one is offscreen, so we guess a bit
  plot_quarter_line(ctx,
                    draw_x[3], 
                    draw_y[3],
                    draw_x[3] + draw_x[2] - draw_x[1],
                    draw_y[2]);

}




static void plot_pixel_viewable (GContext* ctx, int xpix, int line_graph, int x, int y) {
  
#ifdef PBL_COLOR
      if ((x >= GRAPH_BORDER_PX) && (x < xpix + GRAPH_BORDER_PX)) {
#else
      if ((x > GRAPH_BORDER_PX) && (x < xpix + GRAPH_BORDER_PX)) {
#endif
        if (line_graph) 
            graphics_draw_line(ctx, (GPoint){x, y}, (GPoint){x, y+LINE_GRAPH_WIDTH_PX} );
        else 
            graphics_draw_line(ctx, (GPoint){x, y},(GPoint){x, GRAPH_Y_PX+GRAPH_BORDER_PX} );
      }
}


static void plot_quarter_line (GContext* ctx, int x1, int y1, int x2, int y2){
  
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry :  plot_quarter_line(%d,%d - %d,%d)",x1,y1,x2,y2);

  int _xpix       = config_get_intval(CGRAPH_X_PX);
  int line_graph  = config_get_bool(CFG_LINE_GRAPH);

  
  int32_t x, y;
  int range_x       = x2 -x1;
  int range_y       = y2 -y1;
  const int half_pi = TRIG_MAX_ANGLE/4;
  const int      pi = TRIG_MAX_ANGLE/2;
  
  APP_LOG(APP_LOG_LEVEL_INFO, "         before cache_stale() call  ");
  cache_stale();
  int x_step = cache_stale() ? 3: 1;  // indicate stale data with a dashed graph
  APP_LOG(APP_LOG_LEVEL_INFO, "         ..after cache_stale() call  ");

  
  
   // TODO - tidy up
  
  for (x = 0; x < range_x; x = x + x_step ){
      y = (range_y/2) + range_y * sin_lookup(-half_pi + pi * x / range_x) / TRIG_MAX_RATIO / 2 ;

#ifdef PBL_COLOR
      graphics_context_set_stroke_color(ctx, colour_fg());
      plot_pixel_viewable (ctx, _xpix, line_graph,  x1 + x, y1 + y);
      plot_pixel_viewable (ctx, _xpix, line_graph,  x1 + x, y1 + y +1);
      plot_pixel_viewable (ctx, _xpix, line_graph,  x1 + x, y1 + y +2);

      graphics_context_set_stroke_color(ctx, colour_bg());
      plot_pixel_viewable (ctx, _xpix, 0,  x1 + x, y1 + y + 3);
#else
      plot_pixel_viewable (ctx, _xpix, line_graph,  x1 + x, y1 + y);
#endif
  }
}









    //
    //  LOGIC ======================================================
    //
void calc_graph_points (char (*p_state_buf)[8], char (*p_time_buf)[6], int *p_height_buf){
//   app_log_ts(APP_LOG_LEVEL_INFO, "fn_entry:  calc_graph_points()");
  int i;
  int count_input = 0, count_output = 0;
  int prev_mins;
  
  
  
  // reset globals
  for (i=0; i <= NUM_TIDES_BACKGROUND; i++){
    draw_x[i] = draw_y[i] = 0;
  }
  


  //  current time, in minutes from midnight
  int time_now_mins = calc_localtime_mins();
      prev_mins = time_now_mins;
  
  // calculate height range
  int min_h, max_h;
  int range_y = calc_y_range(p_state_buf, p_height_buf, &min_h, &max_h);
  
  // loop for each input 
  while (count_input < NUM_TIDES_BACKGROUND) {
      
      // Count mins from now to next tide hi/lo
      int tidetime_mins = calc_mins (p_time_buf[count_input], &prev_mins);
    
      //  x point
      int xpos_mins        = tidetime_mins - time_now_mins;
      int xpos_px_relative = config_get_intval(CGRAPH_X_PX) * xpos_mins / config_get_intval(CGRAPH_X_MINS);
    
      //  y point
      int ypos_px_absolute = 0;
      int hm = p_height_buf[count_input];

      if (!strcmp(p_state_buf[count_input],"hi")){
//        ypos_px_absolute =  0 +         GRAPH_BORDER_PX + GRAPH_Y_LOWPOINT + (max_h - hm) * GRAPH_EXAGGERATE_Y;
          ypos_px_absolute =  0 +         GRAPH_BORDER_PX + GRAPH_Y_LOWPOINT + (max_h - hm) * (GRAPH_Y_PX-GRAPH_Y_LOWPOINT)/range_y ;

      } else 
      if (!strcmp(p_state_buf[count_input],"lo")){
//        ypos_px_absolute = GRAPH_Y_PX + GRAPH_BORDER_PX - GRAPH_Y_LOWPOINT - (hm - min_h) * GRAPH_EXAGGERATE_Y;
        ypos_px_absolute = GRAPH_Y_PX + GRAPH_BORDER_PX - GRAPH_Y_LOWPOINT - (hm - min_h) * (GRAPH_Y_PX-GRAPH_Y_LOWPOINT)/range_y ;

      } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "do_graph_calc state_buf is %s, expecting hi|lo ", p_state_buf[count_input]);
        return;
      }
    
      // set datapoint for draw routine. draw_x and draw_y are absolute screen coords
      draw_x[count_output] = xpos_px_relative + GRAPH_BORDER_PX ; 
      draw_y[count_output] = ypos_px_absolute;

      // increment counters
      count_input++;
      count_output++;
  }  
    

  
//   app_log_ts(APP_LOG_LEVEL_INFO, "calc_graph_points() - exit ");
}



static int calc_localtime_mins(){
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  int    time_now_mins = (tick_time->tm_hour * 60 ) + tick_time->tm_min;
  
  #ifdef DEBUG_TIME_NOW_MINS
    time_now_mins=DEBUG_TIME_NOW_MINS;
  #endif
    
//   APP_LOG(APP_LOG_LEVEL_INFO, "calc_localtime_mins = %d ", time_now_mins);
  return time_now_mins;
}


static int calc_y_range (char (*p_state_buf)[8], int *p_height_buf, int *min_y, int *max_y){ 
  app_log_ts(APP_LOG_LEVEL_WARNING, "fn_entry:     calc_y_range()" );

  int i;
  *min_y=99, *max_y=-1;
  for (i = 0;  i < NUM_TIDES_BACKGROUND ; i++){

      if (!strcmp(p_state_buf[i],"lo") && p_height_buf[i] < *min_y)
        *min_y = p_height_buf[i];
      if (!strcmp(p_state_buf[i],"hi") && p_height_buf[i] > *max_y)
        *max_y = p_height_buf[i];
  }
//   APP_LOG(APP_LOG_LEVEL_INFO, "fn_exit:     calc_y_range()  min %d, max %d",*min_y,*max_y );

  app_log_ts(APP_LOG_LEVEL_WARNING, "fn_exit:     calc_y_range()" );

  return *max_y - *min_y;
}




// convert HH:MM to elapsed mins since midnight
static int calc_mins (char *s_hhmm, int *now){
//   app_log_ts(APP_LOG_LEVEL_INFO, "fn_entry:  calc_mins(%s, %d)", s_hhmm, *now );

  int tide_mins = -1;
  int i;
  char s_buf[2];
 
  
  if ( (i = strlen(s_hhmm)) != 5){
    APP_LOG(APP_LOG_LEVEL_ERROR, "          calc_mins strlen is %d, expecting 5", i);
    return -1;
  }
  
  // hours  
  strncpy (s_buf, s_hhmm, 2);
  tide_mins = atoi(s_buf) * 60;
  //mins
  strncpy (s_buf, s_hhmm+3, 2); 
  tide_mins = tide_mins + atoi(s_buf);

  // tomorrow?  but allow 60 mins grace, in case we're a bit ahead of the webdata
  // Now lets define this as 6 hrs. That'll be the cache stale time. 
//   APP_LOG(APP_LOG_LEVEL_INFO, "       calc_mins--->%d", tide_mins );

  // we're ahead (cached, link down)
  if ((*now > tide_mins) && ((*now - tide_mins ) < CACHE_MAX_MINS)) {  // by less than 12(6?) hrs
      *now = tide_mins;
//       APP_LOG(APP_LOG_LEVEL_INFO, "       (case 1) ");
  } else   
  if ((*now < tide_mins) && ((tide_mins - *now) > CACHE_MAX_MINS)) {   // now after midnight, tide b4  
    tide_mins = tide_mins - MINS_IN_DAY; 
//     APP_LOG(APP_LOG_LEVEL_INFO, "       (case 2) ");

  } else 
  // we're behind (normal)
  if (abs(tide_mins-*now) > CACHE_MAX_MINS){
    tide_mins = tide_mins + MINS_IN_DAY;
//     APP_LOG(APP_LOG_LEVEL_INFO, "       (case 3) ");
//     APP_LOG(APP_LOG_LEVEL_INFO, "       calc_mins----->%d", tide_mins );
  }
  *now = tide_mins;
  
//   APP_LOG(APP_LOG_LEVEL_INFO, "calc_mins->%d", tide_mins );
  return tide_mins;
}






