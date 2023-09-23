#include "mDNS_Server.hpp"


namespace APICLG {

  const char* ssid = STASSID;
  const char* password = STAPSK;


  // TCP server at port 80 will respond to HTTP requests
  WiFiServer connectedServer(80);

}


APICLG::HTTPParameters::HTTPParameters(){}

APICLG::HTTPParameters::HTTPParameters(String request)
{
  parseRequest(request);
}

APICLG::HTTPParameters::HTTPParameters(std::vector<PathParameters> &parameters)
{
  this->_parameters = parameters;
}


// to do
// const String APICLG::HTTPParameters::responseRequest()
// {
//   if (_reqType == RequestType::GET) {
//     _answer = "HTTP/1.1 200 OK\r\n";
//     // _answer += "Content-Type: application/json\r\n";
//     // _answer += "Connection: close\r\n";
//     // _answer += "Content-Length: \r\n" + String(measureJsonPretty(doc));
//     _answer += "\r\n";
//     // _answer += "\r\n\r\n";
//   }
//   else if (_reqType == RequestType::POST) {
//     _answer = "HTTP/1.1 201 Created\r\n";
//     // _answer += "Content-Type: application/json\r\n";
//     // _answer += "Connection: close\r\n";
//     // _answer += "Content-Length: \r\n" + String(measureJsonPretty(doc));
//     _answer += "\r\n";
//     // _answer += "\r\n\r\n";
//   }
//   return _answer;
// }

// void APICLG::HTTPParameters::setAnswer(const String &answer)
// {
//   this->_answer = answer;
// }

const std::vector<APICLG::PathParameters> &APICLG::HTTPParameters::getParameters()
{
  return this->_parameters;
}

const uint8_t &APICLG::HTTPParameters::getRequestType()
{
  return this->_reqType;
}

// const String &APICLG::HTTPParameters::getAnswer()
// {
//   return this->_answer;
// }

void APICLG::HTTPParameters::parseRequest(String &request)
{
  this->_parameters = parsePath(pathFinding(request));
  this->_reqType = parseRequestType(request);
}


uint8_t APICLG::mDNSServerInit(void) {
  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(APICLG::ssid, APICLG::password);
  DEBUGMLN("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUGM(".");
  }
  DEBUGMLN("");
  DEBUGM("Connected to ");
  DEBUGMLN(ssid);
  DEBUGM("IP address: ");
  DEBUGMLN(WiFi.localIP());

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("esp8266")) {
    DEBUGMLN("Error setting up MDNS responder!");
    return 0;
  }
  DEBUGMLN("mDNS responder started");

  // Start TCP (HTTP) server
  APICLG::connectedServer.begin();
  DEBUGMLN("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);

  return 1;
}


/* APICLG::RequestType APICLG::serverUpdate(HTTPParameters &paramReq, DeviceParameters &curParam) {

  MDNS.update();

  // Check if a client has connected
  
  WiFiClient client = APICLG::connectedServer.accept();
  if (client) {
    DEBUGMLN("");
    DEBUGMLN("New client");
    
    // Wait for data from client to become available
    while ((client.connected() && !client.available())) {
      yield();
    }
    
    // Read the first line of HTTP request
    String req = client.readStringUntil('\r');
    // DEBUGMLN("Request from client: " + req);
    // client.flush();
  
    // Parse HTTP request
    paramReq.parseRequest(req);

    // Answer to client
    StaticJsonDocument<192> doc;
    doc["role"] = String(curParam.role);
    doc["type-gate"] = String(curParam.typeGate);
    doc["state"] = String(curParam.state);
    doc["program-type"] = String(curParam.programType);
    doc["speed"] = String(curParam.speed);

    JsonArray hsvParam = doc.createNestedArray("hsv");
    hsvParam.add(String(curParam.colorHSV.hue));
    hsvParam.add(String(curParam.colorHSV.sat));
    hsvParam.add(String(curParam.colorHSV.value));


    String answer;
    if (paramReq.getRequestType() == APICLG::GET)
    {
      client.println(F("HTTP/1.0 200 OK"));
      client.println(F("Content-Type: application/json"));
      client.println(F("Connection: close"));
      client.print(F("Content-Length: "));
      client.println(measureJsonPretty(doc));
      client.println();
      // answer = "HTTP/1.1 200 OK\r\n";
      // answer += "Content-Type: application/json\r\n";
      // answer += "Connection: close\r\n";
      // answer += "Content-Length: " + String(measureJsonPretty(doc)) + "\r\n";
      // answer += "\r\n";
      // client.println(answer);
      // client.println();
      serializeJsonPretty(doc, client);
      // DEBUGMLN("Answer sent to client" + answer);
      client.stop();
      DEBUGMLN("Done with client");
      return APICLG::GET;
    }
    else if (paramReq.getRequestType() == APICLG::POST)
    {
      if (paramReq.getParameters().size() == 0){
        DEBUGMLN("Error: POST request without parameters");
        return RequestType::none;
      }

      // to do
      client.stop();
      return APICLG::POST;
    }
  }

  return RequestType::none;
} */

// String answer;
// if (param.requestType() == "GET")
// {
//   // Todo GET value
//   answer = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{";
//   for (auto parameter : param.getParameters()) {
//     // !!! Todo
//     if (parameter.getName() == "color") {
//       DEBUGMLN("Todo get: color = " + parameter.getValue());
//       answer += "\"color\": \"red\",";
//     } else if (parameter.getName() == "intensity") {
//       DEBUGMLN("Todo get: intensity = " + parameter.getValue());
//       answer += "\"intensity\": \"45\",";
//     } else if (parameter.getName() == "role") {
//       DEBUGMLN("Todo get: role = " + parameter.getValue());
//       answer += "\"role\": \"slave\",";
//     }
//   }
//   answer += "}\r\n\r\n";
// } 
// else if (req.indexOf("POST") != -1) 
// {
//   // Todo SET value. Don't forget about answer!!!
//   for (auto parameter : param.getParameters()) {
//     // !!! Todo
//     if (parameter.getName() == "color") {
//       DEBUGMLN("Todo set: color = " + parameter.getValue());
//     } else if (parameter.getName() == "intensity") {
//       DEBUGMLN("Todo set: intensity = " + parameter.getValue());
//     } else if (parameter.getName() == "role") {
//       DEBUGMLN("Todo set: role = " + parameter.getValue());
//     }
//   }
// } else {
//   answer = "HTTP/1.1 404 Not Found\r\n\r\n";
// }
// client.print(answer);