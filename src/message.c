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

extern void main_hide_heights_layer();

static int process_js_msg (DictionaryIterator *iterator, void *context);
static int js_tides       (DictionaryIterator *iterator, void *context);
static int js_config      (DictionaryIterator *iterator, void *context);
static int js_ready       (DictionaryIterator *iterator, void *context);
static char *translate_error(AppMessageResult result);


extern Layer       *s_graph_layer;

static int retry_count_out = 3;
static int js_initialised = 0; 

       void message_send_outbox();

extern void main_set_colours();



void cache_set_state    (int key, char *cstring);
void cache_set_time     (int key, char *cstring);
void cache_set_height   (int key, int intval);
void cache_set_cachekey (char* cstring);
void cache_set_refreshed();

//
//  Callback entry points ====================================================
//
void inbox_received_callback(DictionaryIterator *iterator, void *context) {
 APP_LOG(APP_LOG_LEVEL_WARNING, "inbox_received_callback() " );
   
  int update = process_js_msg(iterator, context);
  
     // update graphics
  if (update) {
      APP_LOG(APP_LOG_LEVEL_WARNING, "inbox_received_callback() - marking layer dirty" );
      layer_mark_dirty (s_graph_layer); // only place we do this
    
  }
 APP_LOG(APP_LOG_LEVEL_WARNING, "inbox_received_callback() - exit" );
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
 APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped: %i - %s", reason, translate_error(reason));
//   graph_data_stale_set(1);
  // TODO print_tide_text_layers(state_buf, time_buf, height_buf, appmsg_received_time, portname_buf); // 'Stale' msg
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
 APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed: %i - %s", reason, translate_error(reason));
  // retry 
  if (--retry_count_out > 0) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send RETRY");
     message_send_outbox();
  }
//   graph_data_stale_set(1);
//  TODO print_tide_text_layers(state_buf, time_buf, height_buf, appmsg_received_time, portname_buf); // 'Stale' msg
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
 // APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
  retry_count_out = 3;
}


//
//  Called from main 
//
int messaging_ready(){
  APP_LOG(APP_LOG_LEVEL_WARNING, "messaging ready? = %d",js_initialised);
  return js_initialised;
}


//
//  Pebble to phone - this requests a tide update, passes config at same time
//
void message_send_outbox() {
    APP_LOG(APP_LOG_LEVEL_INFO, "message_send_outbox() " );

    // Begin dictionary
    DictionaryIterator *iter = NULL;
    app_message_outbox_begin(&iter);

    // Add key-value pairs
    dict_write_cstring(iter, CFG_PORT,         config_get_string(CFG_PORT)); 
    dict_write_cstring(iter, CFG_INVERT_COL,   config_get_string(CFG_INVERT_COL)); 
    dict_write_cstring(iter, CFG_LINE_GRAPH,   config_get_string(CFG_LINE_GRAPH));
    dict_write_cstring(iter, CFG_SHOW_HEIGHTS, config_get_string(CFG_SHOW_HEIGHTS));
  
    dict_write_cstring(iter, CFG_PORTNAME,     config_get_string(CFG_PORTNAME));
    dict_write_int8   (iter, CFG_OFFSET,       config_get_intval(CFG_OFFSET));
    dict_write_cstring(iter, CFG_DST,          config_get_string(CFG_DST));
    dict_write_cstring(iter, CFG_SHOW_FEET,    config_get_string(CFG_SHOW_FEET));

  
    // Pass watch time and version no
    // (slight duplication of work in main.c - but i always want this in 24h fmt)
    time_t epoch = time(NULL); 
    struct tm *tick_time = localtime(&epoch);
  
    char hhmm[] = "00:00";
    strftime(hhmm, sizeof("00:00"), "%H:%M", tick_time);

    dict_write_cstring(iter, CFG_TIME,         hhmm      );  
    dict_write_cstring(iter, CFG_VERSION,      APPVERSION);
  
    // Pass the date (1-31) too
    char day[] = "00";  
    strftime(day, sizeof("00"), "%d", tick_time);
    dict_write_cstring(iter, CFG_DATE,         day      );  

    // Dont do this. Really. https://developer.getpebble.com/2/guides/app-phone-communication.html
    //dict_write_end (iter);

    // Send the message
    app_message_outbox_send();
   APP_LOG(APP_LOG_LEVEL_INFO, "message_send_outbox() - exit" );
}


