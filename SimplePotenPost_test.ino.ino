#include <ArduinoHttpClient.h>
//#include <WiFi101.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

char serverAddress[] = "206.189.189.187";  // server address
int port = 8080;
int oldPot = 0;
int currentPot = 0;
WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

void setup() {
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

  int potentiometer = analogRead(A0);                  // read the input pin
  currentPot = map(potentiometer, 0, 1023, 0, 10); // remap the pot value to fit in 1 byte

  if (abs(currentPot - oldPot) >= 1) {

    Serial.println("making POST request");

    client.beginRequest();
    Serial.println("making POST request");
    String contentType = "application/x-www-form-urlencoded";

    String postData = "value=" + String(currentPot) + "user=christina";
    client.beginRequest();
    client.post("/post-test", contentType, postData);
    client.endRequest();

    int statusCode = client.responseStatusCode();
    String response = client.responseBody();

    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);


    oldPot = currentPot;
  }

  Serial.print("Current: ");
  Serial.println(currentPot);
  Serial.print("Old: ");
  Serial.println(oldPot);


  // read the status code and body of the response

  Serial.println("Wait five seconds");
  delay(5000);
}