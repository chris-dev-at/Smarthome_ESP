#include <ESP8266WiFi.h>
#include "Devices.h"
#include "Libraries/LinkedList/LinkedList.h"
#include "Libraries/ESP/ESP.h"

const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";
const char* HOSTNAME = "TestHost123";
#define SERVER_PORT 25567


WiFiServer server(SERVER_PORT);
WiFiClient client;

//LinkedList<Device> devicelist = LinkedList<Device>();
Device* devicelist[41];


void setup() {
  initHardware();
  setupWiFi();
  server.begin();
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
}

void loop() {
  TCPServer();
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
        Serial.println("Create Device");
        CreateDevice(curr_msg);
      }
      break;
    case 1:
      {
        Serial.println("Remove Device");
        RemoveDevice(curr_msg);
      }
      break;
    case 2:
      {
        Serial.println("Action");
        Action(curr_msg);
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
  int dev_id = curr_msg.substring(1, 3).toInt();
  int Device_Type = curr_msg.substring(3, 5).toInt();
  int DataPin = curr_msg.substring(5,7).toInt();
  if(dev_id > 40) return;
  Serial.println("Dev_ID " + String(dev_id));
  Serial.println("Pin " + String(DataPin));
  Serial.println("Device_Type " + String(Device_Type));

  switch(Device_Type)
  {
     case 0:
        break;
     case 1:
        Relay* tmp = new Relay(dev_id, DataPin);
        devicelist[dev_id] = tmp;
        break;
  }
}

void RemoveDevice(String curr_msg)
{
    int dev_id = curr_msg.substring(1, 3).toInt();
    if(dev_id > 40) return;
    if(devicelist[dev_id]->Device_ID == -1)
    {
      Serial.println("Device does not exist!");
      return;
    }
    Serial.println("Device " + String(devicelist[dev_id]->Device_ID) + " removed");            
    devicelist[dev_id] = new Device();
    if(devicelist[dev_id]->Device_ID == -1) Serial.println("Success at removing");
}

void Action(String curr_msg)
{
    int dev_id = curr_msg.substring(1, 3).toInt();
    if(dev_id > 40) return;
    int dev_type = curr_msg.substring(3, 5).toInt();
    String Action = curr_msg.substring(5, 14);
    Action.replace("0", "");
    String Actioninfo = curr_msg.substring(14);
    switch(dev_type){
      case 1:
      {
        Relay* tmp = static_cast<Relay*>(devicelist[dev_id]);
        if(Action == "toggle")
        {
          Serial.println("toggle");
           if(Actioninfo == "1")
           {
              tmp->On();
           }
           else
           {
              tmp->Off();
           }
        }
      }
      break;
      default:
        break;
    }

}



#pragma endregion



#pragma region Setup Voids

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME);
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

  pinMode(LED_BUILTIN, OUTPUT); // LED als Output definieren
  digitalWrite(LED_BUILTIN, LOW); 


}

#pragma endregion
