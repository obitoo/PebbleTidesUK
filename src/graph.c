#include <pebble.h> 
#include <graph.h> 
#include <config.h>   
 
// TODO  - fix sine logic

static int calc_mins (char *, int *);
extern GColor colour_fg();
extern GColor colour_bg();
  
static int draw_x[] = {0,0,0,0,0};  // relative  // GRAPH_NUM_POINTS
static int draw_y[] = {0,0,0,0,0};
static int g_got_tides = 0;

extern  TextLayer   *s_tidetimes_text_layer;
extern  TextLayer   *s_tideheight_text_layer1;
extern  TextLayer   *s_tideheight_text_layer2;



static void plot_one_wave(GContext*, int, int, int , int , int , int);
static int calc_y_range (char (*p_state_buf)[8],  int *p_height_buf, int *min_y, int *max_y);
static int calc_localtime_mins();

static void draw_box(GContext* ctx);
static void draw_tidepoints(GContext* ctx);
static void draw_sinewave (GContext* ctx);


static void print_tidetimes  (char (*p_state)[8], char (*p_time)[6]);
static void print_tideheights(char (*p_state)[8], int *p_height, char *);

    




  //
  // entry points  ==============================
  //
void gfx_layer_update_callback(Layer *me, GContext* ctx) {
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  gfx_layer_update_callback()");

  if (1){
    // set stroke colour - here? every time? 
    graphics_context_set_stroke_color(ctx, colour_fg());
  
    draw_box(ctx);
    draw_tidepoints(ctx); 
    draw_sinewave(ctx);
    
    g_got_tides = 0;
  }  
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_exit:  gfx_layer_update_callback()");

}

void print_tide_text_layers (char (*p_state_buf)[8], char (*p_time_buf)[6], int *p_height_buf, char *time_str){
  
  // times
  print_tidetimes(p_state_buf, p_time_buf);
  
  // heights  
  if (GRAPH_NUM_POINTS  < 4) 
    print_tideheights (p_state_buf, p_height_buf, time_str);

}




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

    if (!config_bool(CFG_LINE_GRAPH))
      return;  
  
    graphics_context_set_fill_color(ctx, colour_fg());

    int i;  
    for (i=0; i< GRAPH_NUM_POINTS ; i++)
    if ((draw_x[i] > 0) && (draw_y[i] > 0)){
       graphics_fill_circle(ctx, (GPoint){draw_x[i] + GRAPH_BORDER_PX , draw_y[i]} , 3);
       APP_LOG(APP_LOG_LEVEL_INFO, "          draw_tidepoints, abs:(%d,%d)  ", draw_x[i], draw_y[i] );
    }

}

static void print_tidetimes(char (*p_state)[8], char (*p_time)[6]){
    APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  print_tidetimes()");

    static char text_layer_buffer[128];
  
    int i = 0;
    for (; i < GRAPH_NUM_POINTS; i++)
      APP_LOG(APP_LOG_LEVEL_INFO, "           print_tidetimes(%d) %d state=%s, time=%s",GRAPH_NUM_POINTS,i,p_state[i], p_time[i]);

  
    switch (GRAPH_NUM_POINTS){
      case 4:
        snprintf(text_layer_buffer, sizeof(text_layer_buffer), 
                 "%s: %s         %s: %s       \n       %s: %s         %s: %s\n",
                 p_state[0], p_time[0] ,           p_state[2], p_time[2],
                 p_state[1], p_time[1] ,           p_state[3], p_time[3]);
        break;
      case 3:
        snprintf(text_layer_buffer, sizeof(text_layer_buffer), 
                 "%s: %s    %s: %s\n         %s: %s       ",
                 p_state[0], p_time[0] ,           p_state[2], p_time[2],
                 p_state[1], p_time[1]  );
        break;
      default:
        strcpy (text_layer_buffer, "--error in print_tidetimes() --");
        break;     
    }
 
    APP_LOG(APP_LOG_LEVEL_INFO, "text_layer_set_text: ");
    APP_LOG(APP_LOG_LEVEL_INFO, "(%s) ", text_layer_buffer);
   
    text_layer_set_text(s_tidetimes_text_layer, text_layer_buffer);


}