//
//  Callback logic  - Javascript appmessage 
//
static int process_js_msg(DictionaryIterator *iterator, void *context){
  APP_LOG(APP_LOG_LEVEL_INFO, "process_js_msg()" );

  int update_gfx = 0;
  
  // debug
//   Tuple *tuple = dict_read_first(iterator);
//   while (tuple) {
  
//     APP_LOG(APP_LOG_LEVEL_WARNING, "tuple: %d, %s", (int)tuple->key, tuple->value->cstring);
//     tuple = dict_read_next(iterator);
//   }
  // \debug
  
  // Can't assume order
  //Tuple *t = dict_read_first(iterator);
  Tuple *t = dict_find(iterator, MSG_TYPE);
  if (t == NULL ){
       APP_LOG(APP_LOG_LEVEL_ERROR, "     Missing MSG_TYPE key" );
       return 0;
  }

  // route to different msg handlers
  if (!strcmp(t->value->cstring,"tides")){
     update_gfx = js_tides (iterator, context);
  } else 
  if (!strcmp(t->value->cstring,"config")){
     update_gfx = js_config (iterator, context);
  } else 
  if (!strcmp(t->value->cstring,"ready")){
     update_gfx = js_ready (iterator, context);
  } else {
     APP_LOG(APP_LOG_LEVEL_ERROR, "     unexpected MSG_TYPE value - %s", t->value->cstring );
  }
  
  APP_LOG(APP_LOG_LEVEL_INFO, "process_js_msg() - exit" );

  return update_gfx;
}
      
      
//
//  Message Handlers 
//  
static int js_tides(DictionaryIterator *iterator, void *context){
 APP_LOG(APP_LOG_LEVEL_INFO, "js_tides() - entry" );

  // Start again. Necessary? 
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    switch(t->key) {
        case   MSG_TYPE:
               break;
        case   KEY_STATE_0:
               cache_set_state (0, t->value->cstring);
               break;
        case   KEY_TIME_0:
               cache_set_time (0, t->value->cstring);
               break;
        case   KEY_STATE_1:
               cache_set_state (1, t->value->cstring);
               break;
        case   KEY_TIME_1:
               cache_set_time (1, t->value->cstring);
               break;
        case   KEY_STATE_2:
               cache_set_state (2, t->value->cstring);
               break;
        case   KEY_TIME_2:
               cache_set_time (2, t->value->cstring);
               break;
        case   KEY_STATE_3:
               cache_set_state (3, t->value->cstring);
               break;
        case   KEY_TIME_3:
               cache_set_time (3, t->value->cstring);
               break;
      
        case   KEY_HEIGHT_0:
               cache_set_height (0, atoi(t->value->cstring));
               break;
        case   KEY_HEIGHT_1:
               cache_set_height (1, atoi(t->value->cstring));
               break;
        case   KEY_HEIGHT_2:
               cache_set_height (2, atoi(t->value->cstring));
               break;
        case   KEY_HEIGHT_3:
               cache_set_height (3, atoi(t->value->cstring));
               break;

        case   KEY_PORTNAME:
               cache_set_cachekey (t->value->cstring);
               break;
        default:
              APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
               break;
    }
    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // only place we do this
  cache_set_refreshed();
    
 // APP_LOG(APP_LOG_LEVEL_INFO, "js_tides()-exit");
  
  return 1;
}


static int js_config(DictionaryIterator *iterator, void *context){
 APP_LOG(APP_LOG_LEVEL_INFO, "js_config() - entry" );
  
  // Start again. Necessary? 
  Tuple *t = dict_read_first(iterator);

  int tmp;
  // For all items
  while(t != NULL) {
    switch(t->key) {
      case CFG_SHOW_HEIGHTS:
            // APP_LOG(APP_LOG_LEVEL_INFO, "      cfg / Show heights: %s", (t->value->cstring));
             config_save_string(CFG_SHOW_HEIGHTS, t->value->cstring);
             // adjust layers a bit
             main_hide_heights_layer();
             break;
      case CFG_INVERT_COL:
            // APP_LOG(APP_LOG_LEVEL_INFO, "      cfg / Invert cols: %s", (t->value->cstring));
             config_save_string(CFG_INVERT_COL,   t->value->cstring);
             // redraw the colours - expensive? 
             main_set_colours();
             break;
      case CFG_LINE_GRAPH:
            // APP_LOG(APP_LOG_LEVEL_INFO, "       cfg / Line Graph: %s", (t->value->cstring));
             config_save_string(CFG_LINE_GRAPH,   t->value->cstring);
             break;
      case CFG_PORT:
            // APP_LOG(APP_LOG_LEVEL_INFO, "       cfg / Port: %s", (t->value->cstring));
             config_save_string(CFG_PORT,         t->value->cstring);
             break;
      case CFG_PORTNAME:
            // APP_LOG(APP_LOG_LEVEL_INFO, "       cfg / Show Portname: %s", (t->value->cstring));
             config_save_string(CFG_PORTNAME,     t->value->cstring);
             break;
      case CFG_OFFSET:
             tmp = atoi(t->value->cstring);            
            // APP_LOG(APP_LOG_LEVEL_INFO, "       cfg / Offset int: %d", tmp);
             config_save_int(CFG_OFFSET,     tmp);
             break;
      case CFG_DST:
            // APP_LOG(APP_LOG_LEVEL_INFO, "       cfg / DST: %s", (t->value->cstring));
             config_save_string(CFG_DST,     t->value->cstring);
             break;
      case CFG_SHOW_FEET:
            // APP_LOG(APP_LOG_LEVEL_INFO, "       cfg / FEET: %s", (t->value->cstring));
             config_save_string(CFG_SHOW_FEET,     t->value->cstring);
             break;
      
      case MSG_TYPE:
             break;
       default:
            // APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
             break;
    }
    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // enable other messaging - TODO - really?
  js_initialised = 1;
  
  // give a little buzz
  vibes_double_pulse();

    
 APP_LOG(APP_LOG_LEVEL_INFO, "js_config() - exit" );
  
  return 1;
}



static int js_ready(DictionaryIterator *iterator, void *context){
  APP_LOG(APP_LOG_LEVEL_INFO, "js_ready() - entry" );
    
  // consume msg. Necessary?
  Tuple *t; 
  do {
    t = dict_read_next(iterator);
  } while (t != NULL);
    
  js_initialised = 1;

  // Send tides request
  message_send_outbox();
   
 APP_LOG(APP_LOG_LEVEL_INFO, "js_ready() - exit" );
  return 0;
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
