#include "DeviceParameters.hpp"




/// @brief Processing HTTP key-value parcels
/// @note Function return 
/// @note 0 - good update device parameters
/// @note 1 - parameter update failed, the data is the same 
/// @note 2 - parameter update failed, a non-existent parameter or an error in the name
/// @note 3 - empty parameter
uint8_t APICLG::updateDeviceParameters(const APICLG::PathParameters &param)
{
  // the parameter is not empty
  if (param.name != "" && param.value != ""){
    if (param.name == "role"){
      if (param.value == "begin"){
        deviceParam.role = APICLG::RoleType::begin;
      }
      else if (param.value == "middle"){
        deviceParam.role = APICLG::RoleType::middle;
      }
      else if (param.value == "end"){
        deviceParam.role = APICLG::RoleType::end;
      }
      else{
        DEBUGMLN("Invalid role");
        return 1;
      }
      return 0;
    }
    else if (param.name == "type-gate"){
      if (param.value == "strip"){
        deviceParam.typeGate = APICLG::TypeGate::strip;
      }
      else if (param.value == "rect"){
        deviceParam.typeGate = APICLG::TypeGate::rect;
      }
      else if (param.value == "triangle"){
        deviceParam.typeGate = APICLG::TypeGate::triangle;
      }
      else if (param.value == "hex"){
        deviceParam.typeGate = APICLG::TypeGate::hex;
      }
      else if (param.value == "circle"){
        deviceParam.typeGate = APICLG::TypeGate::circle;
      }
      else{
        DEBUGMLN("Invalid type-gate");
        return 1;
      }
      return 0;
    }
    else if (param.name == "offset-voltage"){
      deviceParam.offsetVoltage = param.value.toInt();
      return 0;
    }

    else if (param.name == "state"){
      if (param.value == "off"){
        deviceParam.state = APICLG::StateType::off;
      }
      else if (param.value == "run"){
        deviceParam.state = APICLG::StateType::run;
      }
      else if (param.value == "pause"){
        deviceParam.state = APICLG::StateType::pause;
      }

      #ifdef DEBUG_SERIAL
      else if ((param.value == "low-battery")){
        deviceParam.state = APICLG::StateType::lowBattery;
      }
      #endif

      else{
        DEBUGMLN("Invalid state");
        return 1;
      }
      return 0;
    }
    else if (param.name == "program-type"){
      if (param.value == "solid"){
        deviceParam.programType = APICLG::ProgramType::solid;
      }
      else if (param.value == "blink"){
        deviceParam.programType = APICLG::ProgramType::blink;
      }
      else if (param.value == "gradient"){
        deviceParam.programType = APICLG::ProgramType::gradient;
      }
      else if (param.value == "wave"){
        deviceParam.programType = APICLG::ProgramType::wave;
      }
      else if (param.value == "sunrise"){
        deviceParam.programType = APICLG::ProgramType::sunrise;
      }
      else{
        DEBUGMLN("Invalid program-type");
        return 1;
      }
      // deviceParam.sunriseStarted = false;
      return 0;
    }
    // else if (param.name == "gradient-number" && deviceParam.gradientNumber != param.value.toInt()){
    else if (param.name == "gradient-number"){
      deviceParam.gradientNumber = param.value.toInt();
      return 0;
    }
    else if (param.name == "blend-type"){
      if (param.value == "noblend"){
        deviceParam.blendType = NOBLEND;
      }
      else if (param.value == "linearblend"){
        deviceParam.blendType = LINEARBLEND;
      }
      else if (param.value == "linearblend-nowarp"){
        deviceParam.blendType = LINEARBLEND_NOWRAP;
      }
      else {
        DEBUGMLN("Invalid blendType");
        return 1;
      }
      return 0;
    }
    else if (param.name == "scale"){
      deviceParam.scale = param.value.toInt();
      return 0;
    }
    else if (param.name == "speed"){
      deviceParam.speed = param.value.toInt();
      return 0;
    }
    else if (param.name == "delay-sunrise"){
      deviceParam.delaySunrise = param.value.toInt();

      // the beginning of the countdown for sunrise
      deviceParam.refPoint = minutes16();
      deviceParam.sunriseStarted = true;
      return 0;
    }
    

    else if (param.name == "hsv"){
      
      uint32_t hsvHex = strtol(param.value.c_str(), NULL, 16);
      uint8_t h = (hsvHex >> 16) & 0xFF;
      uint8_t s = (hsvHex >> 8) & 0xFF;
      uint8_t v = hsvHex & 0xFF;

      v = v / 255.0 * MAX_BRIGHTNESS; // brigtness limitation
      // DEBUGMLN("hsv: " + String(h) + " " + String(s) + " " + String(v));
      // if (deviceParam.hue != h || deviceParam.sat != s || deviceParam.val != v){
      deviceParam.hue = h;
      deviceParam.sat = s;
      deviceParam.val = v;
      return 0;
    }

    else {
      DEBUGMLN("Invalid param or same: " + param.name + " = " + param.value);
      DEBUGMLN("Key: " + param.name);
      DEBUGMLN("Invalid param or same: " + param.name + " = " + param.value);
      return 2;
    }
  
  }
  else {
    DEBUGMLN("Empty param"); 
    return 3;
  }
  
  return 1;
}

