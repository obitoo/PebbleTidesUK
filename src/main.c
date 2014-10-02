#include <pebble.h>
#include <graph.h>
  
  
static void update_time();
GColor colour_fg();
GColor colour_bg();

extern void calc_graph_points          (char (*p_state_buf)[8], char (*p_time_buf)[6], int *p_height_buf);
extern void print_tide_text_layers (char (*p_state_buf)[8], char (*p_time_buf)[6], int *p_height_buf);
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

static void process_js_data(DictionaryIterator *iterator, void *context);
static char *translate_error(AppMessageResult result);

static char state_buf[4][8];   // "hi" | "lo"
static char time_buf[4][6];    // "23:44"
static int  height_buf[5];    // "56"  = 5.6m 

    //
    //  Callbacks ======================================================
    //
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped: %i - %s", reason, translate_error(reason));
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed: %i - %s", reason, translate_error(reason));
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
  
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "inbox_received_callback() - entry" );
  process_js_data(iterator, context);
  print_tide_text_layers(state_buf, time_buf, height_buf);
  calc_graph_points(state_buf, time_buf, height_buf);
  // update graphics
  layer_mark_dirty (s_graph_layer);
}
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
  s_tideheight_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);


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
      APP_LOG(APP_LOG_LEVEL_INFO, "tick_handler() - entry" );
  
      update_time();
  
      // Get tide data from phone every 30 minutes
      if(tick_time->tm_min % TIDE_PHONE_POLL_MINS == 0) {
        // Begin dictionary
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
      
        // Add a key-value pair
        dict_write_uint8(iter, 0, 0);
      
        // Send the message!
        app_message_outbox_send();
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

//
//  Callback logic  - JS
//
static void process_js_data(DictionaryIterator *iterator, void *context){
  APP_LOG(APP_LOG_LEVEL_INFO, "process_js_data() - entry" );

  
  int i;
   
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  i=0;
  while(t != NULL) {
    // Which key was received?
    APP_LOG(APP_LOG_LEVEL_INFO, "SWITCH %d ",i++ );
    switch(t->key) {
        case   KEY_STATE_0:
               snprintf(state_buf[0], sizeof(state_buf[0]), "%s", t->value->cstring);
               break;
        case   KEY_TIME_0:
               snprintf(time_buf[0], sizeof(time_buf[0]), "%s", t->value->cstring);
               break;
        case   KEY_STATE_1:
               snprintf(state_buf[1], sizeof(state_buf[1]), "%s", t->value->cstring);
               break;
        case   KEY_TIME_1:
               snprintf(time_buf[1], sizeof(time_buf[1]), "%s", t->value->cstring);
               break;
        case   KEY_STATE_2:
               snprintf(state_buf[2], sizeof(state_buf[2]), "%s", t->value->cstring);
               break;
        case   KEY_TIME_2:
               snprintf(time_buf[2], sizeof(time_buf[2]), "%s", t->value->cstring);
               break;
        case   KEY_STATE_3:
               snprintf(state_buf[3], sizeof(state_buf[3]), "%s", t->value->cstring);
               break;
        case   KEY_TIME_3:
               snprintf(time_buf[3], sizeof(time_buf[3]), "%s", t->value->cstring);
               break;
      
        case   KEY_HEIGHT_0:
               height_buf[0] = atoi(t->value->cstring);
               break;
        case   KEY_HEIGHT_1:
               height_buf[1] = atoi(t->value->cstring);
               break;
        case   KEY_HEIGHT_2:
               height_buf[2] = atoi(t->value->cstring);
               break;
        case   KEY_HEIGHT_3:
               height_buf[3] = atoi(t->value->cstring);
               break;
      
        default:
               APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
               break;
    }
    // Look for next item
    t = dict_read_next(iterator);
  }
 


  
  APP_LOG(APP_LOG_LEVEL_INFO, "process_js_data()-exit");
}



static char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}







// EOF