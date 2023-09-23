/// @file    Blink.ino
/// @brief   Blink the first LED of an LED strip
/// @example Blink.ino

#include <Arduino.h>
#include <FastLED.h>
#include <EEManager.h>

#include "settings.hpp"
#include "mDNS_Server.hpp"
#include "DeviceParameters.hpp"
#include "palettes.hpp"



#define DEBUG_SERIAL

// основные параметры устройства
APICLG::DeviceParameters deviceParameters;


// объект управляющий хранением данных устройтсва в энергонезависимой памяти
EEManager memoryDeviceParam(deviceParameters);

CRGB leds[NUM_LEDS];


// String command{};


void updateLedSettings(APICLG::HTTPParameters &paramReq, APICLG::DeviceParameters &param);

// void FillLEDsFromPaletteColorsSerial(uint8_t colorIndex);
void FillingLEDsSolidColors(CHSV hsv);
void FillingLEDsSolidColors(const char *hsvCStr);
void Error(uint8_t code);




void setup()
{
// вывод данных через UART, используется для отладки
#ifdef DEBUG_SERIAL
  Serial.begin(115200); // Start the Serial communication to send messages to the computer
  if (!Serial) Error(1);
  DEBUGMLN("\n");
#endif

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
  
#endif

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setCorrection(TypicalLEDStrip); // GRB ordering is typical
  FastLED.setBrightness(64);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);

  if (!APICLG::mDNSServerInit())
    Error(3);

}


void loop()
{
  if(memoryDeviceParam.tick()) DEBUGMLN("Memory Updated!");
  // to do update memory

// String request = "POST http://192.168.0.1/api?role=begin&type-gate=rect&state=run&program-type=solid&speed=4&hsv=fc0115 HTTP/1.1";



  // static APICLG::HTTPParameters requestParameters;
  // static APICLG::DeviceParameters device;
  // APICLG::RequestType method = APICLG::serverUpdate(requestParameters, device);

  // switch (method)
  // {
  // case APICLG::GET:
  //   // to do
  //   break;
  // case APICLG::POST:
  //   updateLedSettings(requestParameters, device);
  //   break;
  // default:
  //   break;
  // }


  


  // static uint8_t currentPalette{7};
  // static uint8_t colorIndex{0};
  // colorIndex += 1;
  // static uint8_t brightness = 255;
  // for (int i = 0; i < NUM_LEDS; ++i)
  // {
  //   // CRGB color = ColorFromPalette(paletteArr[curPal], idx, 255, LINEARBLEND);
  //   // leds[i] = ColorFromPalette(paletteArr[currentPalette % 44], colorIndex, brightness, LINEARBLEND);
  //   colorIndex += 5;
  // }

  // FastLED.show();

  // FastLED.delay(100);
  // Error(10);
}





// void FillLEDsFromPaletteColorsSerial(uint8_t colorIndex)
// {
//   if (Serial.available())
//   {
//     command = Serial.readStringUntil('\n'); // Чтение строки с командой
//     DEBUGM("Received command: ");
//     DEBUGMLN(command);
//   }



void Error(uint8_t code)
{
  while (1)
  {
    DEBUGMLN("Error code :" + String(code));
    delay(5000);
  }
}

void updateLedSettings(APICLG::HTTPParameters &paramReq, APICLG::DeviceParameters &device)
{
  for (auto param : paramReq.getParameters()) {
    if (param.getName() == "role"){
      // to do
      DEBUGMLN("role = " + param.getValue());
      String valReq = param.getValue();
      
      if (valReq == "begin"){
        device.role = APICLG::RoleType::begin;
      } else if (valReq == "middle"){
        device.role = APICLG::RoleType::middle;
      } else if (valReq == "end"){
        device.role = APICLG::RoleType::end;
      }


    } else if (param.getName() == "state") {
      // to do
      DEBUGMLN("state = " + param.getValue());
    } else if (param.getName() == "program-type") {
      // to do
      DEBUGMLN("program-type = " + param.getValue());
    } else if (param.getName() == "speed") {
      // to do
      DEBUGMLN("speed = " + param.getValue());
    } else if (param.getName() == "hsv")
    {
      FillingLEDsSolidColors(param.getValue().c_str());
      #ifdef DEBUG_SERIAL
      uint32_t hsvHex = strtol(param.getValue().c_str(), NULL, 16);
      CHSV hsv((hsvHex >> 16) & 0xFF, (hsvHex >> 8) & 0xFF, hsvHex & 0xFF); 
      DEBUGMLN("hsv = " + String(hsv.hue) + ", "  + String(hsv.saturation) + ", " + String(hsv.value));
      #endif
    }


  }



}

void FillingLEDsSolidColors(CHSV hsv){
  fill_solid(leds, NUM_LEDS, hsv);
}

void FillingLEDsSolidColors(const char *hsvCStr){
  uint32_t hsvHex = strtol(hsvCStr, NULL, 16);
  CHSV hsv((hsvHex >> 16) & 0xFF, (hsvHex >> 8) & 0xFF, hsvHex & 0xFF);
  fill_solid(leds, NUM_LEDS, hsv);
}
