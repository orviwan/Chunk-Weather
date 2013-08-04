#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "util.h"
#include "weather_layer.h"
#include "config.h"

GFont font_temp; 
GFont font_highlow; 

InverterLayer invertTop;

static uint8_t WEATHER_ICONS[] = {
	RESOURCE_ID_IMAGE_TORNADO,
	RESOURCE_ID_IMAGE_TROPICAL_STORM,
	RESOURCE_ID_IMAGE_HURRICANE,
	RESOURCE_ID_IMAGE_SEVERE_THUNDERSTORMS,
	RESOURCE_ID_IMAGE_THUNDERSTORMS,
	RESOURCE_ID_IMAGE_MIXED_RAIN_AND_SNOW,
	RESOURCE_ID_IMAGE_MIXED_RAIN_AND_SLEET,
	RESOURCE_ID_IMAGE_MIXED_SNOW_AND_SLEET,
	RESOURCE_ID_IMAGE_FREEZING_DRIZZLE,
	RESOURCE_ID_IMAGE_DRIZZLE,
	RESOURCE_ID_IMAGE_FREEZING_RAIN,
	RESOURCE_ID_IMAGE_SHOWERS,
	RESOURCE_ID_IMAGE_SHOWERS2,
	RESOURCE_ID_IMAGE_SNOW_FLURRIES,
	RESOURCE_ID_IMAGE_LIGHT_SNOW_SHOWERS,
	RESOURCE_ID_IMAGE_BLOWING_SNOW,
	RESOURCE_ID_IMAGE_SNOW,
	RESOURCE_ID_IMAGE_HAIL,
	RESOURCE_ID_IMAGE_SLEET,  
	RESOURCE_ID_IMAGE_DUST,
	RESOURCE_ID_IMAGE_FOGGY,
	RESOURCE_ID_IMAGE_HAZE,
	RESOURCE_ID_IMAGE_SMOKY,
	RESOURCE_ID_IMAGE_BLUSTERY,
	RESOURCE_ID_IMAGE_WINDY,
	RESOURCE_ID_IMAGE_COLD,
	RESOURCE_ID_IMAGE_CLOUDY,
	RESOURCE_ID_IMAGE_MOSTLY_CLOUDY_NIGHT,
	RESOURCE_ID_IMAGE_MOSTLY_CLOUDY_DAY,
	RESOURCE_ID_IMAGE_PARTLY_CLOUDY_NIGHT,
	RESOURCE_ID_IMAGE_PARTLY_CLOUDY_DAY,
	RESOURCE_ID_IMAGE_CLEAR_NIGHT,
	RESOURCE_ID_IMAGE_SUNNY,
	RESOURCE_ID_IMAGE_FAIR_NIGHT,
	RESOURCE_ID_IMAGE_FAIR_DAY,
	RESOURCE_ID_IMAGE_MIXED_RAIN_AND_HAIL,
	RESOURCE_ID_IMAGE_HOT,
	RESOURCE_ID_IMAGE_ISOLATED_THUNDERSTORMS,
	RESOURCE_ID_IMAGE_SCATTERED_THUNDERSTORMS,
	RESOURCE_ID_IMAGE_SCATTERED_THUNDERSTORMS2,
	RESOURCE_ID_IMAGE_SCATTERED_SHOWERS,
	RESOURCE_ID_IMAGE_HEAVY_SNOW,
	RESOURCE_ID_IMAGE_SCATTERED_SNOW_SHOWERS,
	RESOURCE_ID_IMAGE_HEAVY_SNOW2,
	RESOURCE_ID_IMAGE_PARTLY_CLOUDY,
	RESOURCE_ID_IMAGE_THUNDERSHOWERS,
	RESOURCE_ID_IMAGE_SNOW_SHOWERS,
	RESOURCE_ID_IMAGE_ISOLATED_THUNDERSHOWERS,
	RESOURCE_ID_IMAGE_NOT_AVAILABLE
};

