#include <ESP8266WiFi.h>
#include "Devices.h"
#include "Libraries/LinkedList/LinkedList.h"
#include "Libraries/ESP/ESP.h"

const char* WIFI_SSID = "Your WIFI-Name";
const char* WIFI_PASSWORD = "Your WIFI-Password";
const char* HOSTNAME = "TestHost123";
#define SERVER_PORT 25567


WiFiServer server(SERVER_PORT);
WiFiClient client;

LinkedList<Device> devicelist = LinkedList<Device>();
Device devicelist2[99];


void setup() {
  initHardware();

  //Polymorphism Testing
  /*delay(3000);
  Serial.println("Device creating...");
  Relay d(3, 7);
  Relay r(2, 6);
  Device d1(2);
  Serial.println("Device created!");

  Serial.println("Adding to List...");
  devicelist.add(d);
  devicelist.add(d1);
  devicelist.add(r);
  Serial.println("Added to List!");

  Serial.println("Adding to Array...");
  devicelist2[0] = d;
  devicelist2[1] = r;
  devicelist2[2] = d1;
  Serial.println("Added to Array!");

  Serial.print("Success!");*/

  //setupWiFi();
  //server.begin();
}

void loop() {
  //TCPServer();
}

#pragma region Server Handeling

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

#pragma endregion


#pragma region Events

void Incomingmsg(String curr_msg) {
  int PacketType = curr_msg[0]-'0'; // curr_msg[0] => 
  switch (PacketType) {
    case 0:
      {
        CreateDevice(curr_msg);
      }
      break;
    case 1:
      {
      }
      break;
    case 2:
      {
      }
      break;
      case 3:
      {
        ESP.restart();
      }
    default:
      {
        Serial.println("Invalid Packet Type received: " + Message);
        //Error
      }
      break;
  }

}

#pragma endregion

#pragma region Functions

void sendString(String data){ //not Tested yet
  for (int i = 0; i < data.length(); i++) {
    client.write(data[i]);
  }
}

void CreateDevice(String curr_msg) {
  int curr_dev_id = curr_msg.substring(1, 3).toInt();
  Serial.println("Adding Device " + String(curr_dev_id));
}

#pragma endregion



#pragma region Setup Voids

void setupWiFi() {
  WiFi.hostname(HOSTNAME);
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

#pragma endregion
