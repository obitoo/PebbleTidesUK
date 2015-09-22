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
  
static void update_time();
GColor colour_fg();
GColor colour_bg();


extern void gfx_layer_update_callback      (Layer *, GContext* );


static Window      *s_main_window;
static TextLayer   *s_time_layer;
static TextLayer   *s_date_layer;
       TextLayer   *s_tidetimes_text_layer;
       TextLayer   *s_tideheight_text_layer1;
       TextLayer   *s_tideheight_text_layer2;
       TextLayer   *s_portname_text_layer;
static GFont        s_time_font;
static GFont        s_date_font;
static GFont        s_tidetime_font;
static GFont        s_tideheight_font;
static GFont        s_portname_font;

       Layer       *s_graph_layer;
//static GBitmap     *s_background_bitmap;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void mainwindow_load(Window *window);
static void mainwindow_unload(Window *window);




extern void inbox_dropped_callback(AppMessageResult , void *);
extern void outbox_failed_callback(DictionaryIterator *, AppMessageResult , void *);
extern void outbox_sent_callback(DictionaryIterator *, void *);
extern void inbox_received_callback(DictionaryIterator *, void *);
extern void message_send_outbox();
extern int  messaging_ready();

extern void cache_init();
extern void cache_deinit();


    //
    //  Init ======================================================
    //
static void init() {
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Create GFontieees.  Using Bold when black on white background
  s_time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  s_date_font =    fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_tidetime_font =    fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_portname_font =   fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);//FONT_KEY_ROBOTO_CONDENSED_21 );  // fonts_get_system_font(FONT_KEY_GOTHIC_18);
  s_tideheight_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);


  // Create window, handlers
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
                    .load = mainwindow_load,
                    .unload = mainwindow_unload
  });
  window_stack_push(s_main_window, true);
  

  
  // Register callbacks..
  app_message_register_inbox_received(inbox_received_callback);
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  // Register callbacks..
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // persistent storage
  cache_init();
  

}

static void deinit() {
  window_destroy(s_main_window);
  cache_deinit();
}

int main (void){
  init();
  app_event_loop();
  deinit();
}


    //
    //  LAYOUT ======================================================
    //
#define FG_COL  GColorWhite
#define FG_COL_DIM GColorElectricBlue  
   
GColor colour_fg(){
    #ifdef PBL_COLOR
        if (config_get_bool(CFG_INVERT_COL)) 
          return GColorBlack;
        else
          return FG_COL;
    #else
       if (config_get_bool(CFG_INVERT_COL))
          return GColorBlack;
       else
          return GColorWhite;
    #endif
}

GColor colour_fg_dim(){
    #ifdef PBL_COLOR
      if (config_get_bool(CFG_INVERT_COL)) 
      return GColorBlack;
        else
      return FG_COL_DIM;
        
    #else
       if (config_get_bool(CFG_INVERT_COL))
          return GColorBlack;
       else
          return GColorWhite;
    #endif
}

 


GColor colour_bg(){
    #ifdef PBL_COLOR
      if (config_get_bool(CFG_INVERT_COL))
          return GColorWhite;
      else
          return GColorBlack;
    #else
      if (config_get_bool(CFG_INVERT_COL))
        return GColorWhite;
      else
        return GColorBlack;
    #endif
}


void main_hide_heights_layer(){
  if (config_get_bool(CFG_SHOW_HEIGHTS)){
      layer_set_hidden((Layer *)s_tideheight_text_layer1, false);
      layer_set_hidden((Layer *)s_tideheight_text_layer2, false);
  } else {
      layer_set_hidden((Layer *)s_tideheight_text_layer1, true);
      layer_set_hidden((Layer *)s_tideheight_text_layer2, true); 
  }
}

void main_set_colours(){
 // APP_LOG(APP_LOG_LEVEL_INFO, "main_set_colours() " );

  window_set_background_color(s_main_window, colour_bg());

  text_layer_set_background_color(s_tidetimes_text_layer, colour_bg());
  text_layer_set_text_color(s_tidetimes_text_layer,  colour_fg());
  
  text_layer_set_background_color(s_portname_text_layer, colour_bg());
  text_layer_set_text_color(s_portname_text_layer,  colour_fg());
    
  text_layer_set_background_color(s_tideheight_text_layer2, colour_bg());
  text_layer_set_text_color(s_tideheight_text_layer2,       colour_fg_dim());
  text_layer_set_background_color(s_tideheight_text_layer1, colour_bg());
  text_layer_set_text_color(s_tideheight_text_layer1,       colour_fg_dim());
  
  text_layer_set_background_color(s_time_layer, colour_bg());
  text_layer_set_text_color(s_time_layer, colour_fg());
  
  text_layer_set_background_color(s_date_layer, colour_bg());  
  text_layer_set_text_color(s_date_layer, colour_fg());
}

