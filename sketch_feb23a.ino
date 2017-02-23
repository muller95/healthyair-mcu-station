#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <strings.h>
#include <stdlib.h>

#define MAX_POST_DATA_SIZE 1024

#include "SHT1X.h"
char *ssid = "AndroidAP";
char *wl_passwd = "Warcraft123Ab";

String email = "muller95@yandex.ru";
String web_passwd = "Warcraft123Ab";
String stname = "station";

int wl_status;



WiFiClient client;
const char* host="http://healthyair.ru/data_listener.php";


int load_led = 12, wl_connected_led = 13;

void postData(float t, float rh)
{
  //char postData[MAX_POST_DATA_SIZE];
  String postData = "email=" + email + "&passwd=" + web_passwd + "&stname=" 
    + stname + "&t=" + String(t) + "&rh=" + String(rh) + "&co2=1000&datetime=2017-02-09 21:47:30";
  
  Serial.println("Try post");
  HTTPClient http;
  http.begin("http://healthyair.ru/data_listener.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  //bzero(postData, MAX_POST_DATA_SIZE);
  Serial.println(t);
//  sprintf(postData, "email=%s&passwd=%s&stname=%s&t=%lf&rh=50&co2=1000&datetime=2017-02-09 21:47:30", 
//    email, web_passwd, stname, t);
  Serial.println(postData);
  int code = http.POST(postData);
  
  
  Serial.println(code);
  if (code >= 200 && code < 300) {
    String resp = http.getString();
    Serial.println(resp);
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

  digitalWrite(wl_connected_led, LOW);
  wl_status = WiFi.status();
  if (wl_status == WL_CONNECTED) {
    digitalWrite(wl_connected_led, HIGH);
    SHT1x sht15(4, 5);//Data, SCK
    float t = sht15.readTemperatureC();
    float rh = sht15.readHumidity();  
    Serial.print(" Temp = ");
    Serial.print(t);
    Serial.println("C");
    Serial.print(" Humidity = ");
    Serial.print(rh); 
    Serial.println("%");
    postData(t, rh);
  }
}

void setup() 
{
  pinMode(load_led, OUTPUT);
  digitalWrite(load_led, HIGH);
  pinMode(wl_connected_led, OUTPUT);
  Serial.begin(74880);
 // Serial.setDebugOutput(true);
  Serial.println("load");

  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  WiFi.begin("Keenetic-5802", "vNX2ambo");
  //WiFi.mode(WIFI_STA);
  
  delay(100);

  //while (WiFi.status() != WL_CONNECTED);

}
