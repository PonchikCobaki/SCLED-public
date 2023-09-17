#include "Path_parameters.hpp"



namespace APICLG{

  const char *signatureStr = "api?";

}

APICLG::PathParameters::PathParameters(){}

APICLG::PathParameters::PathParameters(String name, String value)
{
  this->name = name;
  this->value = value;
}

const String &APICLG::PathParameters::getName()
{
  return name;
}

const String &APICLG::PathParameters::getValue()
{
  return value;
}

void APICLG::PathParameters::setName(const String &name)
{
  this->name = name;
}

void APICLG::PathParameters::setValue(const String &value)
{
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


// Format example: /api?color=1&intensity=30&role=start
// POST http://192.168.0.1/api?role=begin&type-gate=rect&state=run&program-type=solid&speed=4&hsv=fc0115
std::vector<APICLG::PathParameters> APICLG::parsePath(String path)
{
  std::vector<PathParameters> result;
  int startSliceInd = path.indexOf(signatureStr);
  if (startSliceInd != -1)
  {
    startSliceInd += 3; // cut over the "api?"
    int endSliceInd{-1};
    while (startSliceInd + 1 < path.length())
    {
      endSliceInd = path.indexOf('&', startSliceInd + 1);
      if (endSliceInd == -1 && startSliceInd + 1 < path.length())
        endSliceInd = path.length();

      if (endSliceInd != -1)
      {
        String paramStr = path.substring(startSliceInd + 1, endSliceInd);
        // DEBUGMLN("param: " + paramStr);
        PathParameters param = parseParam(paramStr);
        if (!param.getValue().isEmpty())
          result.push_back(param);
        startSliceInd = endSliceInd;
      }
      else
      {
        break;
      }
      yield();
    }
  }

  return result;
}


// Format example: color=1
APICLG::PathParameters APICLG::parseParam(String str)
{
  int delimiter = str.indexOf('=');
  if (delimiter != -1)
  {
    PathParameters param(
        str.substring(0, delimiter),
        str.substring(delimiter + 1, str.length()));
    // DEBUGMLN("param: " + param.getName() + " = " + param.getValue());
    return param;
  }
  else
  {
    return PathParameters();
  }
}

uint8_t APICLG::parseRequestType(String &req)
{
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
