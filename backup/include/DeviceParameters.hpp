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
    gradien,
    wave,
  };

  struct DeviceParameters {
    public: 

    uint8_t role = middle;
    uint8_t typeGate = strip;
    int16_t volOffset = 0; // -256 to 255 (+- 250mV at range 0-1V)

    uint8_t state = off;
    uint8_t programType = solid;
    uint8_t speed = 1;

    uint8_t hue = 0;
    uint8_t sat = 0;
    uint8_t val = 255; // brightness
  };

}