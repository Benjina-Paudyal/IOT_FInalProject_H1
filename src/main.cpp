
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include <FS.h>
#include "SPIFFS.h"
#include <SPI.h>
#include <AsyncWebSocket.h> 
#include <Arduino_JSON.h> 
#include <SD.h>


/****************************************   WIFI MANAGER ASYNC WEBSERVER  **********************************************/

// // Create AsyncWebServer object on port 80
// AsyncWebServer server(80);

// // Search for parameters in HTTP POST request
// const char* PARAM_INPUT_1 = "ssid";
// const char* PARAM_INPUT_2 = "pass";
// const char* PARAM_INPUT_3 = "ip";
// const char* PARAM_INPUT_4 = "gateway";

// // Variables to save values from HTML form
// String ssid;
// String pass;
// String ip;

// String gateway;

// // File paths to save input values permanently
// const char* ssidPath = "/ssid.txt";
// const char* passPath = "/pass.txt";
// const char* ipPath = "/ip.txt";
// const char* gatewayPath = "/gateway.txt";

// IPAddress localIP;
// IPAddress localGateway;
// IPAddress subnet(255, 255, 0, 0);

// // Timer variables
// unsigned long previousMillis = 0;
// const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

// // Set LED GPIO
// const int ledPin = 2;
// String ledState;

// // Set your reset button GPIO pin
// const int resetButtonPin = 18;  // Change this to the actual GPIO pin you are using for the button
// unsigned long buttonPressedStartTime = 0;
// const long resetButtonDuration = 10000;  // 10 seconds

// // Initialize SPIFFS
// void initSPIFFS() {
//   if (!SPIFFS.begin(true)) {
//     Serial.println("An error has occurred while mounting SPIFFS");
//   }
//   Serial.println("SPIFFS mounted successfully");
// }

// // Read File from SPIFFS
// String readFile(fs::FS &fs, const char * path){
//   Serial.printf("Reading file: %s\r\n", path);

//   File file = fs.open(path);
//   if(!file || file.isDirectory()){
//     Serial.println("- failed to open file for reading");
//     return String();
//   }
  
//   String fileContent;
//   while(file.available()){
//     fileContent = file.readStringUntil('\n');
//     break;     
//   }
//   return fileContent;
// }

// // Write file to SPIFFS
// void writeFile(fs::FS &fs, const char * path, const char * message){
//   Serial.printf("Writing file: %s\r\n", path);

//   File file = fs.open(path, FILE_WRITE);
//   if(!file){
//     Serial.println("- failed to open file for writing");
//     return;
//   }
//   if(file.print(message)){
//     Serial.println("- file written");
//   } else {
//     Serial.println("- write failed");
//   }
// }

// // Initialize WiFi
// bool initWiFi() {
//   if(ssid == "" || ip == "") {
//     Serial.println("Undefined SSID or IP address.");
//     return false;
//   }

//   WiFi.mode(WIFI_STA);
//   localIP.fromString(ip.c_str());
//   localGateway.fromString(gateway.c_str());

//   if (!WiFi.config(localIP, localGateway, subnet)){
//     Serial.println("STA Failed to configure");
//     return false;
//   }
  
//   WiFi.begin(ssid.c_str(), pass.c_str());
//   Serial.println("Connecting to WiFi...");

//   unsigned long currentMillis = millis();
//   previousMillis = currentMillis;

//   while(WiFi.status() != WL_CONNECTED) {
//     currentMillis = millis();
//     if (currentMillis - previousMillis >= interval) {
//       Serial.println("Failed to connect.");
//       return false;
//     }
//   }

//   Serial.println(WiFi.localIP());
//   return true;
// }

// // Replaces placeholder with LED state value
// String processor(const String& var) {
//   if(var == "STATE") {
//     if(digitalRead(ledPin)) {
//       ledState = "ON";
//     }
//     else {
//       ledState = "OFF";
//     }
//     return ledState;
//   }
//   return String();
// }

// void checkResetButton() {
//   // Check if the reset button is pressed
//   if (digitalRead(resetButtonPin) == LOW) {
//     // Button is pressed, start the timer
//     if (buttonPressedStartTime == 0) {
//       buttonPressedStartTime = millis();
//     }
//   } else {
//     // Button is released, reset the timer
//     buttonPressedStartTime = 0;
//   }

//   // Check if the button has been pressed for the specified duration
//   if (buttonPressedStartTime > 0 && millis() - buttonPressedStartTime >= resetButtonDuration) {
//     // Button has been pressed for the specified duration, initiate reset
//     Serial.println("Reset button pressed for 10 seconds. Performing reset...");

//     // Close any open files or connections before restarting
//     SPIFFS.end();

//     // Delay for a moment (optional)
//     delay(1000);

//     // Restart the ESP32
//     ESP.restart();
//   }
// }

// void setup() {
//   // Serial port for debugging purposes
//   Serial.begin(115200);

//   initSPIFFS();

//   // Set GPIO 2 as an OUTPUT
//   pinMode(ledPin, OUTPUT);
//   digitalWrite(ledPin, LOW);

