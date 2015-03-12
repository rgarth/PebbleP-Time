#include <pebble.h>

static Window *s_main_window; 
GBitmap *full_bitmap, *empty_bitmap;
static BitmapLayer *p_100_layer, *p_75_layer, *p_50_layer, *p_25_layer;
static TextLayer *h_text_layer, *m_text_layer;

void show_battery_stats (){
  int8_t charge;
  charge = battery_state_service_peek().charge_percent;
  APP_LOG(APP_LOG_LEVEL_INFO, "Current Battery Charge: %i%%", charge);
  
  bitmap_layer_set_bitmap(p_100_layer, full_bitmap);
  bitmap_layer_set_bitmap(p_75_layer, full_bitmap);
  bitmap_layer_set_bitmap(p_50_layer, full_bitmap);
  bitmap_layer_set_bitmap(p_25_layer, full_bitmap);
 
  if (charge < 80) {
    bitmap_layer_set_bitmap(p_100_layer, empty_bitmap);
  }
  if (charge < 55){
    bitmap_layer_set_bitmap(p_75_layer, empty_bitmap);
  }
  if (charge < 30){
    bitmap_layer_set_bitmap(p_50_layer, empty_bitmap);
  }  
  if (charge <= 10) {
    // Show battery as empty
    bitmap_layer_set_bitmap(p_25_layer, empty_bitmap);    
  } 
          
}

static void show_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char h_buffer[] = "00";
  static char m_buffer[] = "00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(h_buffer, sizeof(h_buffer), "%H", tick_time);
  } else {
    // Use 12 hour format
    strftime(h_buffer, sizeof(h_buffer), "%l", tick_time);
  }
  strftime(m_buffer, sizeof(m_buffer), "%M", tick_time);
  
  text_layer_set_text(h_text_layer, h_buffer);
  text_layer_set_text(m_text_layer, m_buffer);
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  show_time();
  
  // Check battery stats every 30 minutes
  if(tick_time->tm_min % 30 == 0){
    show_battery_stats();
  }

}

static void bt_handler(bool connected) {
  if (connected) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone has connected!");
    vibes_short_pulse();
    
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone has disconnected!");
    vibes_double_pulse(); 
  }
  
}

static void main_window_load(Window *window) {
  GFont custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NORDOCA_ADVANCED_REGULAR_EXTENDED_52)); 

  // Create TextLayers for time
  h_text_layer = text_layer_create(GRect(0,  18, 114, 54));
  m_text_layer = text_layer_create(GRect(0, 74, 114, 54));
  text_layer_set_background_color(h_text_layer, GColorBlack);
  text_layer_set_background_color(m_text_layer, GColorBlack);  
  text_layer_set_text_color(h_text_layer, GColorWhite);
  text_layer_set_text_color(m_text_layer, GColorWhite);
  text_layer_set_font(h_text_layer, custom_font);
  text_layer_set_font(m_text_layer, custom_font);
  text_layer_set_text_alignment(h_text_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(m_text_layer, GTextAlignmentRight);
  text_layer_set_text(h_text_layer, "00");
  text_layer_set_text(m_text_layer, "00");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(h_text_layer)); 
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(m_text_layer)); 

  // Load bitmaps into GBitmap structures
  full_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FULL);
  empty_bitmap = gbitmap_create_with_resource(RESOURCE_ID_EMPTY);
  
  // Create BitmapLayers to show GBitmaps and add to Window
  // Using 4 bitmaps to show battery levels
  p_100_layer = bitmap_layer_create(GRect(113, 152, 8, 8));
  bitmap_layer_set_background_color(p_100_layer, GColorBlack); 
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(p_100_layer));

  p_75_layer = bitmap_layer_create(GRect(83, 152, 8, 8));
  bitmap_layer_set_background_color(p_75_layer, GColorBlack); 
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(p_75_layer));

  p_50_layer = bitmap_layer_create(GRect(53, 152, 8, 8));
  bitmap_layer_set_background_color(p_50_layer, GColorBlack); 
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(p_50_layer));

  p_25_layer = bitmap_layer_create(GRect(23, 152, 8, 8));
  bitmap_layer_set_background_color(p_25_layer, GColorBlack); 
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(p_25_layer));
  
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  bitmap_layer_destroy(p_100_layer);
  bitmap_layer_destroy(p_75_layer);
  bitmap_layer_destroy(p_50_layer);
  bitmap_layer_destroy(p_25_layer);
  text_layer_destroy(h_text_layer);
  text_layer_destroy(m_text_layer);
} 

static void init () {
  
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();  
  window_set_background_color(s_main_window, GColorBlack); 

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  show_time();
  show_battery_stats();
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register with the BluetoothConnectionService
  bluetooth_connection_service_subscribe(bt_handler); 
}

static void deinit () {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}