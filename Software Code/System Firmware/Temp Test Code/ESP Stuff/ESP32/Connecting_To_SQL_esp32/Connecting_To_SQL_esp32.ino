#include <WiFiClient.h>
#include <MySQL_Cursor.h>
//#include <DHT.h>
#include <MySQL_Connection.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#define sensorPin1 4 //gpio4 d2

//#define sensorPin2 D2
//#define typeDHT DHT11
//DHT dht(sensorPin1, typeDHT);

//char ssid[] = "EngProWifi";                 // Network Name
//char pass[] = "EngProject2";                 // Network Password

char ssid[] = "TNCAPE24DD3";                 // Network Name
char pass[] = "EDD7C25A6C";                 // Network Password

byte mac[6];

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


//MySQL_Cursor cur = MySQL_Cursor(&conn);
void setup() {

  Serial.begin(9600);

  pinMode(sensorPin1, INPUT);
  //pinMode(sensorPin2, INPUT);

    // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  Serial.println("");
  Serial.print("Assigned IP: ");
  Serial.print(WiFi.localIP());
  Serial.println("");

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

}

void loop() {

  int val = analogRead(sensorPin1);
  //float t = dht.readTemperature();

  //Serial.println(t);

  delay(1000); //5 sec
  
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
