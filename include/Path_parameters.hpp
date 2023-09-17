#pragma once

#include <Stream.h>
#include <vector>
#include "settings.hpp"





namespace APICLG {

  extern const char *signatureStr;
  
  enum RequestType {
    GET,
    POST,
    PUT,
    DELETE,
    none
  };

  class PathParameters {
    private:

    String name;
    String value;
    
    
    public:
    
    PathParameters();
    PathParameters(String name, String value);
    
    const String &getName();
    const String &getValue();

    void setName(const String &name);
    void setValue(const String &value);

  };

  String pathFinding(const String &req);
  std::vector<PathParameters> parsePath(String path);
  PathParameters parseParam(String str);
  uint8_t parseRequestType(String &req);
  
}
