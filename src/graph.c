#include <pebble.h> 
#include <graph.h> 
 
// TODO  - fix sine logic

static int calc_mins (char *, int *);
extern GColor colour_fg();
extern GColor colour_bg();
  
static int draw_x[] = {0,0,0,0};  // relative
static int draw_y[] = {0,0,0,0};

extern  Layer       *s_graph_layer;
extern  TextLayer   *s_tidetimes_text_layer;

static void plot_one_wave(GContext*, int, int, int , int , int , int);

    //
    //  PRESENTATION ======================================================
    //
static void draw_box(GContext* ctx){
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  draw_box()");
  
  // nice border
  graphics_draw_rect	(	 	ctx, (GRect) {.origin = { GRAPH_BORDER_PX, GRAPH_BORDER_PX }, .size = {GRAPH_X_PX, GRAPH_Y_PX}});

  // midline(s)
  int  y = GRAPH_BORDER_PX;
  for (; y < GRAPH_Y_PX; y += (GRAPH_Y_PX/(1+GRAPH_NUM_HOZ_LINES))  )
    graphics_draw_line(ctx, (GPoint){GRAPH_BORDER_PX, y},
                            (GPoint){GRAPH_BORDER_PX+GRAPH_X_PX, y} );

}

static void draw_tidepoints(GContext* ctx){
    APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  draw_tidepoints()");

    if (SOLID_GRAPH)
      return;  
  
    graphics_context_set_fill_color(ctx, colour_fg());

    int i;  
    for (i=0; i< GRAPH_NUM_POINTS ; i++)
    if ((draw_x[i] > 0) && (draw_y[i] > 0)){
       graphics_fill_circle(ctx, (GPoint){draw_x[i] + GRAPH_BORDER_PX , draw_y[i]} , 3);
       APP_LOG(APP_LOG_LEVEL_INFO, "draw_tidepoints, abs:(%d,%d)  ", draw_x[i], draw_y[i] );
    }

}

void print_tidetimes(char (*state_buf)[8], char (*time_buf)[6]){
    APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  print_tidetimes()");

    static char text_layer_buffer[128];

    int i = 0;
    for (; i < 4; i++)
      APP_LOG(APP_LOG_LEVEL_INFO, "           print_tidetimes() %d state=%s, time=%s",i,state_buf[i], time_buf[i]);

    snprintf(text_layer_buffer, sizeof(text_layer_buffer), 
           "%s: %s         %s: %s       \n       %s: %s         %s: %s\n",
           state_buf[0], time_buf[0] ,           state_buf[2], time_buf[2],
           state_buf[1], time_buf[1] ,           state_buf[3], time_buf[3]);
 
    APP_LOG(APP_LOG_LEVEL_INFO, "text_layer_set_text: ");
    APP_LOG(APP_LOG_LEVEL_INFO, "(%s) ", text_layer_buffer);
   
    text_layer_set_text(s_tidetimes_text_layer, text_layer_buffer);
}


// draw sine wave,lamely. Would have been much easier when i was 17 doing a-level maths ;) 
// todo - cycles thru all points properly, if first in the past
static void draw_sinewave (GContext* ctx){
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  draw_sinewave()");

  int plot_x_rel, plot_y;
  int32_t xd, yd;
  int range_y = GRAPH_Y_PX /2 ;
  int range_x = GRAPH_X_PX /2 ;
  

  // plut two 'full' waves, of 0->pi each
  int x_offset = draw_x[0] - range_x/4;
  APP_LOG(APP_LOG_LEVEL_WARNING, "layer_update_callback, draw_x[0] =%d, x_offset = %d ", draw_x[0], x_offset );
  int flip_y = ((draw_y[0] < (range_y))? -1 : 1);
    
      // wave 1 - lhs graph
  plot_one_wave (ctx, (0-x_offset), range_x, draw_x[1], draw_x[0], flip_y, x_offset);
  
      // wave 2 - rhs
  plot_one_wave (ctx, range_x,   GRAPH_X_PX, draw_x[3], draw_x[2], flip_y, x_offset);
  
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_exit:    draw_sinewave()");
}


static void plot_one_wave(GContext* ctx, int xrel_from, int xrel_to, int x1, int x2, int flip_y, int x_offset){

  int     x_rel, plot_x, plot_y;
  int32_t xd, yd;

  int     range_x = GRAPH_X_PX /2 ;
  int     range_y = GRAPH_Y_PX /2 ;
  int     f = x1 - x2;
  

  for (x_rel = xrel_from; x_rel < xrel_to; x_rel++){

    xd = TRIG_MAX_ANGLE * (x_rel-range_x) /range_x;
    yd = sin_lookup(xd*range_x/(2*f)); 

    // amplitude : (GRAPH_Y_PX - 20)/GRAPH_Y_PX
    plot_y =  flip_y * ((GRAPH_Y_PX - 20) * (range_y * yd/TRIG_MAX_RATIO))/GRAPH_Y_PX + range_y  ;

    // draw 
    plot_x = x_rel + GRAPH_BORDER_PX + x_offset ;
    if ((plot_x > GRAPH_BORDER_PX) && (plot_x < GRAPH_X_PX - GRAPH_BORDER_PX)) {
        GPoint p = (GPoint){plot_x, plot_y+GRAPH_BORDER_PX};
      
        if (SOLID_GRAPH)
          graphics_draw_line(ctx, p, (GPoint){plot_x, GRAPH_Y_PX+GRAPH_BORDER_PX});
        else
          graphics_draw_pixel(ctx, p);
    }
  }
}



  //
  // entry point - drawing callback
  //
