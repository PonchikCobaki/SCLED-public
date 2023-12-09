#pragma once

#include <Stream.h>
#include <EEManager.h>
#include <FastLED.h>
#include <ArduinoJson.h>

#include "settings.hpp"
#include "PathParameters.hpp"


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
    sunrise,
  };


 ///@brief The structure of the main parameters of the device that are stored in memory
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
    uint8_t blendType = LINEARBLEND;  
    

    // if programType = gradient or blink or sunrise 

    uint8_t scale = 1;         
    uint8_t speed = 1;


    // if programType = sunrise

    uint16_t delaySunrise = 0;  // in minutes, only ~ 1092 hours
    uint16_t refPoint; // the beginning point of the countdown for sunrise
    uint8_t  sunriseStarted = false; 

    // if programType = solid or blink
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