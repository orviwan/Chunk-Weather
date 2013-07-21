#define SERVER_URL "http://www.mirz.com/httpebble/new-weather.php"

//1=ON;0=OFF - loads appropriate background image
#define SPLIT_MODE 1

//USE DIFFERENT COLOURS FOR TOP/BOTTOM OFF THE SCREEN
#define COLOUR_TEXT_TOP_SPLIT GColorWhite
#define COLOUR_TEXT_BOTTOM_SPLIT GColorBlack

//Celsius / Fahrenheit
#define UNIT_SYSTEM "c" // c or f

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

