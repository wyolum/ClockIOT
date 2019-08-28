/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;

void setup() {

    USE_SERIAL.begin(115200);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 1; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    wifiMulti.addAP("foobar", "spamspamspamspam");

}

bool gotit = false;
/*
{"meta":{"code":"200","execution_time":"0.416060 seconds"},"data":{"ip":"100.36.40.72","city":"Reston","postal":"20191",
"state":"Virginia","state_code":"VA","country":"United States","country_code":"US","location":"38.9311,-77.3489",
"timezone":{"id":"America\/New_York","location":"40.71416,-74.00639","country_code":"US",
"country_name":"United States of America","iso3166_1_alpha_2":"US","iso3166_1_alpha_3":"USA","un_m49_code":"840",
"itu":"USA","marc":"xxu","wmo":"US","ds":"USA","phone_prefix":"1","fifa":"USA","fips":"US","gual":"259","ioc":"USA",
"currency_alpha_code":"USD","currency_country_name":"UNITED STATES","currency_minor_unit":"2","currency_name":"US Dollar",
"currency_code":"840","independent":"Yes","capital":"Washington","continent":"NA","tld":".us","languages":"en-US,es-US,haw,fr",
"geoname_id":"6252001","edgar":""},"datetime":{"date":"09\/04\/2018","date_time":"09\/04\/2018 07:25:20",
"date_time_txt":"Tuesday, September 4, 2018 07:25:20","date_time_wti":"Tue, 04 Sep 2018 07:25:20 -0400",
"date_time_ymd":"2018-09-04T07:25:20-04:00","time":"07:25:20","month":"9","month_wilz":"09","month_abbr":"Sep",
"month_full":"September","month_days":"30","day":"4","day_wilz":"04","day_abbr":"Tue","day_full":"Tuesday","year":"2018",
"year_abbr":"18","hour_12_wolz":"7","hour_12_wilz":"07","hour_24_wolz":"7","hour_24_wilz":"07","hour_am_pm":"am",
"minutes":"25","seconds":"20","week":"36","offset_seconds":"-14400","offset_minutes":"-240","offset_hours":"-4",
"offset_gmt":"-04:00","offset_tzid":"America\/New_York","offset_tzab":"EDT","offset_tzfull":"Eastern Daylight Time",
"tz_string":"EST+5EDT,M3.2.0\/2,M11.1.0\/2","dst":"true","dst_observes":"true","timeday_spe":"early_morning",
"timeday_gen":"morning"}}}
-14400
*/

String jsonLookup(String s, String name){
  int start = s.indexOf(name) + name.length() + 3;
  int stop = s.indexOf('"', start);
  return s.substring(start, stop);
}

void loop() {
    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
        // http.begin("http://example.com/index.html"); //HTTP
	http.begin("https://wyolum.com/utc_offset/utc_offset.py");
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
	    String findme = String("utc_offset");
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
		int start = payload.indexOf(findme) + findme.length() + 3;
		int stop = payload.indexOf('"', start);
		int offset = jsonLookup(payload, String("utc_offset")).toInt();
		String dst_str = jsonLookup(payload, String("dst"));
		bool dst = dst_str.equals("true");
		USE_SERIAL.print(offset);
		USE_SERIAL.print(", ");
		USE_SERIAL.println(dst);
		while(1)delay(100);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(5000);
}
