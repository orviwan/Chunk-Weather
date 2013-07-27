#ifndef CONFIG_H
#define CONFIG_H

// This is the Default APP_ID to work with old versions of httpebble (iOS)
#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x04, 0x9F, 0x49, 0xC0, 0x99, 0xAD }

// Use this one if you want to compile just for Android
//#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x29, 0x08, 0x7A, 0xB9, 0xB6, 0x19}


#define SERVER_URL "http://www.mirz.com/httpebble/weather-yahoo.php"

//1=ON;0=OFF - loads appropriate background image
#define SPLIT_MODE 1

//USE DIFFERENT COLOURS FOR TOP/BOTTOM OFF THE SCREEN
#define COLOUR_TEXT_TOP_SPLIT GColorWhite
#define COLOUR_TEXT_BOTTOM_SPLIT GColorBlack

//Celsius / Fahrenheit
#define UNIT_SYSTEM "f" // c or f

//Check for new weather every X minutes (server code has 14 minute cache header)
#define FREQUENCY_MINUTES 15

// POST variables
#define WEATHER_KEY_LATITUDE 1
#define WEATHER_KEY_LONGITUDE 2
#define WEATHER_KEY_UNIT_SYSTEM 3
	
// Received variables
#define WEATHER_KEY_ICON 1
#define WEATHER_KEY_TEMPERATURE 2
#define WEATHER_KEY_SUNRISE 3
#define WEATHER_KEY_SUNSET 4
#define WEATHER_KEY_HIGH 5
#define WEATHER_KEY_LOW 6
#define WEATHER_KEY_CITY 7
#define WEATHER_KEY_NOW 8

#define WEATHER_HTTP_COOKIE 1949999771
#define TIME_HTTP_COOKIE 1199938782

#endif
