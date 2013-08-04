#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "http.h"
#include "util.h"
#include "weather_layer.h"
#include "link_monitor.h"
#include "config.h"

// This is the Default APP_ID to work with old versions of httpebble (iOS)
#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x04, 0x9F, 0x49, 0xC0, 0x99, 0xAD }

// Use this one if you want to compile just for Android
//#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x29, 0x08, 0x7A, 0xB9, 0xB6, 0x19}

PBL_APP_INFO(MY_UUID,
             "Chunk Weather", "", // Modification of "Futura Weather" by Niknam Moslehi, which was modified "Roboto Weather" by Martin Rosinski
             1, 4, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);


Window window;          /* main window */
Layer background_layer;

BmpContainer background_image;

InverterLayer invertBottom;

TextLayer date_layer, time_layer;   /* layer for the date & time */

GFont font_date, font_time;        /* font for date & time */

static int initial_minute;

void update_background_callback(Layer *layer, GContext *gctx) {
    (void) layer;
	graphics_context_set_stroke_color(gctx, GColorBlack);
	graphics_draw_line(gctx, GPoint(0, 86), GPoint(144, 86));		
}

//Weather Stuff
static int our_latitude, our_longitude;
static bool located = false;
static int16_t icon = 0;
static int16_t high = 0;
static int16_t low = 0;

WeatherLayer weather_layer;

void request_weather();

void handle_failed(int32_t cookie, int http_status, void* context) {
	if(cookie == 0 || cookie == WEATHER_HTTP_COOKIE) {
		weather_layer_set_error(&weather_layer, http_status);
	}
	link_monitor_handle_failure(http_status);	
	//request the location and weather on next minute tick
	located = false;
}

