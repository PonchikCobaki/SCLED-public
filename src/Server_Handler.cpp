#include "Server_Handler.hpp"



namespace APICLG {

  const char* ssid = STASSID;
  const char* password = STAPSK;



  // TCP server at port 80 will respond to HTTP requests
  WiFiServer connectedServer(80);

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
  uint16_t chipId = ESP.getChipId();
  String name = "esp8266_" + String(chipId);
  DEBUGMLN("Device name: " + name);
  if (!MDNS.begin(name)) {
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

APICLG::RequestType APICLG::serverUpdate() {

  MDNS.update();

  // Check if a client has connected
  
  WiFiClient client = APICLG::connectedServer.accept();
  if (client) {
    DEBUGMLN("");
    DEBUGMLN("New client");
    
    // Wait for data from client to become available

    uint8_t attemp = 5;
    
    for (uint8_t i = 0; i < attemp || (client.connected() && !client.available()); i++ ) {
      yield();
      if (i == attemp) {
        client.stop();
        DEBUGMLN("No data from client");
        return RequestType::ERROR;
      }
    }
    

    // Read the first line of HTTP request
    String req = client.readStringUntil('\r');
    // DEBUGMLN("Request from client: " + req);
    client.flush();
  
    // Parse HTTP request
    uint8_t reqType = APICLG::parseRequestType(req);
    uint8_t stateBit = 0;
    
    String answer;

    switch (reqType)
    {
    case RequestType::GET:
    {
      stateBit = parsePath(pathFinding(req));
      
      // Answer to client
      if (stateBit == 0){
        // Json buffer
        StaticJsonDocument<sizeJson> doc;
        createJson(doc);
        
        answer = "HTTP/1.1 200 OK\r\n";
        answer += "Content-Type: application/json\r\n";
        answer += "Content-Length: " + String(measureJsonPretty(doc)) + "\r\n";
        answer += "Connection: close\r\n";
        answer += "\r\n";
        client.print(answer);
        
        serializeJsonPretty(doc, client);
        client.stop();
        serializeJsonPretty(doc, Serial);

      } else {
        answer = "HTTP/1.1 400 Bad Request\r\n";
        answer += "\r\n";
        client.print(answer);
      }

      DEBUGMLN("\nAnswer sent to client: " + answer);
      return APICLG::GET;
      break;
    }

    case RequestType::POST:
      stateBit = parsePath(pathFinding(req));
      if (stateBit == 0){
        answer = "HTTP/1.1 200 OK\r\n";
      } 
      else answer = "HTTP/1.1 400 Bad Request\r\n";

      answer += "Connection: close\r\n";
      answer += "\r\n";
      client.print(answer);
      DEBUGMLN("Answer sent to client: " + answer);

      return APICLG::POST;
      break;

    case RequestType::PUT:
      answer = "HTTP/1.1 500 Internal Error\r\n\r\n";
      client.print(answer);
      DEBUGMLN("Sending 500 Internal Error");

      return APICLG::PUT;
      break;

    case RequestType::DELETE:
      answer = "HTTP/1.1 500 Internal Error\r\n\r\n";
      client.print(answer);
      DEBUGMLN("Sending 500 Internal Error");

      return APICLG::DELETE;
      break;

    default:
      answer = "HTTP/1.1 400 Bad Request\r\n\r\n";
      client.print(answer);
      DEBUGMLN("Sending 400 Bad Request");

      return RequestType::NONE;
      break;
    }
    
    
    // if (reqType == APICLG::GET)
    // {
      // client.println(F("HTTP/1.0 200 OK"));
      // client.println(F("Content-Type: application/json"));
      // client.println(F("Connection: close"));
      // client.print(F("Content-Length: "));
      // client.println(measureJsonPretty(doc));
      // client.println();

      // answer = "HTTP/1.1 200 OK\r\n";
      // answer += "Content-Type: application/json\r\n";
      // answer += "Connection: close\r\n";
      // answer += "Content-Length: " + String(measureJsonPretty(doc)) + "\r\n";
      // answer += "\r\n";
      // client.println(answer);
      // client.println();
      // serializeJsonPretty(doc, client);
      // DEBUGMLN("Answer sent to client" + answer);
      // client.stop();
      // DEBUGMLN("Done with client");
      // return APICLG::GET;
    // }
    // else if (reqType == APICLG::POST)
    // {
    //   if (paramReq.getParameters().size() == 0){
    //     DEBUGMLN("Error: POST request without parameters");
    //     return RequestType::NONE;
    //   }

    //   // to do
    //   client.stop();
    //   return APICLG::POST;
    // }

  } 
  return RequestType::NONE;
}