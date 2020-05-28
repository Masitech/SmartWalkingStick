#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WifiEspNowBroadcast.h>
#include <WifiEspNow.h>
#include <WiFiClient.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

static const int BUTTON_PIN = 0; // "flash" button on NodeMCU, Witty Cloud, etc
static const int LED_PIN = 2;    // ESP-12F blue LED
int ledState = HIGH;
bool ota_flag = true;
uint16_t time_elapsed = 0;

//OTA Program --------------------------------------------------------------------------------------------------------------------------------
const char* ssid = "TNCAPE24DD3";
const char* pass = "EDD7C25A6C";
//OTA Program ^^------------------------------------------------------------------------------------------------------------------------------

//OTA Program --------------------------------------------------------------------------------------------------------------------------------
void arduinoOTAProgram() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
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
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
//OTA Program ^^------------------------------------------------------------------------------------------------------------------------------

//-------------------------
// Message Reciving Function
//-------------------------
void processRx(const uint8_t mac[6], const uint8_t* buf, size_t count, void* cbarg) {
  //Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (int i = 0; i < count; ++i) {
    Serial.print(static_cast<char>(buf[i]));
  }
  Serial.println();
  digitalWrite(LED_PIN, ledState);
  ledState = !ledState;  // was high now low was low now high
}

//-------------------------
// Message Sending Function
//-------------------------
void sendMessage() {
  Serial.println("Sending message");
  char msg[60];
  int len = snprintf(msg, sizeof(msg), "******Message from Watch");
  WifiEspNowBroadcast.send(reinterpret_cast<const uint8_t*>(msg), len);
  Serial.println("Sending message");
  Serial.println(msg);
  Serial.print("Recipients:");
  const int MAX_PEERS = 5;
  WifiEspNowPeerInfo peers[MAX_PEERS];
  int nPeers = std::min(WifiEspNow.listPeers(peers, MAX_PEERS), MAX_PEERS);
  for (int i = 0; i < nPeers; ++i) {
    Serial.printf(" %02X:%02X:%02X:%02X:%02X:%02X", peers[i].mac[0], peers[i].mac[1], peers[i].mac[2], peers[i].mac[3], peers[i].mac[4], peers[i].mac[5]);
  }
  Serial.println();
}
//SQL Connection --------------------------------------------------------------------------------------------------------------
WiFiServer server(80);
//IPAddress ip(192, 168, 43, 14);
//IPAddress gateway(192, 168, 43, 1);
//IPAddress subnet(255, 255, 255, 0);

WiFiClient client;
MySQL_Connection conn((Client *)&client);

char database[] = "USE aruuhntg_MA1";
char selectSQL[] = "UPDATE ArduinoJS SET Value = %d WHERE ID = 1";
char query[128];
char query2[128];

IPAddress server_addr(212,1,212,81);          // MySQL server IP
char user[] = "aruuhntg_ma";           // MySQL user
char password[] = "Masi244144";       // MySQL password
//char database[] = "aruuhntg_MA1";     // MySQL Database
//SQL Connection --------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  arduinoOTAProgram();
  //ESP Comunication --------------------------------------------------------------------------------------------------------------------------------
  WiFi.persistent(false);
  bool ok = WifiEspNowBroadcast.begin("ESPNOW", 3);
  if (!ok) {
    Serial.println("WifiEspNowBroadcast.begin() failed");
    ESP.restart();
  }
  WifiEspNowBroadcast.onReceive(processRx, nullptr);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
  Serial.print("MAC address of this node is ");
  Serial.println(WiFi.softAPmacAddress());
  Serial.println("Press the button to send a message");
  //ESP Comunication --------------------------------------------------------------------------------------------------------------------------------
  //SQL Connection --------------------------------------------------------------------------------------------------------------

  Serial.println("Connecting to database");
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(500);
    Serial.println("Running a query");
    sprintf(query2,database);
    Serial.println(query2);
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    cur_mem->execute(query2, true);
    delete cur_mem;
    //cur.execute("USE aruuhntg_MA1"); // execute a query
    //cur.show_results();            // show the results
    //cur.close();                   // close the cursor
    //conn.close();                  // close the connection
  } else {
    Serial.println("Connect failed. Trying again on next iteration.");
  }
  Serial.println("");
  Serial.println("Connected to SQL Server!");  
  //SQL Connection --------------------------------------------------------------------------------------------------------------
}

void sendMessageLoop() {
  sendMessage();
  WifiEspNowBroadcast.loop();
  delay(10000);
}

void sqlQuery(){
  int val = 25;
  //sprintf(query2,database);
  sprintf(query,selectSQL, val);


  Serial.println("Recording data.");
  Serial.println(query);
  
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  //cur_mem->execute(query2, true);
  cur_mem->execute(query, true);
 // cur_mem->show_results();
  delete cur_mem;
}

//Main Program --------------------------------------------------------------------------------------------------------------------------------
void loop() {

  if(ota_flag){
    while(time_elapsed > 1000){ //1 second
       ArduinoOTA.handle();
       time_elapsed = millis();
       delay(10);
    }
    ota_flag = false;
  }
sendMessageLoop();
sqlQuery();
}



