#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "http.h"
#include "util.h"
#include "weather_layer.h"
#include "time_layer.h"
#include "link_monitor.h"
#include "config.h"


// This is the Default APP_ID to work with old versions of httpebble (iOS)
#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x04, 0x9F, 0x49, 0xC0, 0x99, 0xAD }

// Use this one if you want to compile just for Android
//#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x29, 0x08, 0x7A, 0xB9, 0xB6, 0x19}


PBL_APP_INFO(MY_UUID,
             "Chunk Weather", "", // Modification of "Futura Weather" by Niknam Moslehi, which was modified "Roboto Weather" by Martin Rosinski
             1, 1, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define TIME_FRAME      (GRect(0, 81, 144, 54))
#define DATE_FRAME      (GRect(0, 138, 144, 30))

Window window;          /* main window */
BmpContainer background_image;

TextLayer date_layer;   /* layer for the date */
TimeLayer time_layer;   /* layer for the time */

GFont font_date;        /* font for date */
GFont font_hour;        /* font for hour */

static int initial_minute;

//Weather Stuff
static int our_latitude, our_longitude;
static bool located = false;

WeatherLayer weather_layer;

void request_weather();

void failed(int32_t cookie, int http_status, void* context) {
	if(cookie == 0 || cookie == WEATHER_HTTP_COOKIE) {
		weather_layer_set_error(&weather_layer, http_status);
	}
	link_monitor_handle_failure(http_status);
	
	//Re-request the location and subsequently weather on next minute tick
	located = false;
}

void success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) {
	if(cookie != WEATHER_HTTP_COOKIE) return;

  int16_t high = 0;
  int16_t low = 0;

  //Weather icon
	Tuple* icon_tuple = dict_find(received, WEATHER_KEY_ICON);
	if(icon_tuple) {
		int16_t icon = icon_tuple->value->int16;
		if(icon >= 0 && icon < 16) {
			weather_layer_set_icon(&weather_layer, icon);
		} else {
			weather_layer_set_error(&weather_layer, http_status);
		}
	}
  //Today's high temperature
  Tuple* high_tuple = dict_find(received, WEATHER_KEY_HIGH);
  if(high_tuple) {
    high = high_tuple->value->int16;
  }
  //Today's low temperature
  Tuple* low_tuple = dict_find(received, WEATHER_KEY_LOW);
  if(low_tuple) {
    low = low_tuple->value->int16;
  }
  //Current temperature
	Tuple* temperature_tuple = dict_find(received, WEATHER_KEY_TEMPERATURE);
	if(temperature_tuple) {
    weather_layer_set_highlow(&weather_layer, high, low);
		weather_layer_set_temperature(&weather_layer, temperature_tuple->value->int16);
	}
	
	link_monitor_handle_success();
}

void location(float latitude, float longitude, float altitude, float accuracy, void* context) {
	// Fix the floats
	our_latitude = latitude * 1000000;
	our_longitude = longitude * 1000000;
	located = true;
	request_weather();
}

void reconnect(void* context) {
	located = false;
	request_weather();
}

void request_weather();

/* Called by the OS once per minute. Update the time and date.
*/
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
    /* Need to be static because pointers to them are stored in the text
    * layers.
    */
    static char date_text[] = "XXX 00";
    static char hour_text[] = "00";
    static char minute_text[] = ":00";

    static char date_day[] = "XXX";
    static char date_monthday[] = "00";
    static char date_month[] = "XXX";
    static char date_year[] = "0000";
    static char full_date_text[20] = "";

    (void)ctx;  /* prevent "unused parameter" warning */

    if (t->units_changed & DAY_UNIT) {		
	    string_format_time(date_text,
                           sizeof(date_text),
                           "%a %d",
                           t->tick_time);

	    string_format_time(date_day,
                           sizeof(date_day),
                           "%a",
                           t->tick_time);

	    string_format_time(date_monthday,
                           sizeof(date_monthday),
                           "%d",
                           t->tick_time);

	    string_format_time(date_month,
                           sizeof(date_month),
                           "%b",
                           t->tick_time);

	    string_format_time(date_year,
                           sizeof(date_year),
                           "%Y",
                           t->tick_time);

		  if (date_text[4] == '0') /* is day of month < 10? */ {
		      /* This is a hack to get rid of the leading zero of the
			     day of month
              */
          memmove(&date_text[4], &date_text[5], sizeof(date_text) - 1);
		  }

      //removed broken date suffix
      //snprintf(full_date_text, sizeof(full_date_text), "%s %s%s %s", date_day, date_monthday, daySuffix(my_atoi(date_day)), date_month);

      snprintf(full_date_text, sizeof(full_date_text), "%s %s %s", date_day, date_monthday, date_month); 
      text_layer_set_text(&date_layer, full_date_text);
    }

    if (clock_is_24h_style()) {
      string_format_time(hour_text, sizeof(hour_text), "%H", t->tick_time);
		  if (hour_text[0] == '0') {
              /* This is a hack to get rid of the leading zero of the hour.
              */
              memmove(&hour_text[0], &hour_text[1], sizeof(hour_text) - 1);
      }
    }
    else {
        string_format_time(hour_text, sizeof(hour_text), "%I", t->tick_time);
        if (hour_text[0] == '0') {
            /* This is a hack to get rid of the leading zero of the hour.
            */
            memmove(&hour_text[0], &hour_text[1], sizeof(hour_text) - 1);
        }
    }

    string_format_time(minute_text, sizeof(minute_text), ":%M", t->tick_time);
    time_layer_set_text(&time_layer, hour_text, minute_text);
	
  if(!located || (t->tick_time->tm_min % FREQUENCY_MINUTES) == initial_minute)
  {
    //Every FREQUENCY_MINUTES, request updated weather
    http_location_request(); 
  }
  else
  {
	  //Every minute, ping the phone
	  link_monitor_ping();
  }
}


