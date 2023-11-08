#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#include "API_Parser.hpp"

extern char* pcHostDomain;   

namespace APICLG{
  extern const char* ssid;
  extern const char* password;

  // TCP server at port 80 will respond to HTTP requests
  extern WiFiServer connectedServer;

  extern const uint16_t sizeJson;

  uint8_t mDNSServerInit(void);
  APICLG::RequestType serverUpdate();

}

extern APICLG::DeviceParameters deviceParam;

