#include <pebble.h>
#include <graph.h>

extern void calc_graph_points          (char (*p_state_buf)[8], char (*p_time_buf)[6], int *p_height_buf);
extern void print_tide_text_layers (char (*p_state_buf)[8], char (*p_time_buf)[6], int *p_height_buf, char *);

extern char*  p_current_time;

static void process_js_msg (DictionaryIterator *iterator, void *context);
static void js_tides       (DictionaryIterator *iterator, void *context);
static void js_config      (DictionaryIterator *iterator, void *context);
static void js_ready       (DictionaryIterator *iterator, void *context);
static char *translate_error(AppMessageResult result);

static char state_buf[4][8];   // "hi" | "lo"
static char time_buf[4][6];    // "23:44"
static int  height_buf[5];    // "56"  = 5.6m 

extern Layer       *s_graph_layer;

static char appmsg_received_time[]="00:00";

static int retry_count_out = 3;
static int js_initialised = 0; 

       void message_send_outbox();

//
//  Callback entry points ====================================================
//
void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_WARNING, "inbox_received_callback() - AppMsg entry" );
   
  process_js_msg(iterator, context);
  
     // update graphics
  print_tide_text_layers(state_buf, time_buf, height_buf, appmsg_received_time);
  calc_graph_points(state_buf, time_buf, height_buf);
  layer_mark_dirty (s_graph_layer);
  
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped: %i - %s", reason, translate_error(reason));
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed: %i - %s", reason, translate_error(reason));
  // retry 
  if (--retry_count_out > 0) {
     APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send RETRY");
     message_send_outbox();
  }
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
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
    APP_LOG(APP_LOG_LEVEL_INFO, "message_send_outbox() - entry" );
  
    // Begin dictionary
    DictionaryIterator *iter = NULL;
    app_message_outbox_begin(&iter);
  
    // Add a key-value pair
    dict_write_cstring(iter, CFG_PORT, "1234");
    //                 dict_write_uint8(iter, 0, "1234");

    // Dont do this. Really. https://developer.getpebble.com/2/guides/app-phone-communication.html
    //dict_write_end (iter);

    // Send the message!
    app_message_outbox_send();
}

//
//  Callback logic  - Javascript appmessage 
//
static void process_js_msg(DictionaryIterator *iterator, void *context){
  APP_LOG(APP_LOG_LEVEL_INFO, "process_js_msg() - entry" );

  //quick hack - remove later
  //strcpy(appmsg_received_time, p_current_time);
    
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // if key isnt msg_type we've got an ordering problem
  // doesnt work cos its only an int lol 
//   if (t->key != MSG_TYPE) {
//     APP_LOG(APP_LOG_LEVEL_ERROR, "     unexpected key - %d", (int)t->key );
//     return;
//   }
  
  
  
  // route to different msg handlers
  APP_LOG(APP_LOG_LEVEL_INFO, "process_js_msg() - key string=%s" ,t->value->cstring);

  if (!strcmp(t->value->cstring,"tides")){
     js_tides (iterator, context);
  } else 
  if (!strcmp(t->value->cstring,"config")){
     js_config (iterator, context);
  } else 
  if (!strcmp(t->value->cstring,"ready")){
     js_ready (iterator, context);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "     unexpected MSG_TYPE value - %s", t->value->cstring );
  }
  
  return;
}
      
      
//
//  Message Handlers 
//  
static void js_tides(DictionaryIterator *iterator, void *context){
  APP_LOG(APP_LOG_LEVEL_INFO, "js_tides() - entry" );

  // first item already read
  Tuple *t = dict_read_next(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    APP_LOG(APP_LOG_LEVEL_INFO, "SWITCH %d ",(int)t->key);
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
  
  APP_LOG(APP_LOG_LEVEL_INFO, "js_tides()-exit");
}


static void js_config(DictionaryIterator *iterator, void *context){
  APP_LOG(APP_LOG_LEVEL_INFO, "js_config() - entry" );
  
  Tuple *t = dict_read_next(iterator);

  // For all items
  while(t != NULL) {
    APP_LOG(APP_LOG_LEVEL_INFO, "SWITCH %d ",(int) t->key );
    switch(t->key) {
      case   CFG_SHOW_HEIGHTS:
               APP_LOG(APP_LOG_LEVEL_INFO, "      cfg / Show heights: %s", (t->value->cstring));
               break;
      case   CFG_INVERT_COL:
               APP_LOG(APP_LOG_LEVEL_INFO, "      cfg / Invert cols: %s", (t->value->cstring));
               break;
      case   CFG_SOLID_GRAPH:
              APP_LOG(APP_LOG_LEVEL_INFO, "      cfg / Solid Graph: %s", (t->value->cstring));
               break;
      
       default:
               APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
               break;
    }
    // Look for next item
    t = dict_read_next(iterator);
  }
  
  
  
  APP_LOG(APP_LOG_LEVEL_INFO, "js_config() - exit" );
}

static void js_ready(DictionaryIterator *iterator, void *context){
  APP_LOG(APP_LOG_LEVEL_INFO, "js_ready() - entry" );
    
  // consume msg
  Tuple *t; 
  do {
    t = dict_read_next(iterator);
  } while (t != NULL);
    
  js_initialised = 1;
   
  APP_LOG(APP_LOG_LEVEL_INFO, "js_ready() - exit" );
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