static void print_tideheights(char (*p_state)[8], int *p_height, char *p_timestr){
    APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  print_tideheights()");
    static char text_layer_buffer1[32];
    static char text_layer_buffer2[32];
  
    int min_h, max_h;
    calc_y_range(p_state, p_height, &min_h, &max_h);

      // 3 lines - hi, lo and something in between (spring notification?):
      // need 2 layers to get the vertical alignment right
    char TODO_info[] = "      ";
//       char TODO_info[] = "Spring";
    strcpy (TODO_info, p_timestr);
      
    snprintf(text_layer_buffer1, 
             sizeof(text_layer_buffer1),
            " %d.%dm\n  %s", 
             max_h/10, max_h % 10, TODO_info);
    snprintf(text_layer_buffer2, 
             sizeof(text_layer_buffer2),
            " %d.%dm\n", 
             min_h/10, min_h % 10);
  
    APP_LOG(APP_LOG_LEVEL_INFO, "    heights:  %sm  ", text_layer_buffer1);
    APP_LOG(APP_LOG_LEVEL_INFO, "    heights:  %sm  ", text_layer_buffer2);
    text_layer_set_text(s_tideheight_text_layer1, text_layer_buffer1);
    text_layer_set_text(s_tideheight_text_layer2, text_layer_buffer2);
  
    APP_LOG(APP_LOG_LEVEL_INFO, "fn_exit:  print_tideheights()");
}
  
  


// draw sine wave,lamely. Would have been much easier when i was 17 studying maths ;) 
// todo - cycles thru all points properly, if first in the past
static void draw_sinewave (GContext* ctx){
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  draw_sinewave()");

  int range_y = GRAPH_Y_PX /2 ;
  int range_x = GRAPH_X_LOGICAL_MAX /2 ;
  

  // plut two 'full' waves, of 0->pi each
  int x_offset = draw_x[0] - range_x/4;
  APP_LOG(APP_LOG_LEVEL_INFO, "         draw_sinewave, draw_x[0] =%d, x_offset = %d ", draw_x[0], x_offset );
  int flip_y = ((draw_y[0] < (range_y))? -1 : 1);
    
      // wave 1 - lhs graph
  plot_one_wave (ctx, (0-x_offset), range_x, draw_x[1], draw_x[0], flip_y, x_offset);
  
      // wave 2 - rhs
  plot_one_wave (ctx, range_x,   GRAPH_X_LOGICAL_MAX-x_offset, draw_x[3], draw_x[2], flip_y, x_offset);
  
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_exit:    draw_sinewave()");
}


static void plot_one_wave(GContext* ctx, int xrel_from, int xrel_to, int x1, int x2, int flip_y, int x_offset){
  APP_LOG(APP_LOG_LEVEL_INFO, "plot_one_wave %d, %d, %d, %d, %d, %d ", xrel_from, xrel_to, x1, x2, flip_y, x_offset );

  int     x_rel, plot_x = 0, plot_y;
  int32_t xd, yd;

  int     range_x = GRAPH_X_LOGICAL_MAX /2 ;
  int     range_y = GRAPH_Y_PX /2 ;
  int     f = x1 - x2;
  
  int     line_graph = config_bool(CFG_LINE_GRAPH);

  for (x_rel = xrel_from; x_rel <= xrel_to; x_rel++){

    xd = TRIG_MAX_ANGLE * (x_rel-range_x) ;
    yd = sin_lookup(xd /(2*f)); 
//     APP_LOG(APP_LOG_LEVEL_WARNING, "      xd = %d, yd = %d", (int)xd, (int) yd);

    // amplitude : (GRAPH_Y_PX - 20)/GRAPH_Y_PX
    plot_y =  flip_y * ((GRAPH_Y_PX - 20) * (range_y * yd/TRIG_MAX_RATIO))/GRAPH_Y_PX + range_y  ;

    // draw                 0 >--------------< 144
    //        GRAPH_BORDER_PX  >------------<  GRAPH_BORDER_PX + 136
    plot_x = x_rel + GRAPH_BORDER_PX + x_offset ;
        // only plot if within x-window:
    if ((plot_x > GRAPH_BORDER_PX) && (plot_x < GRAPH_X_PX + GRAPH_BORDER_PX)) {
        GPoint p = (GPoint){plot_x, plot_y+GRAPH_BORDER_PX};
      
        if (line_graph)
           graphics_draw_pixel(ctx, p);
        else
           graphics_draw_line(ctx, p, (GPoint){plot_x, GRAPH_Y_PX+GRAPH_BORDER_PX});

    }
  }
//   APP_LOG(APP_LOG_LEVEL_INFO, "   last plot: xrel = %d, x=%d",x_rel, plot_x);

}






    //
    //  LOGIC ======================================================
    //
