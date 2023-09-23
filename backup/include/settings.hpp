#pragma once
#include <HardwareSerial.h>

#define NUM_LEDS 60
#define DATA_PIN D3


#ifndef STASSID
#define STASSID "Buzya"
#define STAPSK "2~2ht0*IB#4b"
#endif

#define MEM_INIT_KEY 82     // ключ первого запуска. 0-254, на выбор

#define DEBUG_SERIAL




extern HardwareSerial Serial;

extern "C" void yield(void);

#ifdef DEBUG_SERIAL
#define DEBUGMLN(x) Serial.println(x)
#define DEBUGM(x) Serial.print(x)
#else
#define DEBUGMLN(x)
#define DEBUGM(x)
#endif

