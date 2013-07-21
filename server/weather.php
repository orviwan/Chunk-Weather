<?php
define("FLICKR_KEY", "XXXXXXXXXXXXXXXXXXXX");  //GET YOUR OWN API KEY

$payload = json_decode(file_get_contents('php://input'), true);
if(!$payload) {
	//fake data if none supplied
	$payload = json_decode("{\"1\": 20724248,\"2\": -2508086,\"3\": \"c\"}", true);
}	

$lat = $payload[1] / 1000000;
$long = $payload[2] / 1000000;
$units = $payload[3];

$success = false;

if(!$success) {
	$flickrResponse = get_data('http://api.flickr.com/services/rest/?method=flickr.places.findByLatLon&format=json&api_key=' . FLICKR_KEY . '&lat=' . $lat . '&lon=' . $long);
	$flickrResponse = json_decode(substr($flickrResponse, 14, strlen($flickrResponse) - 15), true);
	$woeid = $flickrResponse['places']['place'][0]['woeid'];
	$woename = $flickrResponse['places']['place'][0]['woe_name'];
}

$xml = simplexml_load_file('http://weather.yahooapis.com/forecastrss?w=' . $woeid . '&u=' . $units);
$xml->registerXPathNamespace('yweather', 'http://xml.weather.yahoo.com/ns/rss/1.0');
$condition = $xml->channel->item->xpath('yweather:condition');
$temperature = round($condition[0]['temp']);

$astronomy = $xml->channel->xpath('yweather:astronomy');
$sunrise = str_replace(' ', '', $astronomy[0]['sunrise']);
$sunset = str_replace(' ', '', $astronomy[0]['sunset']);

$forecast = $xml->channel->item->xpath('yweather:forecast');
$forecasthigh = $forecast[0]['high'];
$forecastlow = $forecast[0]['low'];

$location = $xml->channel->xpath('yweather:location');
$city = $location[0]['city'];

// yahoo code => watch face icon id // yahoo condition => watch face condition
$icons = array(
	0 => 5, //tornado => wind
	1 => 5, //tropical storm => wind
	2 => 5, //hurricane => wind
	3 => 10, //severe thunderstorms => thunder
	4 => 10, //thunderstorms => thunder
	5 => 11, //mixed rain and snow => rain-snow
	6 => 12, //mixed rain and sleet => rain-sleet
	7 => 13, //mixed snow and sleet => snow-sleet
	8 => 2, //freezing drizzle => rain
	9 => 2, //drizzle => rain
	10 => 2, //freezing rain => rain
	11 => 2, //showers => rain
	12 => 2, //showers => rain
	13 => 3, //snow flurries => snow
	14 => 3, //light snow showers => snow
	15 => 3, //blowing snow => snow
	16 => 3, //snow => snow
	17 => 4, //hail => sleet
	18 => 4, //sleet => sleet
	19 => 6, //dust => fog
	20 => 6, //foggy => fog
	21 => 6, //haze => fog
	22 => 6, //smoky => fog
	23 => 5, //blustery => wind
	24 => 5, //windy => wind
	25 => 14, //cold => cold
	26 => 7, //cloudy => cloudy
	27 => 9, //mostly cloudy (night) => partly-cloudy-night
	28 => 8, //mostly cloudy (day) => partly-cloudy-day
	29 => 9, //partly cloudy (night) => partly-cloudy-night
	30 => 8, //partly cloudy (day) => partly-cloudy-day
	31 => 1, //clear (night) => clear-night
	32 => 0, //sunny => clear-day
	33 => 9, //fair (night) => partly-cloudy-night
	34 => 8, //fair (day) => partly-cloudy-day
	35 => 12, //mixed rain and hail => rain-sleet
	36 => 15, //hot => hot
	37 => 10, //isolated thunderstorms => thunder
	38 => 10, //scattered thunderstorms => thunder
	39 => 10, //scattered thunderstorms => thunder
	40 => 2, //scattered showers => rain
	41 => 3, //heavy snow => snow
	42 => 3, //scattered snow showers => snow
	43 => 3, //heavy snow => snow
	44 => 8, //partly cloudy => partly-cloudy-day
	45 => 10, //thundershowers => thunder
	46 => 3, //snow showers => snow
	47 => 10, //isolated thundershowers => thunder
	3200 => 16 //not available
);

$data = array();
//careful not to send too much data, can't exceed 100 bytes
$data[1] = array('b', $icons[(int) $condition[0]['code']]);
$data[2] = (int)$temperature; //array('s', round($condition[0]['temp'])); 
//$data[3] = (string)$sunrise;
//$data[4] = (string)$sunset;
$data[5] = (int)$forecasthigh; //(string)$forecasthigh;
$data[6] = (int)$forecastlow; //(string)$forecastlow;
//$data[7] = (string)$city;


/*
$logentry = date('Y-m-d H:i:s') . PHP_EOL . json_encode($payload) . PHP_EOL . json_encode($data) . PHP_EOL; 

$fp = @fopen('log.txt', 'a');  
fputs($fp, $logentry . PHP_EOL);  
@fclose($fp);  
*/

header('Content-Type: application/json');
header('Cache-Control: max-age=840'); //840 seconds = 14 minutes
echo(json_encode($data));


function get_data($url) {
    $ch = curl_init();
    $timeout = 5;
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, $timeout);
    $data = curl_exec($ch);
    curl_close($ch);
    return $data;
}

?>
