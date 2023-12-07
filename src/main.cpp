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
void SmoothBlink(const uint8_t hue, const uint8_t sat, const uint8_t val, const uint8_t valMax, const uint16_t samplingPeriod, const uint8_t smooth, uint8_t mode=0);
void SunRise(const uint8_t valMax, const uint8_t executionTime, uint8_t &start);

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
    DEBUGMLN("scale: " + String(deviceParam.scale));
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
      SmoothBlink(deviceParam.hue,
                  deviceParam.sat,
                  deviceParam.val,
                  deviceParam.val,
                  5000.0 / deviceParam.speed,
                  deviceParam.scale,
                  deviceParam.offsetVoltage
                  );
    // to do
      break;

    case APICLG::ProgramType::gradient:
      FillLEDsFromPaletteColors(deviceParam);
      break;

    case APICLG::ProgramType::wave:
    // to do
      break;

    case APICLG::ProgramType::sunrise:
    {
      if (deviceParam.sunriseStarted){
        uint16_t curTime = minutes16();

        // chek timout
        if (deviceParam.refPoint <= curTime){
          if (curTime - deviceParam.refPoint <= deviceParam.delaySunrise){
            break;
          }
        } 
        else { //  resetting the clock variable after 51 days of continuous operation of the device
          if ((65535 - deviceParam.refPoint) + minutes16() <= deviceParam.delaySunrise){
            break;
          }
        }
        
        // start sunrise

        SunRise(deviceParam.val, 1, deviceParam.sunriseStarted);

      } else {
        FillingLEDsSolidColors(0, 0, 0);
      }

      break;
    }
  
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
      SmoothBlink(0, 255, 10 + lowBattRepeat * 25, 10 + lowBattRepeat * 25, 15, 35);
    }
    // SmoothBlink(deviceParam.hue, deviceParam.sat, deviceParam.val, deviceParam.speed, deviceParam.scale);
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
  // uint8_t scaleReal = map8(devPar.scale, 255, 127);
  // DEBUGMLN("scaleReal: " + String(scaleReal));
  static uint8_t colorIndex = 0;
  colorIndex += devPar.speed;
  // for (uint16_t i = 0; i < NUM_LEDS; ++i) {
  //   leds[i] = ColorFromPalette(paletteArr[devPar.gradientNumber % 44], colorIndex, devPar.val, TBlendType(devPar.blendType));
  //   colorIndex += devPar.scale;
    
  // }
  fill_palette(leds, NUM_LEDS, colorIndex, devPar.scale,
              paletteArr[devPar.gradientNumber % 44], devPar.val,
              TBlendType(devPar.blendType));
  FastLED.delay(1000/UPDATES_PER_SECOND);
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
    // deviceParam.val = hsv.v;

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
      hueCounter = hueBegin;
      satCounter = satBegin;
      valCounter = 0;
    }
  }
}
