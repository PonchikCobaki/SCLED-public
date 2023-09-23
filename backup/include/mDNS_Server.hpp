#pragma once


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "Path_parameters.hpp"
#include "DeviceParameters.hpp"



namespace APICLG {

  extern const char* ssid;
  extern const char* password;

  // TCP server at port 80 will respond to HTTP requests
  extern WiFiServer connectedServer;

  enum parameterName{
    role,
    typeGate,
    state,
    programType,
    speed,
    color
  };

  class HTTPParameters {
    private:

      std::vector<APICLG::PathParameters> _parameters;
      uint8_t _statusCode;
      uint8_t _reqType;
      // String _answer;

    public:

      HTTPParameters();
      HTTPParameters(String request);
      HTTPParameters(std::vector<PathParameters> &parameters);

      void parseRequest(String &request);
      const String responseRequest();

      // void setAnswer(const String &answer);

      const std::vector<APICLG::PathParameters> &getParameters();
      const uint8_t &getRequestType();
      // const String &getAnswer();

  };

  uint8_t mDNSServerInit(void);
  APICLG::RequestType serverUpdate(HTTPParameters &paramReq, DeviceParameters &curParam);


} // namespace APICLG
