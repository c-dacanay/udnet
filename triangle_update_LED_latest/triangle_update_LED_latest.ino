#include <ArduinoHttpClient.h>
//#include <WiFi101.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include <ArduinoJson.h>

int mac1_latestData = 0;
int mac2_latestData = 0;
int mac3_latestData = 0;
int mac4_latestData = 0;

char serverAddress[] = "206.189.189.187";
int port = 8080;
int oldPot = 0;
int currentPot = 0;
const int LEDPin1 = 2;
const int LEDPin2 = 3;


WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

void setup() {
  pinMode(LEDPin1, OUTPUT);
  pinMode(LEDPin2, OUTPUT);
  Serial.begin(9600);
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);                   // print the network name (SSID);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(SECRET_SSID, SECRET_PASS);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

}

void loop() {

  //download latest friend data
  friendDataCheck();


  delay(3000);

  //also check if we need to post an update of our own pot
  postPotUpdate();


  Serial.println("Wait 1 minute");

  //repeat every 60 seconds
  delay(60000);
}

String getLatest() {

  Serial.println("making GET request for LATEST");
  client.beginRequest();

  String get_data = "/latest/?macAddress=4C:11:AE:C9:6A:E8&sessionKey=12345678";

  client.get(get_data);
  client.endRequest();
  String response = client.responseBody();
  Serial.println(response);
  return response;
}

void friendDataCheck() {
  //const size_t capacity = (JSON_ARRAY_SIZE(4) * JSON_OBJECT_SIZE(3)) + 30;
  const size_t capacity = (JSON_ARRAY_SIZE(5) * JSON_OBJECT_SIZE(3)) + 30;
  DynamicJsonDocument doc(capacity);

  //our json object comes from our get request of the mini table, print it to SM for debugging
  String json = getLatest();
  Serial.println(json);

  deserializeJson(doc, json);

  //store each current value into their tracking variables
  mac1_latestData = doc[0]["data_point"];
  mac2_latestData = doc[1]["data_point"];
  mac3_latestData = doc[2]["data_point"];

  //print them out for debugging
  Serial.println(mac1_latestData);
  Serial.println(mac2_latestData);
  Serial.println(mac3_latestData);


  //Emily MAC: A4:CF:12:22:1A:1C
  //Ben MAC: 4C:11:AE:C9:6A:E8
  //Christina MAC: 3C:71:BF:87:C1:D8


  //comment out mine and uncomment the code under your name

  //Ben LED code:
  //map friend 1 and 2 to LED brightness, friend1 = emily, friend2 = christina
  int brightness1 = map(mac1_latestData, 0, 10, 0, 255);
  analogWrite(LEDPin1, brightness1);
  delay(30);

  int brightness2 = map(mac3_latestData, 0, 10, 0, 255);
  analogWrite(LEDPin2, brightness2);
  delay(30);
  
  //Emily LED Code:
  //  int brightness1 = map(mac1_latestData, 0, 10, 0, 255);
  //  analogWrite(LEDPin1, brightness1);
  //  delay(30);
  //
  //  int brightness2 = map(mac2_latestData, 0, 10, 0, 255);
  //  analogWrite(LEDPin2, brightness2);
  //  delay(30);

  //Christina LED Code:
  //  int brightness1 = map(mac2_latestData, 0, 10, 0, 255);
  //  analogWrite(LEDPin1, brightness1);
  //  delay(30);
  //
  //  int brightness2 = map(mac3_latestData, 0, 10, 0, 255);
  //  analogWrite(LEDPin2, brightness2);
  //  delay(30);

}

void postPotUpdate() {

  int potentiometer = analogRead(A0);                  // read the input pin
  currentPot = map(potentiometer, 0, 1023, 0, 10); // remap the pot value to fit in 1 byte

  if (abs(currentPot - oldPot) >= 1) {
    Serial.println("We've changed!");

    Serial.println("making POST request");
    String contentType = "application/x-www-form-urlencoded";
    

    //PUT YOUR MAC ADDRESS IN LINE 149 INSTEAD OF MINE//
    String postData = "macAddress=4C:11:AE:C9:6A:E8&sessionKey=12345678&data=" + String(currentPot);
    client.beginRequest();
    //client.post("/post-test", contentType, postData);
    client.post("/data", contentType, postData);
    client.endRequest();

    int postStatusCode = client.responseStatusCode();
    String response = client.responseBody();

    Serial.print("Status code: ");
    Serial.println(postStatusCode);
    Serial.print("Response: ");
    Serial.println(response);

    oldPot = currentPot;
  }

  Serial.print("Current Mood Out of 10: ");
  Serial.println(currentPot);
  Serial.print("Previous Mood out of 10: ");
  Serial.println(oldPot);

}
