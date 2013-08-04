#ifndef CONFIG_H
#define CONFIG_H

#define SERVER_URL "http://www.mirz.com/httpebble/Chunk-1-4.php"

//1=BlackOnWhite, 2=Split1(WhiteTop), 3=WhiteOnBlack, 4=Split2(BlackTop)
#define BUILD_STYLE 1

//Celsius / Fahrenheit
#define UNIT_SYSTEM "f" // c or f

//vibrate on connection state
#define VIBRATE 1

//Check for new weather every X minutes
#define FREQUENCY_MINUTES 15

// POST variables
#define WEATHER_KEY_LATITUDE 1
#define WEATHER_KEY_LONGITUDE 2
#define WEATHER_KEY_UNIT_SYSTEM 3
	
// Received variables
#define WEATHER_KEY_TEMPERATURE 1
#define WEATHER_KEY_HIGH 2
#define WEATHER_KEY_LOW 3
#define WEATHER_CODE 4

//mmm cookie
#define WEATHER_HTTP_COOKIE 1949999771
#define TIME_HTTP_COOKIE 1199938782

//frames
#define TIME_FRAME      (GRect(0, 81, 144, 54))
#define DATE_FRAME      (GRect(0, 138, 144, 30))
#define WEATHER_ICON_FRAME  (GRect(14, 8, 50, 50))
#define WEATHER_TEMP_FRAME  (GRect(65, 12, 76, 50))
#define WEATHER_HL_FRAME    (GRect(0, 54, 144, 30))


#endif
