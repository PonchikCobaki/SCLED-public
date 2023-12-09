#include "ModeEffects.hpp"

CRGB leds[NUM_LEDS];

void FillingLEDsSolidColors(const uint8_t hue, const uint8_t sat, const uint8_t val){
  CHSV hsv;
  hsv.h = hue;
  hsv.s = sat;
  hsv.v = val;
  fill_solid(leds, NUM_LEDS, hsv);
  FastLED.show();
}

void FillingLEDsSolidColors(CHSV hsv){
  fill_solid(leds, NUM_LEDS, hsv);
}

void FillingLEDsSolidColors(const char *hsvCStr){
  uint32_t hsvHex = strtol(hsvCStr, NULL, 16);
  CHSV hsv((hsvHex >> 16) & 0xFF, (hsvHex >> 8) & 0xFF, hsvHex & 0xFF);
  fill_solid(leds, NUM_LEDS, hsv);
  FastLED.show();
}



/// @brief Smooth flashing of all LEDs
/// @note Зависимость периода от скорости и плавности T = (Sm+V) * Const 
/// @param samplingPeriod period of the sampling in milliseconds
/// @param mode 0 - cyclic blinking, 1 - increasing the brightness, 2 - lowering the brightness  
void SmoothBlink(const uint8_t hue, const uint8_t sat, const uint8_t val, const uint8_t valMax, const uint16_t samplingPeriod, const uint8_t smooth, uint8_t mode)
{
  // init variables
  static bool direction = true;              // true - up, false - down
  static float valCounter = val; // start brightness;
  static uint64_t startTimer = millis();
  
  if (mode == 1 && direction != true){
    direction = true;
    valCounter = 0;
  }
  else if (mode == 2 && direction != false){
    direction = false;
    valCounter = 0;
  }
 

  if (millis() - startTimer > samplingPeriod){  // delay to update the brightness
    startTimer = millis();
    
    float step = (float)valMax/smooth; // step size

    // convert HSV to RGB
    // uint8_t hue2 = map8(hue, 0, 191); // for the correct conversion of HSV to RGB, above 191 does not make sense
    CHSV hsv(hue, sat, 0);
    CRGB rgb;
    
    if (direction){
      // increasing the brightness
      hsv.v = valCounter;
    }
    else {
      // decreasing the brightness
      hsv.v = valMax - valCounter;
    }

    // DEBUGMLN("direction: " + String(direction));
    // DEBUGMLN("valCounter: " + String(valCounter));
    // DEBUGMLN("step: " + String(step));


    // display the color
    hsv2rgb_spectrum(hsv, rgb);
    fill_solid(leds, NUM_LEDS, rgb);
    FastLED.show();

    // iterative adder
    if (mode == 0 || (mode != 0 && valCounter <= valMax))
      valCounter += step;

    // changing the direction of brightness
    if (valCounter > valMax && mode == 0){
      direction = !direction; 
      valCounter = 0;
    }

  }
   
}

void FillLEDsFromPaletteColors(const APICLG::DeviceParameters &devPar)
{
  static uint8_t colorIndex = 0;
  colorIndex += devPar.speed;

  fill_palette(leds, NUM_LEDS, colorIndex, devPar.scale,
              paletteArr[devPar.gradientNumber % 44], devPar.val,
              TBlendType(devPar.blendType));
  FastLED.delay(1000/UPDATES_PER_SECOND);
  FastLED.show();
}

void SunRise(const uint8_t valMax, const uint8_t executionTime, uint8_t &start)
{
  
  constexpr uint8_t hueBegin = 25; // 35 / 360.0 * 255
  constexpr uint8_t hueEnd = 59;   // 72 / 360.0 * 255
  float hueStep = (hueEnd - hueBegin) / (float(executionTime) * 60);
  static float hueCounter = hueBegin;
  
  constexpr uint8_t satBegin = 255;
  constexpr uint8_t satEnd = 146;
  float satStep = (int(satEnd) - satBegin) / (float(executionTime) * 60);
  static float satCounter = satBegin;

  float valStep = valMax / (float(executionTime) * 60);
  static float valCounter = 0;

  
  static uint64_t startTimer = millis();
  // static uint64_t startSatTimer = startHueTimer;

  if (millis() - startTimer > 1000){  // delay to update the brightness
    startTimer = millis();
    if (hueCounter <= hueEnd){
      hueCounter += hueStep;
    }
    if (satEnd <= satCounter){
      satCounter += satStep;
    }
    if (valCounter <= valMax){
      valCounter += valStep;
    }
    DEBUGMLN("hueCounter: " + String(hueCounter) + " satCounter: " + String(satCounter) + " valCounter: " + String(valCounter));
    FillingLEDsSolidColors(hueCounter, satCounter, valCounter);

    if((hueCounter > hueEnd) && (satEnd > satCounter) && (valCounter > valMax)){
      start = false;
      memoryDeviceParam.update();
      hueCounter = hueBegin;
      satCounter = satBegin;
      valCounter = 0;
    }
  }
}
