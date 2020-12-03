#include <ArduinoJson.h>

int mac1_latestData = 0;
int mac2_latestData = 0;
int mac3_latestData = 0;
int mac4_latestData = 0;

void setup() {

  Serial.begin(9600);
  
  //our mini table right now has 4 objects per 4 mac addresses which correlates to array size
  //each object has 3 items: mac_address, data_point, recorded_at
  //TODO: remove dummy mac_address
  
  const size_t capacity = JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(3) + 30;
  DynamicJsonDocument doc(capacity);

  //our json object comes from our get request of the mini table
  const char* json = getLatest();

  deserializeJson(doc, json);

  //const char* sensor = doc["sensor"]; // "gps"
  //long time = doc["time"]; // 1351824120

  mac1_latestData = doc[0]["data"];
  mac2_latestData = doc[1]["data"];
  mac3_latestData = doc[2]["data"];
  mac4_latestData = doc[3]["data"];
  Serial.println(mac1_latestData);

}

void getLatest(){
  Serial.println("making GET request for LATEST");
  client.beginRequest();

  String get_data = "/latest/" + String(last_ID) + "/?macAddress=AA:BB:CC:DD:EE:FF&sessionKey=12345678";

  client.get(get_data);
  client.endRequest();
}

void loop() {
  // put your main code here, to run repeatedly:

}
