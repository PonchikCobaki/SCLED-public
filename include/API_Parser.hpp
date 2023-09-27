#pragma once
#include <Stream.h>
#include "settings.hpp"
#include "Path_parameters.hpp"
#include "DeviceParameters.hpp"



namespace APICLG {

  extern const char *signatureStr;
  
  enum RequestType {
    GET,
    POST,
    PUT,
    DELETE,
    ERROR,
    NONE
  };


  uint8_t parseRequestType(String &req);
  String pathFinding(const String &req);
  uint8_t parsePath(const String &path);
  uint8_t parseParam(APICLG::PathParameters &param, const String &str);

}