/* Initialize the application.
*/
void handle_init(AppContextRef ctx) {
  PblTm tm;
  PebbleTickEvent t;
  ResHandle res_d;
  ResHandle res_h;

  window_init(&window, "Chunk");
  window_stack_push(&window, true /* Animated */);

  resource_init_current_app(&APP_RESOURCES);

  if(SPLIT_MODE==1) {
    bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND_SPLIT, &background_image);
  }
  else {
    bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image);
  }
  layer_add_child(&window.layer, &background_image.layer.layer);

  res_d = resource_get_handle(RESOURCE_ID_SMALL_26);
  res_h = resource_get_handle(RESOURCE_ID_BIG_52);

  font_date = fonts_load_custom_font(res_d);
  font_hour = fonts_load_custom_font(res_h);

  time_layer_init(&time_layer, window.layer.frame);
  time_layer_set_text_color(&time_layer, COLOUR_TEXT_BOTTOM_SPLIT);
  time_layer_set_background_color(&time_layer, GColorClear);
  time_layer_set_fonts(&time_layer, font_hour, font_hour);
  layer_set_frame(&time_layer.layer, TIME_FRAME);
  layer_add_child(&window.layer, &time_layer.layer);

  text_layer_init(&date_layer, window.layer.frame);
  text_layer_set_text_color(&date_layer, COLOUR_TEXT_BOTTOM_SPLIT);
  text_layer_set_background_color(&date_layer, GColorClear);
  text_layer_set_font(&date_layer, font_date);
  text_layer_set_text_alignment(&date_layer, GTextAlignmentCenter);
  layer_set_frame(&date_layer.layer, DATE_FRAME);
  layer_add_child(&window.layer, &date_layer.layer);

	// Add weather layer
	weather_layer_init(&weather_layer);
	layer_add_child(&window.layer, &weather_layer.layer);
	
	http_register_callbacks((HTTPCallbacks){
    .failure=failed,
    .success=success,
    .reconnect=reconnect,
    .location=location
  }, (void*)ctx);
	
	// Refresh time
  get_time(&tm);
  t.tick_time = &tm;
  t.units_changed = MINUTE_UNIT | HOUR_UNIT | DAY_UNIT;

  initial_minute = (tm.tm_min % FREQUENCY_MINUTES);

	handle_minute_tick(ctx, &t);
}

/* Shut down the application
*/
void handle_deinit(AppContextRef ctx)
{
    fonts_unload_custom_font(font_date);
    fonts_unload_custom_font(font_hour);
    bmp_deinit_container(&background_image);
	  weather_layer_deinit(&weather_layer);
}


/********************* Main Program *******************/

void pbl_main(void *params)
{
    PebbleAppHandlers handlers =
    {
      .init_handler = &handle_init,
      .deinit_handler = &handle_deinit,
      .tick_info = {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
      },
		  .messaging_info = {
			  .buffer_sizes = {
				  .inbound = 124,
				  .outbound = 256,
			  }
		  }
    };

    app_event_loop(params, &handlers);
}

void request_weather() {
	if(!located) {
		http_location_request();
		return;
	}
	// Build the HTTP request
	DictionaryIterator *body;
	HTTPResult result = http_out_get(SERVER_URL, WEATHER_HTTP_COOKIE, &body);
	if(result != HTTP_OK) {
    weather_layer_set_error(&weather_layer, 9988);
		return;
	}
	dict_write_int32(body, WEATHER_KEY_LATITUDE, our_latitude);
	dict_write_int32(body, WEATHER_KEY_LONGITUDE, our_longitude);
	dict_write_cstring(body, WEATHER_KEY_UNIT_SYSTEM, UNIT_SYSTEM);
	// Send it.
	if(http_out_send() != HTTP_OK) {
    weather_layer_set_error(&weather_layer, 9989);
		return;
	}
}
