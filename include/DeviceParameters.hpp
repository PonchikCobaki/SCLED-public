#pragma once

#include <Stream.h>
#include <FastLED.h>



namespace APICLG {

  enum RoleType {
    begin,
    middle,
    end
  };

  enum TypeGate {
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
    gradien,
    wave,
  };

  class DeviceParameters {
    public:

    uint8_t role;
    uint8_t typeGate;
    uint8_t state;
    uint8_t programType;
    uint8_t speed;
    CHSV colorHSV;

    DeviceParameters();
    
  };

}