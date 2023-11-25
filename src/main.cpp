#include <Arduino.h>
#include <EEManager.h>
#include <FastLED.h>

#include "settings.hpp"
#include "DeviceParameters.hpp"
#include "Server_Handler.hpp"
#include "palettes.hpp"

APICLG::DeviceParameters deviceParam;
EEManager memoryDeviceParam(deviceParam);


CRGB leds[NUM_LEDS];

void Error(uint8_t code);

void FillingLEDsSolidColors(const uint8_t hue, const uint8_t sat, const uint8_t val);
void FillingLEDsSolidColors(CHSV hsv);
void FillingLEDsSolidColors(const char *hsvCStr);
void FillLEDsFromPaletteColors(const APICLG::DeviceParameters &devPar);
void SmoothBlink(const uint8_t hue, const uint8_t sat, const uint8_t val, const uint8_t speed, const uint8_t smooth);

void setup() {
  #ifdef DEBUG_SERIAL
    Serial.begin(115200); // Start the Serial communication to send messages to the computer
    if (!Serial) Error(1);
    DEBUGMLN("\n");
  #endif

  delay( 1000 ); // power-up safety delay

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setCorrection(TypicalLEDStrip); // GRB ordering is typical
  // FastLED.setBrightness(255);
  FastLED.setMaxPowerInVoltsAndMilliamps(LED_SUPPLE_VOLTAGE, LED_SUPPLE_CURRENT);


#ifdef SAVE_EEPROM
    // выделение из Flesh памяти блоков для хранения данных (эмуляция EEPROM)
  EEPROM.begin(memoryDeviceParam.blockSize());

  // запись стандартных значений при первом запуске
  uint8_t stat = memoryDeviceParam.begin(0, MEM_INIT_KEY);
  if (stat == 0){
    DEBUGMLN("read data from eeprom memory");
  } else if (stat == 1){
    DEBUGMLN("first write data to eeprom memory");
  } else {
    DEBUGMLN("ERROR write data to eeprom memory, not enough memory space");
    Error(2);
  }

#ifdef DEBUG_SERIAL
  DEBUGMLN("Init Key and Data from eeprom memory: ");
  for (uint16_t i = memoryDeviceParam.startAddr(); i < memoryDeviceParam.endAddr() + 1; i++)
    DEBUGM(String(EEPROM.read(i)) + " ");
  DEBUGMLN();
  
#endif // DEBUG_SERIAL
#endif // SAVE_EEPROM

  if (!APICLG::serverInit())
    Error(3);
}

// String request = "POST http://192.168.0.1/api?role=begin&type-gate=rect&state=run&program-type=solid&speed=4&hsv=fc0115 HTTP/1.1";