void layer_update_callback(Layer *me, GContext* ctx) {
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  layer_update_callback()");

  // set stroke colour - here? every time? 
  graphics_context_set_stroke_color(ctx, colour_fg());
  
  draw_box(ctx);
  
  draw_tidepoints(ctx); 
  
  draw_sinewave(ctx);
    
}


    //
    //  LOGIC ======================================================
    //
void calc_graph_points (char (*p_state_buf)[8], char (*p_time_buf)[6]){
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  calc_graph_points()");
  int i;
  int count_input = 0, count_output = 0;
  int prev_mins;
  
  // reset globals
  for (i=0; i < 4; i++){
    draw_x[i] = draw_y[i] = 0;
  }
    
  
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  int time_now_mins = (tick_time->tm_hour * 60 ) + tick_time->tm_min;
  #ifdef DEBUG_TIME_NOW_MINS
    time_now_mins=DEBUG_TIME_NOW_MINS;
  #endif
  APP_LOG(APP_LOG_LEVEL_INFO, "      time_now_mins=%d",time_now_mins);

  // loop for each input 
  while (count_input < GRAPH_NUM_POINTS) {
      
      // Count mins from now to next tide hi/lo
      prev_mins = time_now_mins;
      int tidetime_mins = calc_mins (p_time_buf[count_input], &prev_mins);
      if (tidetime_mins < 0){ // error
          APP_LOG(APP_LOG_LEVEL_ERROR, "calc_mins_failed (%s)", p_time_buf[count_input]);
          return;
      }  
//       APP_LOG(APP_LOG_LEVEL_INFO, "tidetime_mins = %d", time_now_mins );
      
      // calculate x point
      int xpos_mins        = tidetime_mins - time_now_mins;
      int xpos_px_relative = GRAPH_X_PX * xpos_mins / GRAPH_X_MINS;
      APP_LOG(APP_LOG_LEVEL_INFO, "    xpos: mins = %d  px_rel = %d", xpos_mins, xpos_px_relative );
    
      // in the past? (possible but not likely if web server on top of things)
//       if (xpos_mins < 0 ){
//           APP_LOG(APP_LOG_LEVEL_INFO, " ..skipping tide input point (time_now_mins=%d, tidetime_mins=%d)",
//                   time_now_mins, tidetime_mins);
//         count_input++;
//         continue;
//       }
    
      // calculate y point
      int ypos_px_absolute = 0;
      if (!strcmp(p_state_buf[count_input],"hi")){
        ypos_px_absolute = GRAPH_BORDER_PX + 10;
        APP_LOG(APP_LOG_LEVEL_INFO, "    ypos: High Tide %s",p_state_buf[count_input]);
      } else 
      if (!strcmp(p_state_buf[count_input],"lo")){
        ypos_px_absolute = GRAPH_Y_PX - 10;
        APP_LOG(APP_LOG_LEVEL_INFO, "    ypos: Low Tide %s",p_state_buf[count_input]);
      } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "do_graph_calc state_buf is %s, expecting hi|lo ", p_state_buf[count_input]);
        return;
      }
      
      // set datapoint for draw routine   
      draw_x[count_output] = xpos_px_relative; 
      draw_y[count_output] = ypos_px_absolute;
      APP_LOG(APP_LOG_LEVEL_ERROR, "relative tidepoint set as (%d,%d)",draw_x[count_output], draw_y[count_output]);

      // increment counters
      count_input++;
      count_output++;
  }  
    
  layer_mark_dirty (s_graph_layer);
  
  APP_LOG(APP_LOG_LEVEL_INFO, "do_graph_calc() - exit ");
}






// convert HH:MM to elapsed mins since midnight
static int calc_mins (char *s_hhmm, int *i_prev_mins){
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  calc_mins(%s, %d)", s_hhmm, *i_prev_mins );

  int i_mins = -1;
  int i;
  char s_buf[2];
 
  
  if ( (i = strlen(s_hhmm)) != 5){
    APP_LOG(APP_LOG_LEVEL_ERROR, "calc_mins strlen is %d, expecting 5", i);
    return -1;
  }
  
  // hours  
  strncpy (s_buf, s_hhmm, 2);
  i_mins = atoi(s_buf) * 60;
  //mins
  strncpy (s_buf, s_hhmm+3, 2); 
//   APP_LOG(APP_LOG_LEVEL_INFO, "  calc_mins mins string is %s", s_hhmm+3 );
  i_mins = i_mins + atoi(s_buf);

  // tomorrow?  but allow 60 mins grace, in case we're a bit ahead of the webdata
  if ((*i_prev_mins - i_mins) > 60 ){
    i_mins = i_mins + MINS_IN_DAY;
  }
  *i_prev_mins = i_mins;
  
  APP_LOG(APP_LOG_LEVEL_INFO, "calc_mins->%d", i_mins );
  return i_mins;
}


