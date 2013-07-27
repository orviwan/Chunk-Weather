#ifndef WEATHER_LAYER_H
#define WEATHER_LAYER_H

typedef struct {
	Layer layer;
	BmpContainer icon_layer;
	TextLayer temp_layer;
	TextLayer temp_layer_background;
	TextLayer temp_layer_highlow;
	bool has_weather_icon;
	char temperature[10];
  char highlow[36];
} WeatherLayer;

typedef enum {
	WEATHER_ICON_CLEAR_DAY = 0,
	WEATHER_ICON_CLEAR_NIGHT=1,
	WEATHER_ICON_RAIN=2,
	WEATHER_ICON_SNOW=3,
	WEATHER_ICON_SLEET=4,
	WEATHER_ICON_WIND=5,
	WEATHER_ICON_FOG=6,
	WEATHER_ICON_CLOUDY=7,
	WEATHER_ICON_PARTLY_CLOUDY_DAY=8,
	WEATHER_ICON_PARTLY_CLOUDY_NIGHT=9,
	WEATHER_ICON_THUNDER=10,
	WEATHER_ICON_RAIN_SNOW=11,
	WEATHER_ICON_SNOW_SLEET=12,
	WEATHER_ICON_COLD=13,
	WEATHER_ICON_HOT=14,
	WEATHER_ICON_NO_WEATHER=15
} WeatherIcon;

void weather_layer_init(WeatherLayer* weather_layer);
void weather_layer_deinit(WeatherLayer* weather_layer);
void weather_layer_set_icon(WeatherLayer* weather_layer, WeatherIcon icon);
void weather_layer_set_temperature(WeatherLayer* weather_layer, int16_t temperature);
void weather_layer_set_highlow(WeatherLayer* weather_layer, int16_t high, int16_t low);
void weather_layer_set_error(WeatherLayer* weather_layer, int http_status);

#endif
