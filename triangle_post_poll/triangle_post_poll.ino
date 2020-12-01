
#include <ArduinoHttpClient.h>
//#include <WiFi101.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"


char serverAddress[] = "206.189.189.187";  // server address
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
}

void loop() {
  //getMyPotUpdate();

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

void postPotUpdate() {

  int potentiometer = analogRead(A0);                  // read the input pin
  currentPot = map(potentiometer, 0, 1023, 0, 10); // remap the pot value to fit in 1 byte

  if (abs(currentPot - oldPot) >= 1) {
    Serial.println("We've changed!");

    Serial.println("making POST request");
    String contentType = "application/x-www-form-urlencoded";

    

  //PUT YOUR MAC ADDRESS HERE//

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

  //Sample get ID 2 from dummy device MAC
  //client.get("/data/2/?macAddress=AA:BB:CC:DD:EE:FF&sessionKey=12345678");

  //String get_data = "/data/" + String(last_friend1_ID+1) + "/?macAddress=AA:BB:CC:DD:EE:FF&sessionKey=12345678";
  String get_data = "/data/" + String(last_friend1_ID) + "/?macAddress=A4:CF:12:22:1A:1C&sessionKey=12345678";
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

  //return the status code
  return getStatusCode1;
  
}

int getFriend2PotUpdate() {
  
  Serial.println("making GET request");
  client.beginRequest();

  //REPLACE MAC ADDRESS OF FRIEND2 HERE
  //Christina MAC: A4:CF:12:23:4E:7C

  //generic get all data
  //client.get("/data?macAddress=4C:11:AE:C9:6A:E8&sessionKey=12345678");

  //String get_data = "/data/" + String(last_friend2_ID+1) + "/?macAddress=AA:BB:CC:DD:EE:FF&sessionKey=12345678";
  
  String get_data = "/data/" + String(last_friend2_ID) + "/?macAddress=A4:CF:12:23:4E:7C&sessionKey=12345678";
  client.get(get_data);


  client.endRequest();
//gather the status response from the server, also store the body into a string and get its length
  getStatusCode2 = client.responseStatusCode();
  String getResponse = client.responseBody();
  
  
  Serial.print("Status code: ");
  Serial.println(getStatusCode2);
  Serial.print("Response: ");
  Serial.println(getResponse);

  //return status code

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