static void mainwindow_load(Window *window) {

  // Create graph layer
  s_graph_layer = layer_create(GRect(0, 0, 144, 75));
      // callback fn:
  layer_set_update_proc(s_graph_layer, gfx_layer_update_callback);

        // screen is 144 x 168  ------------------------
  // Create text times Layer
  s_tidetimes_text_layer = text_layer_create(GRect(0, GRAPH_Y_PX + GRAPH_BORDER_PX , 144, 50));
  text_layer_set_text(s_tidetimes_text_layer, "Loading...");
  text_layer_set_font(s_tidetimes_text_layer, s_tidetime_font);
  text_layer_set_text_alignment(s_tidetimes_text_layer, GTextAlignmentLeft);
  
  // Portname Layer - left aligned with date 
  s_portname_text_layer = text_layer_create(GRect(0, GRAPH_Y_PX + GRAPH_BORDER_PX + 40, 139, 21));  // 36,139,26 >>> for 21 font
  text_layer_set_text(s_portname_text_layer, "");
  text_layer_set_font(s_portname_text_layer, s_portname_font);
  text_layer_set_text_alignment(s_portname_text_layer, GTextAlignmentRight);
   
  //   date   
  s_date_layer = text_layer_create(GRect(5, 100, 139, 30));
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  text_layer_set_text(s_date_layer, "Today 10");
  
  //   time  
  s_time_layer = text_layer_create(GRect(5, 118, 139, 50));
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  
  //   heights - 3/4 width graph only  
  s_tideheight_text_layer1 = text_layer_create(GRect(102 +GRAPH_BORDER_PX, 0, MAX_X, GRAPH_Y_PX + 40));
  text_layer_set_font(s_tideheight_text_layer1, s_tideheight_font);
  text_layer_set_text_alignment(s_tideheight_text_layer1, GTextAlignmentLeft);
  s_tideheight_text_layer2 = text_layer_create(GRect(102 +GRAPH_BORDER_PX, GRAPH_Y_PX - 15 , MAX_X, GRAPH_Y_PX + 20));
  text_layer_set_font(s_tideheight_text_layer2, s_tideheight_font);
  text_layer_set_text_alignment(s_tideheight_text_layer2, GTextAlignmentLeft);
  
  // set colours - inverted ornot
  main_set_colours();
  
    
  // Add as child layers to the Window's root layer
  layer_add_child(window_get_root_layer(window), s_graph_layer);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_tideheight_text_layer1));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_tideheight_text_layer2));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_tidetimes_text_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_portname_text_layer));


  // hide if 3/4 width
  main_hide_heights_layer();
}



static void mainwindow_unload(Window *window) {

  // Destroy textlayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_tidetimes_text_layer);
  text_layer_destroy(s_tideheight_text_layer1);
  text_layer_destroy(s_tideheight_text_layer2);
  
  // Destroy fonts
//   fonts_unload_custom_font(s_tidetime_font);
//   fonts_unload_custom_font(s_time_font);

  // Destroy Graph Layer
   layer_destroy(s_graph_layer);
}




  //
  //  Callback logic - time
  //
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
      static int started_at = -1;  // 0 to 9
  
      APP_LOG(APP_LOG_LEVEL_INFO, "main.c:tick_handler() started_at = %d", started_at );
      APP_LOG(APP_LOG_LEVEL_INFO, "main.c:tick_handler() messaging_ready() = %d", messaging_ready() );


      if (started_at == -1)
         started_at = tick_time->tm_min % 10;

      update_time();
  
      // Get tide data from phone every few minutes  
      if((tick_time->tm_min % TIDE_PHONE_POLL_MINS == started_at) && messaging_ready()){
        message_send_outbox();
      }
}

static void update_time() {
      // Get a tm structure
      time_t temp = time(NULL); 
      struct tm *tick_time = localtime(&temp);
    
      // Create a long-lived buffer
      static char buffer[] = "00:00";
      static char dateString[] = "wednesday 99 ";
  
      // Write the current hours and minutes into the buffer
      if(clock_is_24h_style() == true) {
        // Use 24 hour format
        strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
      } else {
        // Use 12 hour format
        strftime(buffer, sizeof("00:00"), "%l:%M", tick_time);
      }
    
      //date
      strftime(dateString, sizeof(dateString), "%A %e ", tick_time);
    
      // update layers
      text_layer_set_text(s_time_layer, buffer);
      text_layer_set_text(s_date_layer, dateString);
}








// EOF
