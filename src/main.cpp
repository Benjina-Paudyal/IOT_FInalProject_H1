

// Code adapted from https://randomnerdtutorials.com/
// Consulted ChatGPT for guidance on handling edge cases


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
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h> 


// Some Global variables

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String formattedDate;
String dayStamp;
String timeStamp;

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

// variables to store temperature
String temperature = "";

// timer variables 
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

// network credentials

const char* ssid = "BudhhawasborninNepal";
const char* password = "sagarmatha";

// const char* ssid = "E308";
// const char* password = "98806829";

// Creating AsyncWebServer object on port 80 
AsyncWebServer server(80);

// creating WebSocket instance
AsyncWebSocket ws("/ws");

//Timestamp 
String getTimeStamp(){
  while(!timeClient.update()){
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();

  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0,splitT);

  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);

  return timeStamp;
}


String readDSTemperatureC() {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempCByIndex(0);

  if(tempC == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  } else {
    Serial.print("Temperature Celsius: ");
    Serial.println(tempC); 
  }
  return String(tempC);
}


String getSensorReadings() {

    String tempData = getTimeStamp();
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);

  if (tempC == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";  // Return an empty JSON object
  } else {
    Serial.print("Temperature Celsius: ");
    Serial.println(tempC);
  }
  
 
  // Create a JSON object with the temperature
  JSONVar jsonDocument;  
  jsonDocument[tempData] = tempC;
  String jsonString = JSON.stringify(jsonDocument);
  Serial.println(jsonString);
  return jsonString;
}


void initSPIFFS()
{
  if(!SPIFFS.begin(true)){
    Serial.println("An error has occured while mounting SPIFFS ");
  }
   Serial.println("Spiffs mounted successfully ");
}

// Initialize wifi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void notifyClients(const String& message) {
  ws.textAll(message);
}


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    String sensorReadings = getSensorReadings();
    Serial.print(sensorReadings);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}


//web scoket 
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


//Write File in SD card 
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message) && file.print('\n')){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

//Append to file on SD
void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
   if(file.print(message) && file.print('\n')){
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

//If File exist append else write to SD
void WriteOrAppendToFile(fs::FS &fs, const char * path, String message){
  File file = fs.open(path);
  message += ",\n";
  if(!file){
    writeFile(fs,path,message.c_str());
  }
  else{
    appendFile(fs,path,message.c_str());
  }
}


//Delete File from SD
void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
 
}



void setup()
{
  // serial port for debugging purpose
  Serial.begin(115200);


  if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  initWiFi();

  initSPIFFS();
 
  initWebSocket();

 
   // Route for root / web page
   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

   server.on("/data.csv", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/data.csv ", "text/plain");
  });

  server.on("/data.csv", HTTP_DELETE, [](AsyncWebServerRequest *request){
    Serial.println("here");
    deleteFile(SD,"/data.csv");

    request->send(200,String(), "File is deleted ,yes ");
  });

  // Start server
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}

void loop(){

    if ((millis() - lastTime) > timerDelay) {

     temperature = getSensorReadings();
     WriteOrAppendToFile(SD, "/data.csv", temperature);
    notifyClients(temperature);
    lastTime = millis();
  }
  ws.cleanupClients();
}