#include <pebble.h>
#include <graph.h>
  
  
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
static GFont        s_time_font;
static GFont        s_date_font;
static GFont        s_tidetime_font;
static GFont        s_tideheight_font;
       Layer       *s_graph_layer;
//static GBitmap     *s_background_bitmap;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void mainwindow_load(Window *window);
static void mainwindow_unload(Window *window);




       char *p_current_time;

extern void inbox_dropped_callback(AppMessageResult , void *);
extern void outbox_failed_callback(DictionaryIterator *, AppMessageResult , void *);
extern void outbox_sent_callback(DictionaryIterator *, void *);
extern void inbox_received_callback(DictionaryIterator *, void *);
extern void message_send_outbox();
extern int  messaging_ready();

  


    //
    //  Init ======================================================
    //
static void init() {
  APP_LOG(APP_LOG_LEVEL_INFO, "init()");
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Create GFontieees
  s_time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  s_tidetime_font =    fonts_get_system_font(FONT_KEY_GOTHIC_18);
  s_date_font =    fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_tideheight_font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);


  // Create window, handlers
  s_main_window = window_create();
  window_set_background_color(s_main_window, colour_bg());

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
}

static void deinit() {
    APP_LOG(APP_LOG_LEVEL_INFO, "deinit()");
  window_destroy(s_main_window);
}

int main (void){
  init();
  APP_LOG(APP_LOG_LEVEL_INFO, "app_event_loop()");
  app_event_loop();
  deinit();
}


    //
    //  LAYOUT ======================================================
    //

GColor colour_fg(){
  if (BLACK_ON_WHITE)
    return GColorBlack;
  else
    return GColorWhite;
}
GColor colour_bg(){
  if (BLACK_ON_WHITE)
    return GColorClear;
  else
    return GColorBlack;
}

static void mainwindow_load(Window *window) {

  // Create graph layer
  s_graph_layer = layer_create(GRect(0, 0, 144, 75));
      // callback fn:
  layer_set_update_proc(s_graph_layer, gfx_layer_update_callback);
  layer_add_child(window_get_root_layer(window), s_graph_layer);

        // screen is 144 x 168  ------------------------
  
  
  // Create text times Layer
  s_tidetimes_text_layer = text_layer_create(GRect(0, GRAPH_Y_PX + GRAPH_BORDER_PX , 144, 50));
  text_layer_set_background_color(s_tidetimes_text_layer, colour_bg());
  text_layer_set_text_color(s_tidetimes_text_layer,  colour_fg());
  text_layer_set_text(s_tidetimes_text_layer, "Loading...");
  text_layer_set_font(s_tidetimes_text_layer, s_tidetime_font);
  text_layer_set_text_alignment(s_tidetimes_text_layer, GTextAlignmentLeft);

  //   date   
  s_date_layer = text_layer_create(GRect(5, 100, 139, 30));
  text_layer_set_background_color(s_date_layer, colour_bg()); // Clear? White? 
  text_layer_set_text_color(s_date_layer, colour_fg());
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  text_layer_set_text(s_date_layer, "Today 10");
  
  //   time  
    s_time_layer = text_layer_create(GRect(5, 118, 139, 50));

     text_layer_set_background_color(s_time_layer, colour_bg());
  text_layer_set_text_color(s_time_layer, colour_fg());
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  
  //   heights - half and 3/4 graph only  
  s_tideheight_text_layer1 = text_layer_create(GRect(GRAPH_X_PX+GRAPH_BORDER_PX, 4, MAX_X, GRAPH_Y_PX + 40));
   text_layer_set_background_color(s_tideheight_text_layer1, colour_bg());
  text_layer_set_text_color(s_tideheight_text_layer1, colour_fg());
  text_layer_set_font(s_tideheight_text_layer1, s_tideheight_font);
  text_layer_set_text_alignment(s_tideheight_text_layer1, GTextAlignmentLeft);
  
  s_tideheight_text_layer2 = text_layer_create(GRect(GRAPH_X_PX+GRAPH_BORDER_PX, GRAPH_Y_PX - 13 , MAX_X, GRAPH_Y_PX + 20));
  text_layer_set_background_color(s_tideheight_text_layer2, colour_bg());
  text_layer_set_text_color(s_tideheight_text_layer2, colour_fg());
  text_layer_set_font(s_tideheight_text_layer2, s_tideheight_font);
  text_layer_set_text_alignment(s_tideheight_text_layer2, GTextAlignmentLeft);
    
    
  // Add as child layers to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_tidetimes_text_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_tideheight_text_layer1));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_tideheight_text_layer2));

  
  APP_LOG(APP_LOG_LEVEL_INFO, "mainwindow_load() - exit " );
}



static void mainwindow_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_INFO, "mainwindow_unload() " );

  // Destroy textlayers
  APP_LOG(APP_LOG_LEVEL_INFO, "destroy s_timelayer " );

  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);

  APP_LOG(APP_LOG_LEVEL_INFO, "destroy s_weatherlayer " );

  text_layer_destroy(s_tidetimes_text_layer);
  text_layer_destroy(s_tideheight_text_layer1);
  text_layer_destroy(s_tideheight_text_layer2);
  
  // Destroy fonts

  APP_LOG(APP_LOG_LEVEL_INFO, "destroy fonts 1" );
//   fonts_unload_custom_font(s_tidetime_font);
//   APP_LOG(APP_LOG_LEVEL_INFO, "destroy fonts 2" );
//   fonts_unload_custom_font(s_time_font);
  

  // Destroy Graph Layer
   layer_destroy(s_graph_layer);
}




  //
  //  Callback logic - time
  //
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
      APP_LOG(APP_LOG_LEVEL_WARNING, "tick_handler() - time callback entry" );
  
      update_time();
  
      static int first_time = 1;
  
      // Get tide data from phone every few minutes  
      if(((tick_time->tm_min % TIDE_PHONE_POLL_MINS == 0) || first_time ) && messaging_ready()){
        
        first_time = 0;
        message_send_outbox();
        
      }
      APP_LOG(APP_LOG_LEVEL_INFO, "tick_handler() - exit" );
}

static void update_time() {
      // Get a tm structure
      time_t temp = time(NULL); 
      struct tm *tick_time = localtime(&temp);
    
      // Create a long-lived buffer
      static char buffer[] = "00:00";
      static char dateString[] = "wednesday 99 ";
  
      // for other fns to use
      p_current_time = buffer;  
     
      APP_LOG(APP_LOG_LEVEL_INFO, "update_time() - entry" );
    
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
  

      
      APP_LOG(APP_LOG_LEVEL_INFO, "update_time() - exit" );
}








// EOF