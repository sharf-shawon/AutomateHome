/*
AutomateHomeRev2
By Mohammed Sharfuddin Shawon
(written for Wemos D1 R2)

*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
ESP8266WiFiMulti wifi;

String    ssid        = "2WIRE739";
String    password    = "5326810248";
String    SERVER      = "http://automatehome.cf/";
String    API         = "rAicNAGYheXwtGGP";
String    CID         = "wemos1";
const int fetchSpeed  = 2; //Hz
const int relayPins[] = {D4,  D5,  D6,  D7};
const int relayCount  = 4;

String LINK = SERVER + "?MODE=2&API=" + API + "&CID=" + CID;
bool relayStatsOld[relayCount];
bool relayStatsNew[relayCount];

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  Serial.println();
  Serial.println("*******************************************************************************");
  Serial.print("Relay Count: ");
  Serial.println(relayCount);
  Serial.println("Relay No => Connected Pin");
  Serial.println("========================="); 
  for (int i = 0; i < relayCount; i++)
  {
    Serial.print("Relay #");
    Serial.print(i + 1);
    Serial.print(" => ");
    Serial.println(relayPins[i]);
  }
  Serial.println("========================="); 
  Serial.println();
  Serial.println("SERVER: " + SERVER);
  Serial.println("API: " + API);
  Serial.println("CID: " + CID);
  Serial.print("Link: ");
  Serial.println(LINK);
  Serial.print("Connecting to " + ssid);
  wifi.addAP("2WIRE739", "5326810248");
  Serial.println(": Connected");
  Serial.println("*******************************************************************************");
  Serial.println();

  for (int i = 0; i < relayCount; i++)
    pinMode(relayPins[i], OUTPUT);
  for (int i = 0; i < relayCount; i++)
    relayStatsOld[i] = true;
  for (int i = 0; i < relayCount; i++)
    relayStatsNew[i] = true;
}

void loop() {
  // wait for WiFi connection
  if ((wifi.run() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(LINK); //HTTP
    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK){
        String payload = http.getString();
        String data = decodeData(payload);
        if (data.toInt() != -1)
          runRelays(data);
      }
    } else {
      Serial.printf("[HTTP] error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  delay(1000 / fetchSpeed);
}

String decodeData(String $data)
{
  switch ($data.charAt(0))
  {
    case '!':
      Serial.println(errorHandler($data.substring(1, 4).toInt()));
      return "-1";
      break;
    case '#':
      return $data.substring(1);
      break;
    default:
      Serial.println("UNKNOWN SERVER RESPONSE:" + $data);
      return "-1";
      break;
  }
}

String errorHandler(int error)
{
  switch (error)
  {
    case 101:
      return "Server Error: NO DATA SENT";
      break;
    case 102:
      return "Server Error: NO MODE";
      break;
    case 103:
      return "Server Error: INVALID MODE";
      break;
    case 110:
      return "Server Error: NO API";
      break;
    case 111:
      return "Server Error: INVALID API";
      break;
    case 112:
      return "Server Error: NO CID";
      break;
    case 113:
      return "Server Error: INVALID CID";
      break;
    default:
      return "Server Unknown Error: " + error;
      break;
  }
}

String runRelays(String rawData)
{
  for (int i = 0; i < relayCount; i++)
  {
    relayStatsNew[i] = rawData.substring(i, i + 1).toInt();
    if (relayStatsOld[i] != relayStatsNew[i])
    {
      digitalWrite(relayPins[i], (relayStatsNew[i] == 1) ? LOW : HIGH);
      Serial.print("Relay #");
      Serial.print(i + 1);
      Serial.println((relayStatsNew[i] == 1) ? ": ON" : ": OFF");
      Serial.flush();
      relayStatsOld[i] = relayStatsNew[i];
    }
  }
}
