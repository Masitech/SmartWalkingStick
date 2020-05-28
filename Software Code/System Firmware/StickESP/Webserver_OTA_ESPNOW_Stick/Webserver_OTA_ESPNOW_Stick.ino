/*
  access the sample web page at 
  while connected AP http://smartstick.local
  Masum Ahmed Ma786@kent.ac.uk 
  Version 1.12 Smart Stick Code
*/
#include <WifiEspNow.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(D5, D6); // RX, TX
//#define Serial Serial
ESP8266WiFiMulti wifiMulti;

//- Buttons
const int simButton = D2;
const int button2 = D3;
const int button3 = D4;
const int button1 = D7;
const int simPin = D0; // send sms and update the sql website
unsigned long simInvoke = 0; //timer millis() - sim > xInterval
unsigned long xInterval = 250; //pin is on for 250ms


IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);

//----------------------------------------------
// MAC ADDRESS of ESP to Connect & Global Variable
//----------------------------------------------
static uint8_t PEER[] {0x5E, 0xCF, 0x7F, 0x65, 0xA6, 0x89};

char c;

const char *ssid = "SmartStick APStick"; // The name of the Wi-Fi network that will be created
const char *password = "SmartStick786";   // The password required to connect to it, leave blank for an open network
const char* host = "smartstick";
const char *OTAName = "smartstick";           // A name and a password for the OTA service
const char *OTAPassword = "SmartStick786";

ESP8266WebServer server(80);
//holds the current upload
File fsUploadFile;

//-----------------------------------------------
// Message Reciving Function
//-----------------------------------------------
void printReceivedMessage(const uint8_t mac[6], const uint8_t* buf, size_t count, void* cbarg) {
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (int i = 0; i < count; ++i) {
    Serial.print(static_cast<char>(buf[i]));
  }
  Serial.println();
  // digitalWrite(LED_PIN, HIGH);
}


//-----------------------------------------------
// Message Sending Function
//-----------------------------------------------
void sendMess(char mssg[1]) {
  char msg[60];
  int len = snprintf(msg, sizeof(msg), "%s", mssg);
  WifiEspNow.send(PEER, reinterpret_cast<const uint8_t*>(msg), len);
  delay(5);
}

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload() {
  if (server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //Serial.print("handleFileUpload Data: "); Serial.println(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}

void handleFileDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  Serial.println("handleFileDelete: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileCreate() {
  if (server.args() == 0)
    return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  Serial.println("handleFileCreate: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (SPIFFS.exists(path))
    return server.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if (file)
    file.close();
  else
    return server.send(500, "text/plain", "CREATE FAILED");
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileList() {
  if (!server.hasArg("dir")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  Serial.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  output += "]";
  server.send(200, "text/json", output);
}

void startSPIFFS() {
  SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
  Serial.println("SPIFFS started. Contents:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}

//--------------------------------------------------------ESPNOW Setup--------------------------------------------
void startESPNOW() {
  bool ok = WifiEspNow.begin();
  if (!ok) {
    Serial.println("WifiEspNow.begin() failed");
    ESP.restart();
  }

  WifiEspNow.onReceive(printReceivedMessage, nullptr);

  ok = WifiEspNow.addPeer(PEER);
  if (!ok) {
    Serial.println("WifiEspNow.addPeer() failed");
    ESP.restart();
  }
}
//---------------------------------------------------------Start WIFI --------------------------------------------
void startWiFi() {
  // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  WiFi.softAP(ssid, password);             // Start the access point
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started\r\n");
  // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("TNCAPE24DDX3", "EDD7C25A6C");   //home
  wifiMulti.addAP("SmartStickProject", "SmartStickProject786");  //custom network
  wifiMulti.addAP("SmartStick APWatch", "SmartStick786");

  Serial.println("Connecting");
  while (wifiMulti.run() != WL_CONNECTED && WiFi.softAPgetStationNum() < 1) {  // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");
  if (WiFi.softAPgetStationNum() == 0) {     // If the ESP is connected to an AP
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());             // Tell us what network we're connected to
    Serial.print("IP address:\t");
    Serial.print(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer
  } else {                                   // If a station is connected to the ESP SoftAP
    Serial.print("Station connected to ESP8266 AP");
  }
  Serial.println("\r\n");
}
//-----------------------------------------------------Start HTTP Server------------------------------------------
void startServer() {
  //SERVER INIT
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, []() {
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ", \"analog\":" + String(analogRead(A0));
    json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });
  server.begin();
  Serial.println("HTTP server started");

}

//---------------------------------------------------Over The Air Programming Setup-------------------------------
void startOTA() { // Start the OTA service
  ArduinoOTA.setHostname(OTAName);
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\r\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready\r\n");
}

//----------------------------------------------Multicast Dynamic Name Server -----------------------
void startMDNS() { // Start the mDNS responder
  MDNS.begin(host);
  Serial.print("Open http://");
  Serial.print(host);
  Serial.println(".local/edit to see the file browser");
}

//----------------------------------------------Program Setup-----------------------------------------
void setup(void) {
  Serial.begin(115200);
  Serial.print("\n");
  mySerial.begin(115200);

  pinMode(simButton, INPUT); // button 4 on schmatic
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(simPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //calls the different functions part of setup More tidy this way
  startWiFi();
  startESPNOW();
  startSPIFFS();
  startMDNS();
  startServer();
  startOTA();

}
//--------------------------------------------------------Check UART (SERIAL From Arduino)----------------------
void checkUART() {
  if (mySerial.available() > 0) {
    char ca = mySerial.read();
    Serial.println(ca);
    messageFinder(ca);
    //sendMess(static_cast<char*>(ca));
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
  if (Serial.available() > 0) {
    mySerial.write(Serial.read());
    // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}

//--------------------------------------------------------Decode Serial Message---------------------------------
//what message
void messageFinder(char c) {
  switch (c) {
    case '1':
      sendMess("1");
      break;
    case '2':
      sendMess("2");
      break;
    case '3':
      sendMess("3");
      break;
    case '4':
      sendMess("4");
      break;
    default:
      break;
  }
}

//--------------------------------------------Press Buton to make sms and sql update----------------------------
//upload location and send sms function
void invokeSim() {
  if (digitalRead(simButton)) {
    digitalWrite(simPin, HIGH);
    simInvoke = millis();
  }
  if ((millis() - simInvoke) > xInterval) {
    digitalWrite(simPin, LOW);
  }
}

//--------------------------------------------------------MAIN LOOP---------------------------------------
void loop(void) {
  checkUART();
  invokeSim(); //if pin high will call sim
  server.handleClient();                      // run the server
  ArduinoOTA.handle();                        // listen for OTA events
}
