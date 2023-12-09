#pragma once


#include "DeviceParameters.hpp"
#include "Palettes.hpp"


extern CRGB leds[NUM_LEDS];

void FillingLEDsSolidColors(const uint8_t hue, const uint8_t sat, const uint8_t val);
void FillingLEDsSolidColors(CHSV hsv);
void FillingLEDsSolidColors(const char *hsvCStr);
void SmoothBlink(const uint8_t hue, const uint8_t sat, const uint8_t val, const uint8_t valMax, const uint16_t samplingPeriod, const uint8_t smooth, uint8_t mode=0);
void FillLEDsFromPaletteColors(const APICLG::DeviceParameters &devPar);
void SunRise(const uint8_t valMax, const uint8_t executionTime, uint8_t &start);