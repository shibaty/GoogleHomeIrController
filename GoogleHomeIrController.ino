#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ArduinoJson.h>
#include "ElectronicsConst.h"
#include "WifiConfig.h"

constexpr int port = 80;
constexpr uint16_t gpiopin_ir = 14;
constexpr uint16_t khz = 38;
constexpr float interval = 5 * 60;

ESP8266WebServer server(port);
IRsend irsend(gpiopin_ir);

bool irsendForTv(String action) {

  uint32_t actionData;

  if (action == "poweron") {
    actionData = ElectronicsConst_Data::PanasonicTv_Power;
  } else
  if (action == "poweroff") {
    actionData = ElectronicsConst_Data::PanasonicTv_Power;
  } else
  if (action == "volumeup") {
    actionData = ElectronicsConst_Data::PanasonicTv_VolumeUp;
  } else
  if (action == "volumedown") {
    actionData = ElectronicsConst_Data::PanasonicTv_VolumeDown;
  } else
  if (action == "channelup") {
    actionData = ElectronicsConst_Data::PanasonicTv_ChannelUp;
  } else
  if (action == "channeldown") {
    actionData = ElectronicsConst_Data::PanasonicTv_ChannelDown;
  } else {
    Serial.println("unknown action");
    return false;
  }

  irsend.sendPanasonic(
    ElectronicsConst_Data::PanasonicTv_Address,
    actionData
  );

  return true;
}

bool irsendForLight(String action) {

  uint16_t* raw;
  uint16_t size;

  if (action == "poweron") {
    raw = const_cast<uint16_t*>(ElectronicsConst_RAW::LightOn);
    size = sizeof(ElectronicsConst_RAW::LightOn) / sizeof(uint16_t);
  } else
  if (action == "poweroff") {
    raw = const_cast<uint16_t*>(ElectronicsConst_RAW::LightOff);
    size = sizeof(ElectronicsConst_RAW::LightOff) / sizeof(uint16_t);
  } else {
    Serial.println("unknown action");
    return false;
  }

  irsend.sendRaw(raw, size, khz);
  return true;
}

bool irsendForAc(String action) {

  uint16_t* raw;
  uint16_t size;

  if (action == "heateron") {
    raw = const_cast<uint16_t*>(ElectronicsConst_RAW::AcHeaterOn);
    size = sizeof(ElectronicsConst_RAW::AcHeaterOn) / sizeof(uint16_t);
  } else
  if (action == "acon") {
    raw = const_cast<uint16_t*>(ElectronicsConst_RAW::AcAcOn);
    size = sizeof(ElectronicsConst_RAW::AcAcOn) / sizeof(uint16_t);
  } else
  if (action == "poweroff") {
    raw = const_cast<uint16_t*>(ElectronicsConst_RAW::AcPowerOff);
    size = sizeof(ElectronicsConst_RAW::AcPowerOff) / sizeof(uint16_t);
  } else {
    Serial.println("unknown action");
    return false;
  }

  irsend.sendRaw(raw, size, khz);
  return true;
}

bool irsendForSpeaker(String action) {

  uint64_t actionData;

  if (action == "poweron") {
    actionData = ElectronicsConst_Data::Speaker_Power;
  } else
  if (action == "poweroff") {
    actionData = ElectronicsConst_Data::Speaker_Power;
  } else {
    Serial.println("unknown action");
    return false;
  }

  irsend.sendNEC(actionData);
  return true;
}

bool irsendForRoomba(String action) {

  uint16_t* raw;
  uint16_t size;

  if (action == "poweron") {
    raw = const_cast<uint16_t*>(ElectronicsConst_RAW::RoombaStart);
    size = sizeof(ElectronicsConst_RAW::RoombaStart) / sizeof(uint16_t);
  } else {
    Serial.println("unknown action");
    return false;
  }

  irsend.sendRaw(raw, size, khz);
  return true;
}

bool irsendForSampleAc(String action) {

  uint16_t* raw;
  uint16_t size;

  if (action == "heateron") {
    raw = const_cast<uint16_t*>(ElectronicsConst_RAW::SampleAcHeaterOn);
    size = sizeof(ElectronicsConst_RAW::SampleAcHeaterOn) / sizeof(uint16_t);
  } else
  if (action == "poweroff") {
    raw = const_cast<uint16_t*>(ElectronicsConst_RAW::SampleAcPowerOff);
    size = sizeof(ElectronicsConst_RAW::SampleAcPowerOff) / sizeof(uint16_t);
  } else {
    Serial.println("unknown action");
    return false;
  }

  irsend.sendRaw(raw, size, khz);
  return true;
}

void handleCtl() {
  StaticJsonBuffer<256> jsonBuffer;

  JsonObject& jsonRoot = jsonBuffer.parseObject(server.arg("plain"));

  if (!jsonRoot.success()) {
    Serial.println("json parse error.");
    server.send(400, "text/plain", "Bad Request");
    return;
  }

  String target = jsonRoot["target"];
  String action = jsonRoot["action"];

  Serial.print("target: ");
  Serial.print(target);
  Serial.print(" action:");
  Serial.println(action);

  bool result = false;
  if (target == "tv") {
    result = irsendForTv(action);
  } else
  if(target == "light") {
    result = irsendForLight(action);
  } else
  if(target == "ac") {
    result = irsendForAc(action);
  } else
  if(target == "speaker") {
    result = irsendForSpeaker(action);
  } else
  if(target == "roomba") {
    result = irsendForRoomba(action);
  } else
  if(target == "sampleac") {
    result = irsendForSampleAc(action);
  } else {
    Serial.println("unknown target");
  }

  if (result) {
    server.send(200, "text/plain", "Success");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void handleRoot() {
  server.send(200);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found.");
}

void setup() {
  // Serial initialize
  Serial.begin(115200);
  delay(1000);

  // WiFi initialize
  WiFi.mode(WIFI_STA);
  WiFi.begin(WifiConfig::ssid, WifiConfig::password);
  
  // wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WifiConfig::ssid);

  // set static ip address
  IPAddress ipAddr;
  ipAddr.fromString(WifiConfig::ipAddr);
  WiFi.config(ipAddr, WiFi.gatewayIP(), WiFi.subnetMask()); 
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // set WebServer handle
  server.on("/", handleRoot);
  server.on("/ctl", HTTP_POST, handleCtl);
  server.onNotFound(handleNotFound);

  // start
  irsend.begin();
  server.begin();
  Serial.println("WebServer Started.");
}

void loop() {
  server.handleClient();
}
