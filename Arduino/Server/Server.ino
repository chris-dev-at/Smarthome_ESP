#include <ESP8266WiFi.h>
#include "Devices.h"
#include "Libraries/LinkedList/LinkedList.h"

const char* WIFI_SSID = "WIFI_SSID";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";
#define SERVER_PORT 25567


WiFiServer server(SERVER_PORT);
WiFiClient client;

Device devicelist[99];

void setup() {
  initHardware();
  //setupWiFi();
  //server.begin();
}

void loop() {
  devicelist[1] = Device(1);
  devicelist[2] = Relay(2, 3);

  Serial.println(devicelist[1].toString());
  Serial.println(devicelist[2].toString());
  Serial.println(devicelist[3].toString());
  delay(1000);
  Serial.println("------");
  //TCPServer();
}



String Message = "";
char lastchar = ' ';

void TCPServer() {
  while (lastchar != '.') {
    if (!client.connected()) {
      // try to connect to a new client
      client = server.available();
    } else {
      //Serial.println("Client connected");
      // read data from the connected client

      if (client.available() > 0) {
        //Serial.println(client.read());
        lastchar = client.read();

        if (lastchar != 0) {
          //Serial.print(lastchar);
          Message = Message + lastchar;
        }
      }
    }
  }
  if (lastchar == '.') {
    Serial.println(Message);
    lastchar = 0;
    Incomingmsg(Message);
    Message = "";
  }
}


void Incomingmsg(String curr_msg) {
  int PacketType = curr_msg[0];
  switch (PacketType) {
    case 0:
      {
        CreateDevice(curr_msg);
      }
    case 1:
      {
      }
    case 2:
      {
      }
    default:
      {
        Serial.println("Invalid Packet Type received: " + Message);
        //Error
      }
  }

}

void CreateDevice(String curr_msg) {
  int curr_dev_id = curr_msg.substring(1, 3).toInt();
  Serial.println("Current Device ID: " + curr_dev_id);
}


void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("ESP32_Buzzer");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to Wifi... Try:");
    Serial.println(".");
    delay(100);
  }

  Serial.print("ESP32 #2: TCP Server IP address: ");
  Serial.println(WiFi.localIP());
}

void initHardware() {
  Serial.begin(115200);
}