void handle_success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) {
	if(cookie != WEATHER_HTTP_COOKIE) return;

	//Weather icon
	Tuple* icon_tuple = dict_find(received, WEATHER_CODE);
	if(icon_tuple) {
		icon = icon_tuple->value->int16;
		if(icon >= 0 && icon <= 48) {
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

void handle_location(float latitude, float longitude, float altitude, float accuracy, void* context) {
	// Fix the floats
	our_latitude = latitude * 1000000;
	our_longitude = longitude * 1000000;
	located = true;
	request_weather();
}

void handle_reconnect(void* context) {
	located = false;
	request_weather();
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

    (void)ctx;
	
    static char hour_text[] = "00";
    static char minute_text[] = "00";	
	static char full_time_text[] = "00:00";

    static char date_day[] = "XXX";
    static char date_monthday[] = "00";
    static char date_month[] = "XXX";    
    static char full_date_text[20] = "";

	//DATE
    if (t->units_changed & DAY_UNIT) {
	    string_format_time(date_day,
                           sizeof(date_day),
                           "%a",
                           t->tick_time);

	    string_format_time(date_monthday,
                           sizeof(date_monthday),
                           "%d",
                           t->tick_time);
						   
        if (date_monthday[0] == '0') {
            memmove(&date_monthday[0], &date_monthday[1], sizeof(date_monthday) - 1); //remove leading zero
        }

	    string_format_time(date_month,
                           sizeof(date_month),
                           "%b",
                           t->tick_time);

      snprintf(full_date_text, sizeof(full_date_text), "%s %s %s", date_day, date_monthday, date_month); 
      text_layer_set_text(&date_layer, full_date_text);
    }
	
	if (t->units_changed & HOUR_UNIT && BUILD_STYLE==5) {
		if(t->tick_time->tm_hour<6 || t->tick_time->tm_hour>17) {
			//nightMode();
		}
	}

	//TIME
    if(clock_is_24h_style()) {
		string_format_time(hour_text, sizeof(hour_text), "%H", t->tick_time);
    }
    else {
        string_format_time(hour_text, sizeof(hour_text), "%I", t->tick_time);
        if (hour_text[0] == '0') {
            memmove(&hour_text[0], &hour_text[1], sizeof(hour_text) - 1); //remove leading zero
        }
    }
    string_format_time(minute_text, sizeof(minute_text), "%M", t->tick_time);	
	snprintf(full_time_text, sizeof(full_time_text), "%s:%s", hour_text, minute_text);	
	text_layer_set_text(&time_layer, full_time_text);


	if(!located || (t->tick_time->tm_min % FREQUENCY_MINUTES) == initial_minute) {
		//Every FREQUENCY_MINUTES, request updated weather
		http_location_request(); 
	}
	else {
		//Every minute, ping the phone
		link_monitor_ping();
	}
}


void handle_init(AppContextRef ctx) {
	PblTm tm;
	PebbleTickEvent t;
	ResHandle res_d, res_t;

	window_init(&window, "Chunk");
	window_stack_push(&window, true /* Animated */);
	
	layer_init(&background_layer, window.layer.frame);
    background_layer.update_proc = &update_background_callback;
    layer_add_child(&window.layer, &background_layer);	
	
	resource_init_current_app(&APP_RESOURCES);
	
	res_d = resource_get_handle(RESOURCE_ID_SMALL_26);
	res_t = resource_get_handle(RESOURCE_ID_BIG_52);

	font_date = fonts_load_custom_font(res_d);
	font_time = fonts_load_custom_font(res_t);

	text_layer_init(&time_layer, window.layer.frame);
	
	text_layer_set_background_color(&time_layer, GColorClear);
	text_layer_set_font(&time_layer, font_time);
	text_layer_set_text_alignment(&time_layer, GTextAlignmentCenter);
	layer_set_frame(&time_layer.layer, TIME_FRAME);
	layer_add_child(&window.layer, &time_layer.layer);

	text_layer_init(&date_layer, window.layer.frame);
	
	text_layer_set_background_color(&date_layer, GColorClear);
	text_layer_set_font(&date_layer, font_date);
	text_layer_set_text_alignment(&date_layer, GTextAlignmentCenter);
	layer_set_frame(&date_layer.layer, DATE_FRAME);
	layer_add_child(&window.layer, &date_layer.layer);

	// Add weather layer
	weather_layer_init(&weather_layer);
	layer_add_child(&window.layer, &weather_layer.layer);
	weather_layer_set_loading(&weather_layer);
	
	window_set_background_color(&window, GColorWhite);
	text_layer_set_text_color(&time_layer, GColorBlack);
	text_layer_set_text_color(&date_layer, GColorBlack);
	
	if(BUILD_STYLE==2 || BUILD_STYLE==3) {
		//WhiteOnBlack, Split1(WhiteTop)
		inverter_layer_init(&invertBottom, GRect(0,86,144,82));
		layer_add_child(&window.layer, &invertBottom.layer);
	}	

	http_register_callbacks((HTTPCallbacks){
		.failure=handle_failed,
		.success=handle_success,
		.reconnect=handle_reconnect,
		.location=handle_location
	}, (void*)ctx);

	//refresh time
	get_time(&tm);
	t.tick_time = &tm;
	t.units_changed = MINUTE_UNIT | HOUR_UNIT | DAY_UNIT;

	initial_minute = (tm.tm_min % FREQUENCY_MINUTES);

	handle_minute_tick(ctx, &t);
}

void handle_deinit(AppContextRef ctx) {
	fonts_unload_custom_font(font_date);
	fonts_unload_custom_font(font_time); 	
	weather_layer_deinit(&weather_layer);
}

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
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
		return;
	}
	dict_write_int32(body, WEATHER_KEY_LATITUDE, our_latitude);
	dict_write_int32(body, WEATHER_KEY_LONGITUDE, our_longitude);
	dict_write_cstring(body, WEATHER_KEY_UNIT_SYSTEM, UNIT_SYSTEM);
	// Send it.
	if(http_out_send() != HTTP_OK) {
		return;
	}
}
