#include <pebble.h>

// A struct to hold our UI elements
static struct {
  Window *window;
  TextLayer *text_layer;
} main_data;

// Keep track of the last button pressed and the number of sequential clicks
static ButtonId s_last_button = BUTTON_ID_SELECT;
static int s_click_count = 0;

////////////////////////////////////////////////////////////////////////////////
// Click Handlers
//
// These functions just update the text layer with what happened.
////////////////////////////////////////////////////////////////////////////////

// --- UP Button (Single vs. Long) ---
static void prv_up_single_click_handler(ClickRecognizerRef recognizer, void *ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Up single click");

  if (s_last_button == BUTTON_ID_UP) {
    s_click_count++;
  } else {
    s_last_button = BUTTON_ID_UP;
    s_click_count = 1;
  }

  static char buf[16];
  snprintf(buf, sizeof(buf), "UP: %d", s_click_count);
  text_layer_set_text(main_data.text_layer, buf);
}

static void prv_up_long_click_handler(ClickRecognizerRef recognizer, void *ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Up long click");
  s_click_count = 0;
  text_layer_set_text(main_data.text_layer, "UP: Long");
}

// --- SELECT Button (Raw vs. Single vs. Long) ---
static void prv_select_raw_press_handler(ClickRecognizerRef recognizer, void *ctx) {
  // This fires IMMEDIATELY on press down
  s_click_count = 0;
  text_layer_set_text(main_data.text_layer, "SELECT: Raw Press");
}

static void prv_select_single_click_handler(ClickRecognizerRef recognizer, void *ctx) {
  // This fires on RELEASE, if it wasn't a long click
  if (s_last_button == BUTTON_ID_SELECT) {
    s_click_count++;
  } else {
    s_last_button = BUTTON_ID_SELECT;
    s_click_count = 1;
  }

  static char buf[16];
  snprintf(buf, sizeof(buf), "SELECT: %d", s_click_count);
  text_layer_set_text(main_data.text_layer, buf);
}

static void prv_select_long_click_handler(ClickRecognizerRef recognizer, void *ctx) {
  // This fires after being HELD for 500ms
  s_click_count = 0;
  text_layer_set_text(main_data.text_layer, "SELECT: Long");
}

// --- DOWN Button (Single only) ---
static void prv_down_single_click_handler(ClickRecognizerRef recognizer, void *ctx) {
  if (s_last_button == BUTTON_ID_DOWN) {
    s_click_count++;
  } else {
    s_last_button = BUTTON_ID_DOWN;
    s_click_count = 1;
  }

  static char buf[16];
  snprintf(buf, sizeof(buf), "DOWN: %d", s_click_count);
  text_layer_set_text(main_data.text_layer, buf);
}


////////////////////////////////////////////////////////////////////////////////
// Click Configuration
//
// This is where we tell the system which functions to call for which buttons.
////////////////////////////////////////////////////////////////////////////////

static void prv_click_config_provider(void *ctx) {
  // UP Button
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_single_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, 500, prv_up_long_click_handler, NULL);

  // SELECT Button
  // Note the order: Raw, then Single, then Long
  window_raw_click_subscribe(BUTTON_ID_SELECT, prv_select_raw_press_handler, NULL, NULL);
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_single_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 500, prv_select_long_click_handler, NULL);

  // DOWN Button
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_single_click_handler);
}


////////////////////////////////////////////////////////////////////////////////
// Window Loading / Unloading
////////////////////////////////////////////////////////////////////////////////

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer
  main_data.text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text(main_data.text_layer, "Press a button!");
  text_layer_set_text_alignment(main_data.text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(main_data.text_layer));
}

static void prv_window_unload(Window *window) {
  // Destroy the TextLayer
  text_layer_destroy(main_data.text_layer);
}


////////////////////////////////////////////////////////////////////////////////
// App Initialization / Deinitialization
////////////////////////////////////////////////////////////////////////////////

static void prv_initialize(void) {
  // Create main Window
  main_data.window = window_create();

  // Set click provider
  window_set_click_config_provider(main_data.window, prv_click_config_provider);
  
  // Set handlers for window load/unload
  window_set_window_handlers(main_data.window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(main_data.window, true);
}

static void prv_terminate(void) {
  // Destroy Window
  window_destroy(main_data.window);
}

// Entry point
int main(void) {
  prv_initialize();
  app_event_loop();
  prv_terminate();
}
