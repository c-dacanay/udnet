
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
const int LEDPin = 2;
bool friend1_update = false;
bool friend2_update = false;
int getStatusCode1 = 0;
int getStatusCode2 = 0;
int last_friend1_ID = 0;
int last_friend2_ID = 0;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

void setup() {
  pinMode(LEDPin, OUTPUT);
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

  //our mini table right now has 4 objects per 4 mac addresses which correlates to array size
  //each object has 3 items: mac_address, data_point, recorded_at
  //TODO: remove dummy mac_address
  const size_t capacity = JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(3) + 30;
  DynamicJsonDocument doc(capacity);

  //our json object comes from our get request of the mini table
  String json = getLatest();
  Serial.println(json);

  deserializeJson(doc, json);

  //const char* sensor = doc["sensor"]; // "gps"
  //long time = doc["time"]; // 1351824120

  mac1_latestData = doc[0]["data"];
  mac2_latestData = doc[1]["data"];
  mac3_latestData = doc[2]["data"];
  mac4_latestData = doc[3]["data"];
  Serial.println(mac1_latestData);
}

void loop() {

  //check for updates from other devices
  getFriend1PotUpdate();
  getFriend2PotUpdate();
  
  //if there's an update, next function returns true
  friendUpdate();
  delay(3000);
  
  //also check if we need to post an update of our own pot
  postPotUpdate();
  delay(3000);

  //if there is an update from friends, turn an LED ON for 10 seconds
  if (friend1_update || friend2_update) {
    digitalWrite(LEDPin, HIGH);
    delay(10000);
    digitalWrite(LEDPin, LOW);
    delay(10);
  }
  else {
    digitalWrite(LEDPin, LOW);
    friend1_update = false;
  }

  Serial.println("Wait five seconds");
  
  //repeat every 5 seconds
  delay(5000);
}

String getLatest(){
  Serial.println("making GET request for LATEST");
  client.beginRequest();

  String get_data = "/latest/";

  client.get(get_data);
  client.endRequest();
  String response = client.responseBody();
  return response; 
}

void postPotUpdate() {

  int potentiometer = analogRead(A0);                  // read the input pin
  currentPot = map(potentiometer, 0, 1023, 0, 10); // remap the pot value to fit in 1 byte

  if (abs(currentPot - oldPot) >= 1) {
    Serial.println("We've changed!");

    Serial.println("making POST request");
    String contentType = "application/x-www-form-urlencoded";

  //PUT YOUR MAC ADDRESS HERE//
    String postData = "macAddress=A4:CF:12:23:4E:7C&sessionKey=12345678&data=" + String(currentPot);
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

  Serial.print("Current: ");
  Serial.println(currentPot);
  Serial.print("Old: ");
  Serial.println(oldPot);

}

int getFriend1PotUpdate() {

  Serial.println("making GET request");
  client.beginRequest();


  //REPLACE MAC ADDRESS OF FRIEND1 HERE
  //Emily MAC: A4:CF:12:22:1A:1C
  //Ben MAC: 4C:11:AE:C9:6A:E
  //Christina MAC: A4:CF:12:23:4E:7C
  
  //String get_data = "/data/" + String(last_friend1_ID+1) + "/?macAddress=AA:BB:CC:DD:EE:FF&sessionKey=12345678";
  String get_data = "/data/" + String(last_friend1_ID) + "/?macAddress=4C:11:AE:C9:6A:E8&sessionKey=12345678";
  client.get(get_data);

  //generic get all request
  //client.get("/data?macAddress=AA%3ABB%3ACC%3ADD%3AEE%3AFF&sessionKey=12345678");
  
  client.endRequest();

  //gather the status response from the server, also store the body into a string and get its length
  getStatusCode1 = client.responseStatusCode();
  String getResponse = client.responseBody();

  //print them out for debugging
  Serial.print("Status code: ");
  Serial.println(getStatusCode1);
  Serial.print("Response: ");
  Serial.println(getResponse);
  return getStatusCode1;
  
}

int getFriend2PotUpdate() {
  
  Serial.println("making GET request");
  client.beginRequest();

  //REPLACE MAC ADDRESS OF FRIEND2 HERE
  String get_data = "/data/" + String(last_friend2_ID) + "/?macAddress=A4:CF:12:22:1A:1C&sessionKey=12345678";
  client.get(get_data);
  
  client.endRequest();
  getStatusCode2 = client.responseStatusCode();
  String getResponse = client.responseBody();
  
  Serial.print("Status code: ");
  Serial.println(getStatusCode2);
  Serial.print("Response: ");
  Serial.println(getResponse);
  return getStatusCode2;


}


void friendUpdate() {


   if (getStatusCode1 == 200 || getStatusCode1 == 201){
    last_friend1_ID++;
    friend1_update = true;
  }
  else{
    friend1_update = false;
  }

 if (getStatusCode2 == 200 || getStatusCode2 == 201){
    last_friend2_ID++;
    friend2_update = true;
  }
  else{
    friend2_update = false;
  }

}
