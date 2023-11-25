#include "API_Parser.hpp"



namespace APICLG{

  const char *signatureStr = "api?";

}

APICLG::PathParameters::PathParameters(){}

APICLG::PathParameters::PathParameters(String name, String value)
{
  this->name = name;
  this->value = value;
}



/*
    The functions of parsing an HTTP request with writing to an object.

    First line of HTTP request looks like "GET /path HTTP/1.1"
    Retrieve the "/path" part by finding the spaces */
String APICLG::pathFinding(const String &req)
{
  int addr_start = req.indexOf(' ');
  if (addr_start == -1)
  {
    DEBUGMLN("Invalid request start: " + req);
    return "";
  }

  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_end == -1)
  {
    addr_end = req.indexOf('\r', addr_start + 1);
    if (addr_end == -1)
    {
      addr_end = req.indexOf('\n', addr_start + 1);
      if (addr_end == -1)
      {
        DEBUGMLN("Invalid request end: " + req);
        return "";
      }
    }
  }

  // DEBUGMLN("Request: " + req);
  String path = req.substring(addr_start + 1, addr_end);
  // DEBUGM("Path: ");
  // DEBUGMLN(path);

  return path;
}


/*  Format example: /api?color=1&intensity=30&role=start
    return key and value
 */
uint8_t APICLG::parsePath(const String &path)
{
  
  
  int startSliceInd = path.indexOf(signatureStr);
  if (startSliceInd != -1)
  {
    startSliceInd += 3; // cut over the "api?"
    int endSliceInd{-1};
    while (startSliceInd + 1 < path.length())
    {
      endSliceInd = path.indexOf('&', startSliceInd + 1);
      if (endSliceInd == -1 && startSliceInd + 3 < path.length()) // the last parameter of the path ...&a=3
        endSliceInd = path.length();

      if (endSliceInd != -1)
      {
        String paramStr = path.substring(startSliceInd + 1, endSliceInd);
        // DEBUGMLN("param: " + paramStr);

        // parse on name-value
        PathParameters param;
        if (parseParam(param, paramStr) == 0){ 
          if(APICLG::updateDeviceParameters(param) == 0){
            // save to device parameters
            memoryDeviceParam.update();
          } else  return 1;
          
        }
        else {
          DEBUGMLN("Invalid param in parse path: " + paramStr);
          return 1;
        }

        startSliceInd = endSliceInd;
      }
      else
      {
        DEBUGMLN("Invalid param in parse path: " + path);
        return 1;
      }
      // yield();
    }
  }

  return 0;
}


// Format example: color=1
uint8_t APICLG::parseParam(APICLG::PathParameters &param, const String &str)
{
  int delimiter = str.indexOf('=');
  if (delimiter != -1)
  {
    DEBUGMLN("Param string: " + str);
    param = {
        str.substring(0, delimiter),
        str.substring(delimiter + 1, str.length()) 
    };
    // DEBUGMLN("param: " + param.getName() + " = " + param.getValue());
    return 0;
  }
  else
  {
    DEBUGMLN("Invalid param without assignment \"=\": " + str);
    return 1;
  }
}

uint8_t APICLG::parseRequestType(String &req)
{
  while (req[0] == ' ')
  {
    req = req.substring(1, req.length());
  }

  int addr_start = req.indexOf(' ');
  String reqType = req.substring(0, addr_start);
  if (reqType == "GET")
    return APICLG::GET;
  else if (reqType == "POST")
    return APICLG::POST;
  else if (reqType == "PUT")
    return APICLG::PUT;
  else if (reqType == "DELETE")
    return APICLG::DELETE;
  else return APICLG::GET;
}