void weather_layer_init(WeatherLayer* weather_layer) {
	layer_init(&weather_layer->layer, GRect(0, 0, 144, 86));

	ResHandle res_t;
	ResHandle res_hl;

	res_t = resource_get_handle(RESOURCE_ID_MEDIUM_34);
	res_hl = resource_get_handle(RESOURCE_ID_SMALL_22);

	font_temp = fonts_load_custom_font(res_t);
	font_highlow = fonts_load_custom_font(res_hl);

	// Add temperature layer
	text_layer_init(&weather_layer->temp_layer, WEATHER_TEMP_FRAME);
	text_layer_set_background_color(&weather_layer->temp_layer, GColorClear);	
	text_layer_set_text_alignment(&weather_layer->temp_layer, GTextAlignmentCenter);
	text_layer_set_font(&weather_layer->temp_layer, font_temp);
	layer_add_child(&weather_layer->layer, &weather_layer->temp_layer.layer);

	//highlow layer
	text_layer_init(&weather_layer->temp_layer_highlow, WEATHER_HL_FRAME);
	text_layer_set_background_color(&weather_layer->temp_layer_highlow, GColorClear);	
	text_layer_set_text_alignment(&weather_layer->temp_layer_highlow, GTextAlignmentCenter);
	text_layer_set_font(&weather_layer->temp_layer_highlow, font_highlow);
	layer_add_child(&weather_layer->layer, &weather_layer->temp_layer_highlow.layer);

	bmp_init_container(WEATHER_ICONS[WEATHER_ICON_NOT_AVAILABLE], &weather_layer->icon_layer);
	
	text_layer_set_text_color(&weather_layer->temp_layer, GColorBlack);
	text_layer_set_text_color(&weather_layer->temp_layer_highlow, GColorBlack);	
	
	if(BUILD_STYLE>=3) {
		//WhiteOnBlack, Split2(BlackTop)
		inverter_layer_init(&invertTop, GRect(0, 0, 144, 86));
		layer_add_child(&weather_layer->layer, &invertTop.layer);
	}
	
}

void weather_layer_set_icon(WeatherLayer* weather_layer, WeatherIcon icon) {	
	layer_remove_from_parent(&weather_layer->icon_layer.layer.layer);
	bmp_deinit_container(&weather_layer->icon_layer);
	bmp_init_container(WEATHER_ICONS[icon], &weather_layer->icon_layer);
	layer_set_frame(&weather_layer->icon_layer.layer.layer, WEATHER_ICON_FRAME);
	layer_add_child(&weather_layer->layer, &weather_layer->icon_layer.layer.layer);
	
	if(BUILD_STYLE>=3) {
		//WhiteOnBlack, Split2(BlackTop)
		layer_remove_from_parent(&invertTop.layer);
		layer_add_child(&weather_layer->layer, &invertTop.layer);
	}
}

void weather_layer_set_highlow(WeatherLayer* weather_layer, int16_t high, int16_t low) {
	snprintf(weather_layer->highlow, sizeof(weather_layer->highlow), "LOW:%d° HIGH:%d°", low, high);
	text_layer_set_text(&weather_layer->temp_layer_highlow, weather_layer->highlow);
}

void weather_layer_set_loading(WeatherLayer* weather_layer) {
	snprintf(weather_layer->highlow, sizeof(weather_layer->highlow), "%s", "CHUNK v1.4");
	text_layer_set_text(&weather_layer->temp_layer_highlow, weather_layer->highlow);
}

void weather_layer_set_temperature(WeatherLayer* weather_layer, int16_t t) {
	snprintf(weather_layer->temperature, sizeof(weather_layer->temperature), "%d°", t);
	text_layer_set_text(&weather_layer->temp_layer, weather_layer->temperature);  
}

void weather_layer_set_error(WeatherLayer* weather_layer, int http_status) {
	weather_layer_set_icon(weather_layer, WEATHER_ICON_NOT_AVAILABLE);
	if(http_status==0) {
		snprintf(weather_layer->temperature, sizeof(weather_layer->temperature), "%s%s", "?", "...");
	}
	else {
		snprintf(weather_layer->temperature, sizeof(weather_layer->temperature), "%s%d", "E:", http_status-1000);
	}
	text_layer_set_text(&weather_layer->temp_layer, weather_layer->temperature);  
}

void weather_layer_deinit(WeatherLayer* weather_layer) {
	bmp_deinit_container(&weather_layer->icon_layer);
	fonts_unload_custom_font(font_temp);
	fonts_unload_custom_font(font_highlow);
}