///@brief Creating a json document with the current parameters
///@note Function return 
///@note 0 - if success
///@note 1 - if an erroneous parameter
uint8_t APICLG::createJson(StaticJsonDocument<sizeJson> &jsonDoc)
{
  if(!jsonDoc.capacity()){
    jsonDoc.clear();
  }

  bool badBit = false;

  // if (deviceParam.role == APICLG::RoleType::begin){
  //   jsonDoc["role"] = "begin";
  // }
  // else if (deviceParam.role == APICLG::RoleType::middle){
  //   jsonDoc["role"] = "middle";
  // }
  // else if (deviceParam.role == APICLG::RoleType::end){
  //   jsonDoc["role"] = "end";
  // }
  // else{
  //   DEBUGMLN("Invalid role");
  //   badBit = true;
  // }

  // if (deviceParam.typeGate == APICLG::TypeGate::strip){
  //   jsonDoc["type-gate"] = "strip";
  // }
  // else if (deviceParam.typeGate == APICLG::TypeGate::rect){
  //   jsonDoc["type-gate"] = "rect";
  // }
  // else if (deviceParam.typeGate == APICLG::TypeGate::triangle){
  //   jsonDoc["type-gate"] = "triangle";
  // }
  // else if (deviceParam.typeGate == APICLG::TypeGate::hex){
  //   jsonDoc["type-gate"] = "hex";
  // }
  // else if (deviceParam.typeGate == APICLG::TypeGate::circle){
  //   jsonDoc["type-gate"] = "circle";
  // }
  // else{
  //   DEBUGMLN("Invalid role");
  //   badBit = true;
  // }

  // jsonDoc["offset-voltage"] = deviceParam.offsetVoltage;

  if (deviceParam.state == APICLG::StateType::off){
    jsonDoc["state"] = "off";
  }
  else if (deviceParam.state == APICLG::StateType::run){
    jsonDoc["state"] = "run";
  }
  else if (deviceParam.state == APICLG::StateType::pause){
    jsonDoc["state"] = "pause";
  }
  else if (deviceParam.state == APICLG::StateType::lowBattery){
    jsonDoc["state"] = "low-battery";
  }
  else{
    DEBUGMLN("Invalid role");
    badBit = true;
  }

  if (deviceParam.programType == APICLG::ProgramType::solid){
    jsonDoc["program-type"] = "solid";
  }
  else if (deviceParam.programType == APICLG::ProgramType::blink){
    jsonDoc["program-type"] = "blink";
  }
  else if (deviceParam.programType == APICLG::ProgramType::gradient){
    jsonDoc["program-type"] = "gradient";
  }
  else if (deviceParam.programType == APICLG::ProgramType::wave){
    jsonDoc["program-type"] = "wave";
  }
  else if (deviceParam.programType == APICLG::ProgramType::sunrise){
    jsonDoc["program-type"] = "sunrise";
  }
  else{
    DEBUGMLN("Invalid role");
    badBit = true;
  }

  //  adaptive output of parameters

  if (deviceParam.programType == APICLG::ProgramType::blink || deviceParam.programType == APICLG::ProgramType::gradient){
      jsonDoc["scale"] = deviceParam.scale;
      jsonDoc["speed"] = deviceParam.speed;
    }

  if (deviceParam.programType == APICLG::ProgramType::gradient){
    jsonDoc["gradient-number"] = deviceParam.gradientNumber;
    
    if (deviceParam.blendType == NOBLEND){
      jsonDoc["blend-type"] = "noblend";
    }
    else if (deviceParam.blendType == LINEARBLEND){
      jsonDoc["blend-type"] = "linearblend";
    }
    else if (deviceParam.blendType == LINEARBLEND_NOWRAP){
      jsonDoc["blend-type"] = "linearblend-nowarp";
    }
    else{
      DEBUGMLN("Invalid role");
      badBit = true;
    }
  }

  if (deviceParam.programType == APICLG::ProgramType::sunrise){
    jsonDoc["delay-sunrise"] = deviceParam.delaySunrise;
    jsonDoc["time-passed"] = (minutes16() - deviceParam.refPoint);
  }

  // color

  JsonArray hsvParam = jsonDoc.createNestedArray("hsv");
    hsvParam.add(deviceParam.hue);
    hsvParam.add(deviceParam.sat);
    hsvParam.add(uint8_t(deviceParam.val / (float)MAX_BRIGHTNESS * 255.0));

  // error handler
  if (badBit){
    return 1;
  }
  return 0;
}