//   // Load values saved in SPIFFS
//   ssid = readFile(SPIFFS, ssidPath);
//   pass = readFile(SPIFFS, passPath);
//   ip = readFile(SPIFFS, ipPath);
//   gateway = readFile(SPIFFS, gatewayPath);
//   Serial.println(ssid);
//   Serial.println(pass);
//   Serial.println(ip);
//   Serial.println(gateway);

//   if(initWiFi()) {
//     // Route for root / web page
//     server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
//       request->send(SPIFFS, "/index.html", "text/html", false, processor);
//     });
//     server.serveStatic("/", SPIFFS, "/");
    
//     // Route to set GPIO state to HIGH
//     server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
//       digitalWrite(ledPin, HIGH);
//       request->send(SPIFFS, "/index.html", "text/html", false, processor);
//     });

//     // Route to set GPIO state to LOW
//     server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
//       digitalWrite(ledPin, LOW);
//       request->send(SPIFFS, "/index.html", "text/html", false, processor);
//     });
//     server.begin();
//   }
//   else {
//     // Connect to Wi-Fi network with SSID and password
//     Serial.println("Setting AP (Access Point)");
//     // NULL sets an open Access Point
//     WiFi.softAP("ESP-Benjina", NULL);

//     IPAddress IP = WiFi.softAPIP();
//     Serial.print("AP IP address: ");
//     Serial.println(IP); 

//     // Web Server Root URL
//     server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//       request->send(SPIFFS, "/wifimanager.html", "text/html");
//     });
    
//     server.serveStatic("/", SPIFFS, "/");
    
//     server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
//       int params = request->params();
//       for(int i=0;i<params;i++){
//         AsyncWebParameter* p = request->getParam(i);
//         if(p->isPost()){
//           // HTTP POST ssid value
//           if (p->name() == PARAM_INPUT_1) {
//             ssid = p->value().c_str();
//             Serial.print("SSID set to: ");
//             Serial.println(ssid);
//             // Write file to save value
//             writeFile(SPIFFS, ssidPath, ssid.c_str());
//           }
//           // HTTP POST pass value
//           if (p->name() == PARAM_INPUT_2) {
//             pass = p->value().c_str();
//             Serial.print("Password set to: ");
//             Serial.println(pass);
//             // Write file to save value
//             writeFile(SPIFFS, passPath, pass.c_str());
//           }
//           // HTTP POST ip value
//           if (p->name() == PARAM_INPUT_3) {
//             ip = p->value().c_str();
//             Serial.print("IP Address set to: ");
//             Serial.println(ip);
//             // Write file to save value
//             writeFile(SPIFFS, ipPath, ip.c_str());
//           }
//           // HTTP POST gateway value
//           if (p->name() == PARAM_INPUT_4) {
//             gateway = p->value().c_str();
//             Serial.print("Gateway set to: ");
//             Serial.println(gateway);
//             // Write file to save value
//             writeFile(SPIFFS, gatewayPath, gateway.c_str());
//           }
//         }
//       }
//       request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
//       delay(3000);
//       ESP.restart();
//     });
//     server.begin();

//       // Set the reset button pin as INPUT_PULLUP
//   pinMode(resetButtonPin, INPUT_PULLUP);

//   }
// }
// void loop() {
//   checkResetButton();
// }

/********************************************'    websocket ****************************************/


// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

// variables to store temperature
String temperature = "";

// timer variables 
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// network credentials

const char* ssid = "BudhhawasborninNepal";
const char* password = "sagarmatha";

// const char* ssid = "E308";
// const char* password = "98806829";

// Creating AsyncWebServer object on port 80 
AsyncWebServer server(80);

// creating WebSocket instance
AsyncWebSocket ws("/ws");

// Json Variable to Hold Sensor Readings
JSONVar readings;

void notifyClients(const String& message) {
  ws.textAll(message);
}


// Function to get sensor readings and return JSON object
String getSensorReadings() {
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempCByIndex(0);

  if (tempC == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return "{}";  // Return an empty JSON object
  } else {
    Serial.print("Temperature Celsius: ");
    Serial.println(tempC);
  }

  // Create a JSON object with the temperature
  JSONVar jsonDocument;  
  jsonDocument["temperature"] = tempC;

  String jsonString = JSON.stringify(jsonDocument);
  return jsonString;
}



void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket client connected");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket client disconnected");
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    String message = (char *)data;
    if (message == "getReadings") {
      String sensorReadings = getSensorReadings();
      notifyClients(sensorReadings);
    }
  }
}

void setup()
{
  // serial port for debugging purpose
  Serial.begin(115200);
  Serial.println();

  // testing if it can read the file or not?
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // start up the DS18B20 library
  sensors.begin();
  temperature = getSensorReadings();


 // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

   // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/temperaturec", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperature.c_str());
  });
  
  // WebSocket route
  server.addHandler(&ws);
  ws.onEvent(onWsEvent);

  // Start server
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}

void loop(){
  if ((millis() - lastTime) > timerDelay) {
     temperature = getSensorReadings();
    notifyClients(temperature);
    lastTime = millis();
  }
  ws.cleanupClients();
}