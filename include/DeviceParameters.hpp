#pragma once

#include <Stream.h>
#include <EEManager.h>
#include <FastLED.h>
#include <ArduinoJson.h>

#include "settings.hpp"
#include "Path_parameters.hpp"


namespace APICLG {

  enum RoleType {
    begin,
    middle,
    end
  };

  enum TypeGate {
    strip,
    rect,
    triangle,
    hex,
    circle
  };
  
  enum StateType {
    off,
    run,
    pause,
    lowBattery,
  };

  enum ProgramType {
    solid,
    blink,
    gradient,
    wave,
  };

  struct DeviceParameters {
    public: 

    // char ssid[64];
    // char stapsk[64];

    // device parameters
    uint8_t role = middle;
    uint8_t typeGate = strip;
    int16_t offsetVoltage  = 0; // -256 to 255 (+- 250mV at range 0-1V)

    // program state parameters
    uint8_t state = off;
    uint8_t programType = solid;

    // if programType = gradient
    uint8_t gradientNumber = 0;        
    uint8_t gradientScale = 1;         
    uint8_t blendType = LINEARBLEND;  
    uint8_t speed = 1;

    // color HSV type
    uint8_t hue = 0;
    uint8_t sat = 0;
    uint8_t val = 255; // brightness
    
    
  };
  const uint16_t sizeJson = 256;

  uint8_t updateDeviceParameters(const APICLG::PathParameters &param);
  uint8_t createJson(StaticJsonDocument<sizeJson> &jsonDoc);
}

extern EEManager memoryDeviceParam; 
extern APICLG::DeviceParameters deviceParam;