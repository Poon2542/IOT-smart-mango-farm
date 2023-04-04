// -------------------AWS---------------------- //

//Include Lib for Arduino to Nodemcu
#include <SoftwareSerial.h>
#include <ArduinoJson.h>


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#ifndef STASSID
#define STASSID "iPhone" //<YOUR-WIFI-NETWORK>
#define STAPSK "66666666" //<YOUR-NETWORK-PASSWORD>
#endif

// ----------------------------------------- //

//D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D6, D5);


//struct
//{
// uint32_t total;
// uint32_t ok;
// uint32_t crc_error;
// uint32_t time_out;
// uint32_t connect;
// uint32_t ack_l;
// uint32_t ack_h;
// uint32_t unknown;
//}

//counter = { 0,0,0,0,0,0,0,0};


// ------------------------Connect AWS------------------------------- //

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "nsorxdk0tg.execute-api.ap-southeast-1.amazonaws.com"; //<YOUR-API-GATEWAY-ENDPOINT>.execute-api.<YOUR-REGION>.amazonaws.com do not include "https://"
String url = "/DepAwithKey"; //<YOUR-API-GATEWAY-DEPLOYMENT-NAME>

const char* API_KEY = "LGTKaPTKpF8YnQNBSFVu63VMjsvLU5hK7glOMoC3";

const int httpsPort = 443;
unsigned long uptime;

// Use web browser to view and copy SHA1 fingerprint of the certificate
//to acquire the thumbprint for this webpage, go to the breadcrumbs in the upper right corner of your browser.
//Then go to Tools-->developer tools-->security-->view certificate-->details(tab)-->thumbprint
//const char fingerprint[] PROGMEM = "98 f8 5e fc 87 65 43 5f 0f c1 1e fe e9 81 c9 9c c2 43 27 4c"; //example thumbprint with proper formatting

const char fingerprint[] PROGMEM = "fb 5c 8a 7c 03 5e 05 55 07 2d 9c f4 c9 3f 24 4f 07 f2 5d 4d"; //<YOUR-SHA-THUMBPRINT>

WiFiClientSecure client;

// --------------------------------------------------------- //

uint32_t soilhumid = 0;
uint32_t humid = 0;
uint32_t temp = 0;
void setup()
{
  //ตัววัดความชื้นดิน
  
  
  // Initialize Serial port
  Serial.begin(9600);
  nodemcu.begin(9600);
  //while (!Serial) continue;
  Serial.println("Begin");
  
  
  //connect to server
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    Serial.print("connecting to ");
    Serial.println(host);
    Serial.printf("Using fingerprint '%s'\n", fingerprint);
    client.setFingerprint(fingerprint);
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //String url = "/dep1";
  
  Serial.print("requesting URL: ");
  Serial.println(url);
}


void loop()
{
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject(nodemcu);
  
  if (data == JsonObject::invalid()) {
  //Serial.println("Invalid Json Object");
  jsonBuffer.clear();
  return;
  }
  
  Serial.println("JSON Object Recieved");
  Serial.print("Recieved Soil: ");
  soilhumid = data["timetable"];
  Serial.println(soilhumid);
  Serial.print("Recieved Humidity: ");
  humid = data["humidity"];
  Serial.println(humid);
  Serial.print("Recieved Temperature: ");
  temp = data["temperature"];
  Serial.println(temp);
  Serial.println("-----------------------------------------");
  
  
  client.print(String("GET ") + url + "/?soilhumid=" + (String) soilhumid
  + "&temperature=" + (String) temp + "&humidity=" + (String) humid + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "x-api-key: " + API_KEY + "\r\n" +
  "User-Agent: 14 ESP8266\r\n" +
  "Connection: close\r\n\r\n");
  
  Serial.println("request sent");
  while (client.connected()) {
  Serial.println("We are connected\n");
  String line = client.readStringUntil('\n');
  if (line == "\r") {
  Serial.println("headers received");
  break;
  }
  }
  String line = client.readStringUntil('\n');
  // if (line.startsWith("{\"state\":\"success\"")) {
  // Serial.println("esp8266/Arduino CI successfull!");
  // } else {
  // Serial.println("esp8266/Arduino CI has failed");
  // }
  
  
  
  if (!client.connect(host, httpsPort)) {
  Serial.println("connection failed");
  return;
  }
  delay(2000);
}


// -- END OF FILE --