void loop() {
  
  #ifdef SAVE_EEPROM
  if(memoryDeviceParam.tick()){
    DEBUGMLN("Memory Updated!");

    DEBUGMLN("role: " + String(deviceParam.role));
    DEBUGMLN("type-gate: " + String(deviceParam.typeGate));
    DEBUGMLN("offsetVoltage: " + String(deviceParam.offsetVoltage));
    DEBUGMLN("state: " + String(deviceParam.state)); 
    DEBUGMLN("program-type: " + String(deviceParam.programType));
    DEBUGMLN("gradientNumber: " + String(deviceParam.gradientNumber));
    DEBUGMLN("gradientScale: " + String(deviceParam.gradientScale));
    DEBUGMLN("blendType: " + String(deviceParam.blendType));
    DEBUGMLN("speed: " + String(deviceParam.speed));
    DEBUGMLN("hue: " + String(deviceParam.hue));
    DEBUGMLN("sat: " + String(deviceParam.sat));
    DEBUGMLN("val: " + String(deviceParam.val));
  }
  #endif

  APICLG::serverUpdate();
  yield();

  // if (Serial.available())
  // {
  //   String request = Serial.readStringUntil('\n');
  //   DEBUGMLN(request);

  //   String path = APICLG::pathFinding(request);
  //   DEBUGMLN(path);
  //   APICLG::parsePath(path);
  //   DEBUGMLN("role: " + String(deviceParam.role));
  //   DEBUGMLN("type-gate: " + String(deviceParam.typeGate));
  //   DEBUGMLN("offsetVoltage: " + String(deviceParam.offsetVoltage));
  //   DEBUGMLN("state: " + String(deviceParam.state)); 
  //   DEBUGMLN("program-type: " + String(deviceParam.programType));
  //   DEBUGMLN("speed: " + String(deviceParam.speed));
  //   DEBUGMLN("hue: " + String(deviceParam.hue));
  //   DEBUGMLN("sat: " + String(deviceParam.sat));
  //   DEBUGMLN("val: " + String(deviceParam.val));
  // }
  static uint8_t lowBattRepeat = 5;
  if (deviceParam.state == APICLG::StateType::off){
    FillingLEDsSolidColors(0, 0, 0);
  }
  else if (deviceParam.state == APICLG::StateType::run){
    lowBattRepeat = 5;
    switch (deviceParam.programType)
    {
    case APICLG::ProgramType::solid:
      FillingLEDsSolidColors(deviceParam.hue, deviceParam.sat, deviceParam.val);
      break;

    case APICLG::ProgramType::blink:
      SmoothBlink(deviceParam.hue, deviceParam.sat, deviceParam.val, deviceParam.speed, 35);
    // to do
      break;

    case APICLG::ProgramType::gradient:
      FillLEDsFromPaletteColors(deviceParam);
      break;

    case APICLG::ProgramType::wave:
    // to do
      break;

    default:
    // to do
      break;
    }
  }
  else if (deviceParam.state == APICLG::StateType::pause){
    // skip update frame 
  }
  else if (deviceParam.state == APICLG::StateType::lowBattery){
    if (lowBattRepeat > 0){ 
      lowBattRepeat--;
      SmoothBlink(0, 255, 10 + lowBattRepeat * 25, 15, 35);
    }
    // SmoothBlink(deviceParam.hue, deviceParam.sat, deviceParam.val, deviceParam.speed, deviceParam.gradientScale);
  }

  

  // Error(43);

}
// POST http://192.168.0.1/api?hsv=00fff0 HTTP/1.1


void Error(uint8_t code)
{
  while (1)
  {
    DEBUGMLN("Error code :" + String(code));
    delay(5000);
  }
}

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

void FillLEDsFromPaletteColors(const APICLG::DeviceParameters &devPar)
{
  static uint8_t colorIndex = 0;
  colorIndex += devPar.speed;
  // for (uint16_t i = 0; i < NUM_LEDS; ++i) {
  //   leds[i] = ColorFromPalette(paletteArr[devPar.gradientNumber % 44], colorIndex, devPar.val, TBlendType(devPar.blendType));
  //   colorIndex += devPar.gradientScale;
    
  // }
  fill_palette(leds, NUM_LEDS, colorIndex, devPar.gradientScale,
              paletteArr[devPar.gradientNumber % 44], devPar.val,
              TBlendType(devPar.blendType));
  FastLED.delay(1000/UPDATES_PER_SECOND);
  FastLED.show();
}

/// @brief Плавное мигание всей лентой 
/// @note Зависимость периода от скорости и плавности T = 1/(V+Sm) - обратно пропорциональная
/// на больших значениях T имеет смысл уменьшить Sm до 15, менять только V. На маленьких
/// значениях T имеет смысл увеличить Sm  15     
void SmoothBlink(const uint8_t hue, const uint8_t sat, const uint8_t val, const uint8_t speed, const uint8_t smooth)
{
  uint8_t hue2 = map8(hue, 0, 191);
  CHSV hsv(hue2, sat, 0);
  CRGB rgb;
  float step = (float)val/smooth;
  for (float i = 0; (i <= val) & (val - i > step); i+= step){
    hsv.v = i;
    hsv2rgb_raw(hsv, rgb);
    fill_solid(leds, NUM_LEDS, rgb);
    FastLED.show();
    FastLED.delay(1000/speed);
    APICLG::serverUpdate();
  }
  yield();
  for (float i = 0; (i <= val) & (val - i > step); i+= step){
    hsv.v = val - i;
    hsv2rgb_raw(hsv, rgb);
    fill_solid(leds, NUM_LEDS, rgb);
    FastLED.show();
    FastLED.delay(1000/speed);
    APICLG::serverUpdate();
  }
  rgb = 0;
  fill_solid(leds, NUM_LEDS, rgb);
  FastLED.show();

 
}




