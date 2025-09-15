#pragma once
#include <HardwareSerial.h>


#define DEBUG_SERIAL    
#define SAVE_EEPROM 

// #define STRIP
#define STRIP_5M
// #define RECTANGLE_LOOP

// #define MY_WIFI
// #define MY_WIFI_2


/* 
  WiFi setting
 */

#ifndef MY_WIFI || !defined(MY_WIFI_2)
#define STD_WIFI
#endif

#ifdef MY_WIFI
#ifndef STASSID
#define STASSID "BSSID1";
#define STAPSK  "PASS1";
#endif
#endif

#ifdef MY_WIFI_2
#ifndef STASSID
#define STASSID "BSSID2";
#define STAPSK  "PASS2";
#endif
#endif

#ifdef STD_WIFI
#ifndef STASSID
#define STASSID "BSSID3";
#define STAPSK  "PASS3";
#endif
#endif


/* 
  Main LED settings
 */

// #ifndef STRIP
// #define RECTANGLE_LOOP
// #endif

#ifdef STRIP
#define LED_SUPPLE_VOLTAGE 5
#define LED_SUPPLE_CURRENT 1500 // mA
#define MAX_BRIGHTNESS 255 // 0-255

#define NUM_LEDS    60
#define DATA_PIN    D2
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#endif

#ifdef STRIP_5M
#define LED_SUPPLE_VOLTAGE 5
#define LED_SUPPLE_CURRENT 2000 // mA
#define MAX_BRIGHTNESS 50 // 0-255

#define NUM_LEDS    300
#define DATA_PIN    D2
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#endif

#ifdef RECTANGLE_LOOP
#define LED_SUPPLE_VOLTAGE 12
#define LED_SUPPLE_CURRENT 1000 // mA
#define MAX_BRIGHTNESS 100 // 0-255

#define NUM_LEDS    112
#define DATA_PIN    D4
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB

#define SAVE_EEPROM
#endif



/* 
  Other settings
 */

#define UPDATES_PER_SECOND 60

#define MEM_INIT_KEY 82     // ключ первого запуска. 0-254, на выбор




// 
// 
// 

extern HardwareSerial Serial;

extern "C" void yield(void);

#ifdef DEBUG_SERIAL
#define DEBUGMLN(x) Serial.println(x)
#define DEBUGM(x) Serial.print(x)
#else
#define DEBUGMLN(x)
#define DEBUGM(x)
#endif