void calc_graph_points (char (*p_state_buf)[8], char (*p_time_buf)[6], int *p_height_buf){
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  calc_graph_points()");
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

  APP_LOG(APP_LOG_LEVEL_INFO, "      time_now_mins=%d",time_now_mins);

  
  // calculate height range
  int min_h, max_h;
  int h_range = calc_y_range(p_state_buf, p_height_buf, &min_h, &max_h);
  APP_LOG(APP_LOG_LEVEL_INFO, "       miny=%d, maxy=%d, range =%d",min_h, max_h, h_range);
  
  // loop for each input 
  while (count_input < NUM_TIDES_BACKGROUND) {
      
      // Count mins from now to next tide hi/lo
      int tidetime_mins = calc_mins (p_time_buf[count_input], &prev_mins);
      if (tidetime_mins < 0){ // error
          APP_LOG(APP_LOG_LEVEL_ERROR, "calc_mins_failed (%s)", p_time_buf[count_input]);
          return;
      }  
//       APP_LOG(APP_LOG_LEVEL_INFO, "tidetime_mins = %d", time_now_mins );
      
      //  x point
      int xpos_mins        = tidetime_mins - time_now_mins;
      int xpos_px_relative = GRAPH_X_PX * xpos_mins / GRAPH_X_MINS;
      APP_LOG(APP_LOG_LEVEL_INFO, "        xpos: mins = %d  px_rel = %d", xpos_mins, xpos_px_relative );
    

    
      //  y point
      int ypos_px_absolute = 0;
      int hm = p_height_buf[count_input];
      APP_LOG(APP_LOG_LEVEL_INFO, "       ypos: Height=%d",p_height_buf[count_input]);

      if (!strcmp(p_state_buf[count_input],"hi")){
        ypos_px_absolute =  0 +         GRAPH_BORDER_PX + GRAPH_Y_LOWPOINT + (max_h - hm) * GRAPH_EXAGGERATE_Y ;
      } else 
      if (!strcmp(p_state_buf[count_input],"lo")){
        ypos_px_absolute = GRAPH_Y_PX + GRAPH_BORDER_PX - GRAPH_Y_LOWPOINT - (hm - min_h) * GRAPH_EXAGGERATE_Y;
      } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "do_graph_calc state_buf is %s, expecting hi|lo ", p_state_buf[count_input]);
        return;
      }
      APP_LOG(APP_LOG_LEVEL_INFO, "       ypos: %s Tide = %dm",p_state_buf[count_input], hm );

    
      // set datapoint for draw routine   
      draw_x[count_output] = xpos_px_relative; 
      draw_y[count_output] = ypos_px_absolute;
//     heavy..  APP_LOG(APP_LOG_LEVEL_WARNING, "relative tidepoint set as (%d,%d)",draw_x[count_output], draw_y[count_output]);

      // increment counters
      count_input++;
      count_output++;
  }  
    

  g_got_tides = 1;
  
  
  APP_LOG(APP_LOG_LEVEL_INFO, "do_graph_calc() - exit ");
}



static int calc_localtime_mins(){
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  int time_now_mins = (tick_time->tm_hour * 60 ) + tick_time->tm_min;
  #ifdef DEBUG_TIME_NOW_MINS
    time_now_mins=DEBUG_TIME_NOW_MINS;
  #endif
  return time_now_mins;
}


static int calc_y_range (char (*p_state_buf)[8], int *p_height_buf, int *min_y, int *max_y){ 
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:     calc_y_range()" );

  int i;
  *min_y=99, *max_y=-1;
  for (i = 0;  i < GRAPH_NUM_POINTS; i++){
      APP_LOG(APP_LOG_LEVEL_INFO, "        %d, h= %d",i, p_height_buf[i]  );

      if (!strcmp(p_state_buf[i],"lo") && p_height_buf[i] < *min_y)
        *min_y = p_height_buf[i];
      if (!strcmp(p_state_buf[i],"hi") && p_height_buf[i] > *max_y)
        *max_y = p_height_buf[i];
  }
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_exit:     calc_y_range()  min %d, max %d",*min_y,*max_y );

  return *max_y - *min_y;
}




// convert HH:MM to elapsed mins since midnight
static int calc_mins (char *s_hhmm, int *i_prev_mins){
  APP_LOG(APP_LOG_LEVEL_INFO, "fn_entry:  calc_mins(%s, %d)", s_hhmm, *i_prev_mins );

  int i_mins = -1;
  int i;
  char s_buf[2];
 
  
  if ( (i = strlen(s_hhmm)) != 5){
    APP_LOG(APP_LOG_LEVEL_ERROR, "          calc_mins strlen is %d, expecting 5", i);
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


