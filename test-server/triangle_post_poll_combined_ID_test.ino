
#include <ArduinoHttpClient.h>
//#include <WiFi101.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

// droplet address
char serverAddress[] = "206.189.189.187";  // server address
//keep this port the same
int port = 8080;
//pot initializer vars
int oldPot = 0;
int currentPot = 0;
//LED pin var
const int LEDPin = 2;
//boolean var for toggling when we need to turn LED on
bool friend_update = false;

//leftovers from previous code
//bool friend1_update = false;
//bool friend2_update = false;

//status code placeholder vars for each user (dummy device, ben, xtina, em)
int getStatusCode1 = 0;
int getStatusCode2 = 0;
int getStatusCode3 = 0;
int getStatusCode4 = 0;
//leftover from previous code vars
//int last_friend1_ID = 0;
//int last_friend2_ID = 0;

//aggregate transaction ID counter, incremented as posts are "gotted"
int last_ID = 1;

//connect to WiFi
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
}

void loop() {
  //check for updates from dummy device
  getDummyPotUpdate();

  //check for updates from my device
  getMyPotUpdate();

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
  if (friend_update) {
    digitalWrite(LEDPin, HIGH);
    delay(10000);
    digitalWrite(LEDPin, LOW);
    delay(10);
  }
  //otherwise we keep the LED off
  else {
    digitalWrite(LEDPin, LOW);
    //toggle the friend update bool back to off until next update
    friend_update = false;
  }

  Serial.println("Wait five seconds");

  //repeat every 5 seconds
  delay(5000);
}

void postPotUpdate() {

  int potentiometer = analogRead(A0);                  // read the input pin
  currentPot = map(potentiometer, 0, 1023, 0, 10); // remap the pot value to fit in 1 byte

  if (abs(currentPot - oldPot) >= 1) {   //maybe we increase this value so it only posts for big changes?
    Serial.println("We've changed!");

    Serial.println("making POST request");
    String contentType = "application/x-www-form-urlencoded";



    //PUT YOUR MAC ADDRESS HERE instead of mine//
    String postData = "macAddress=4C:11:AE:C9:6A:E8&sessionKey=12345678&data=" + String(currentPot);
    //start and end the client end post request
    client.beginRequest();
    client.post("/data", contentType, postData);
    client.endRequest();

    //store what we get back from server
    int postStatusCode = client.responseStatusCode();
    String response = client.responseBody();
    //print it to SM for our own sake
    Serial.print("Status code: ");
    Serial.println(postStatusCode);
    Serial.print("Response: ");
    Serial.println(response);

    //reset the pot variable to the new value for next loop iteration
    oldPot = currentPot;
  }

  Serial.print("Current: ");
  Serial.println(currentPot);
  Serial.print("Old: ");
  Serial.println(oldPot);

}

int getDummyPotUpdate() {

  Serial.println("making GET request");
  client.beginRequest();

  //string for get query for latest post by the dummy mac address
  String get_data = "/data/" + String(last_ID) + "/?macAddress=AA:BB:CC:DD:EE:FF&sessionKey=12345678";

  client.get(get_data);

  //generic get all request
  //client.get("/data?macAddress=AA%3ABB%3ACC%3ADD%3AEE%3AFF&sessionKey=12345678");

  client.endRequest();

  //gather the response from the server
  getStatusCode3 = client.responseStatusCode();
  String getResponse = client.responseBody();



  //print them out for debugging
  Serial.print("Status code: ");
  Serial.println(getStatusCode3);
  Serial.print("Response: ");
  Serial.println(getResponse);

  //return the status code
  return getStatusCode3;

}

int getFriend1PotUpdate() {

  Serial.println("making GET request");
  client.beginRequest();

  //REPLACE MAC ADDRESS OF FRIEND1 HERE
  //Emily MAC: A4:CF:12:22:1A:1C

  
  //string for get query for latest post by the Emily (friend1 in my case) mac address
  String get_data = "/data/" + String(last_ID) + "/?macAddress=A4:CF:12:22:1A:1C&sessionKey=12345678";
  client.get(get_data);
  client.endRequest();

  //gather the status response from the server, also store the body into a string and get its length
  getStatusCode1 = client.responseStatusCode();
  String getResponse = client.responseBody();

  //print them out for debugging
  Serial.print("Status code: ");
  Serial.println(getStatusCode1);
  Serial.print("Response: ");
  Serial.println(getResponse);

  //return the status code
  return getStatusCode1;

}

int getFriend2PotUpdate() {

  Serial.println("making GET request");
  client.beginRequest();

  //REPLACE MAC ADDRESS OF FRIEND2 HERE
  //Christina MAC: A4:CF:12:23:4E:7C

 
  //string for get query for latest post by the Christina (friend2 in my case) mac address
  String get_data = "/data/" + String(last_ID) + "/?macAddress=A4:CF:12:23:4E:7C&sessionKey=12345678";
  client.get(get_data);
  client.endRequest();
  
  //gather the status response from the server
  getStatusCode2 = client.responseStatusCode();
  String getResponse = client.responseBody();

  //print for debugging
  Serial.print("Status code: ");
  Serial.println(getStatusCode2);
  Serial.print("Response: ");
  Serial.println(getResponse);

  //return status code
  return getStatusCode2;


}

int getMyPotUpdate() {

  Serial.println("making GET request");
  client.beginRequest();

    //string for get query for latest post by my mac address
  String get_data = "/data/" + String(last_ID) + "/?macAddress=4C:11:AE:C9:6A:E8&sessionKey=12345678";
  client.get(get_data);
  client.endRequest();
  
  //gather the status response from the server, also store the body into a string and get its length
  getStatusCode4 = client.responseStatusCode();
  String getResponse = client.responseBody();


  Serial.print("Status code: ");
  Serial.println(getStatusCode2);
  Serial.print("Response: ");
  Serial.println(getResponse);

  //return status code

  return getStatusCode4;


}


void friendUpdate() {

//if any of our status codes from our get requests returns 200 (OK), there is an update, toggle boolean to true and increment our aggregate ID counter
  if (getStatusCode1 == 200  || getStatusCode2 == 200 || getStatusCode3 == 200 || getStatusCode4 == 200) {
    last_ID++;
    friend_update = true;
  }
//otherwise (404 code) no update posted
  else {
    friend_update = false;
  }

}
