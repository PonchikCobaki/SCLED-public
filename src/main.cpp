#include <Arduino.h>
#include <EEManager.h>


#include "settings.hpp"
#include "DeviceParameters.hpp"
#include "ServerHandler.hpp"
#include "ModeEffects.hpp"


APICLG::DeviceParameters deviceParam;
EEManager memoryDeviceParam(deviceParam);


void Error(uint8_t code);


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

  static uint8_t lowBattRepeat = 5; // count blinking when low battery
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
          if (curTime - deviceParam.refPoint < deviceParam.delaySunrise){
            break;
          }
        } 
        else { //  resetting the clock variable after 51 days of continuous operation of the device
          if ((65535 - deviceParam.refPoint) + minutes16() < deviceParam.delaySunrise){
            break;
          }
        }
        // start sunrise
        SunRise(deviceParam.val, 1, deviceParam.sunriseStarted);

      } else {
        //  smooth fading at the end of the alarm and when the mode is selected
        if (leds[0] != CRGB::Black){
          static uint32_t refTime = millis();
          if (millis() - refTime > 20){
            refTime = millis();
            fade_raw(leds, NUM_LEDS, 1);
            FastLED.show();
          }
          
        }
        
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


}



void Error(uint8_t code)
{
  while (1)
  {
    DEBUGMLN("Error code :" + String(code));
    delay(5000);
  }
}
