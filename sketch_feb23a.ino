#include <AsyncEventSource.h>
#include <AsyncJson.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <StringArray.h>
#include <WebAuthentication.h>
#include <WebHandlerImpl.h>
#include <WebResponseImpl.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define MAX_POST_DATA_SIZE 1024

#include "SHT1X.h"
char *ssid = "AndroidAP";
char *wl_passwd = "Warcraft123Ab";

String email = "muller95@yandex.ru";
String web_passwd = "Warcraft123Ab";
String stname = "station";
int wl_status;

AsyncWebServer server(80);

IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,1,9);
IPAddress subnet(255,255,255,0);


WiFiClient client;
const char* host="http://healthyair.ru/data_listener.php";
int load_led = 12, wl_connected_led = 12, data_sent_led = 13;

uint16_t k30_query()
{
  uint8_t len = 8;
  uint8_t query[] = {0xFE, 0x4, 0x0, 0x3, 0x0, 0x1, 0xD5, 0xC5};
  uint8_t resp[7];
//  Serial.println("try get ppm");
  while (Serial.available() == 0) {
    Serial.write(query, len);
//    Serial.println(k30_serial.available());
    delay(100);
  }
    
  while (Serial.available() < 7) {
//    Serial.println(k30_serial.available());
    delay(100);
  }
  
  for (int i = 0; i < 7; i++) 
    resp[i] = Serial.read();

  uint16_t ppm = (resp[3] << 8) | resp[4];

  return ppm;
}

void postData(float t, float rh, uint16_t co2)
{
  //char postData[MAX_POST_DATA_SIZE];
  String postData = "email=" + email + "&passwd=" + web_passwd + "&stname=" 
    + stname + "&t=" + String(t) + "&rh=" + String(rh) + "&co2=" + String(co2) + "&datetime=2017-02-09 21:47:30";
  
  Serial.println("Try post");
  HTTPClient http;
  http.begin("http://healthyair.ru/data_listener.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  Serial.println(t);
  Serial.println(postData);
  int code = http.POST(postData);
  
  
  Serial.println(code);
  if (code >= 200 && code < 300) {
//    String resp = http.getString();
//    Serial.println(resp);
    digitalWrite(data_sent_led, HIGH);
  }
  http.end();

  /*HTTPClient http;
http.begin("http://jsonplaceholder.typicode.com/posts");
http.addHeader("Content-Type", "application/x-www-form-urlencoded");
http.POST("title=foo&body=bar&userId=1");
http.writeToStream(&Serial);
http.end();*/
}

void loop() 
{
  /*if (WiFi.status() != WL_CONNECTED) {
    Serial.println("TRY CONNECT");
    
    WiFi.begin(ssid, passwd);
    delay(5000);
    Serial.print("Result=");
    Serial.println(WiFi.waitForConnectResult());
    
//    delay(5000);
    return;
  }*/

  digitalWrite(data_sent_led, LOW);
  digitalWrite(wl_connected_led, LOW);
  wl_status = WiFi.status();
  if (wl_status == WL_CONNECTED) {
    digitalWrite(wl_connected_led, HIGH);
    SHT1x sht15(4, 5);//Data, SCK
    float t = sht15.readTemperatureC();
    float rh = sht15.readHumidity();
    uint16_t ppm = k30_query();
    postData(t, rh, ppm);
  }
}

void setup() 
{
  //pinMode(load_led, OUTPUT);
  //digitalWrite(load_led, HIGH);
  pinMode(wl_connected_led, OUTPUT);
  pinMode(data_sent_led, OUTPUT);
  Serial.begin(9600);
  Serial.println("load");
  
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready config" : "Failed config!");
  Serial.println(WiFi.softAP("Healthy station") ? "Ready" : "Failed!");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "HERE");
  });

  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  WiFi.begin("Keenetic-5802", "vNX2ambo");
  //WiFi.mode(WIFI_AP_STA);
}
