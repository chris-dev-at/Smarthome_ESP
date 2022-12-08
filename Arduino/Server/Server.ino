#include <ESP8266WiFi.h>
#include "Devices.h"
#include "Libraries/LinkedList/LinkedList.h"
#include "Libraries/ESP/ESP.h"

const char* WIFI_SSID = "YOUR-WIFI"; //enter your WiFi_SSID here
const char* WIFI_PASSWORD = "YOUR-WIFI-PASSWORD"; //enter your WiFi_PASSWORD here
const char* HOSTNAME = "TestHost123"; //TODO: try automatically finding devices from c# server over Hostname
#define SERVER_PORT 25567


WiFiServer server(SERVER_PORT);
WiFiClient client;

Device* devicelist[41]; //List of all Devices | [index] = device_id


void setup() {
  initHardware(); //setup Debuggin over Serial (115200 baud)
  setupWiFi();
  server.begin();
}

void loop() {
  TCPServer(); //Constantly looping for a new Packet
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
      //Serial.println("Client connected: ");
      // read data from the connected client

      if (client.available() > 0) {
        lastchar = client.read();

        if (lastchar != 0) {
          Message = Message + lastchar;
        }
      }
    }
  }
  if (lastchar == '.') { //if '.' is sent the server knows a packet is finished
    Serial.println("Message received: " + Message);
    lastchar = 0;
    Incomingmsg(Message); //further handeling of received Message
    Message = "";
  }
}

#pragma endregion


#pragma region Events

void Incomingmsg(String curr_msg) {
  curr_msg.replace(".","");
  int PacketType = curr_msg[0]-'0'; // curr_msg[0] => 
  switch (PacketType) {
    case 0: // PacketType = CreateDevice
      {
        Serial.println("Create Device");
        CreateDevice(curr_msg);
      }
      break;
    case 1: // PacketType = RemoveDevice
      {
        Serial.println("Remove Device");
        RemoveDevice(curr_msg);
      }
      break;
    case 2: // PacketType = ActionPacket
      {
        Serial.println("Action");
        Action(curr_msg);
      }
      break;
      case 3: // Just during developement
      {
        ESP.restart();
      }
    default:
      {
        Serial.println("Invalid Packet Type received: " + Message);
      }
      break;
  }

}

#pragma endregion


#pragma region Functions

//sending Data to Client (C# Server)
void sendString(String data){ //not Tested yet
  for (int i = 0; i < data.length(); i++) {
    client.write(data[i]);
  }
}

//Creating a Device
void CreateDevice(String curr_msg) {
  int dev_id = curr_msg.substring(1, 3).toInt();
  int Device_Type = curr_msg.substring(3, 5).toInt();
  int DataPin = curr_msg.substring(5,7).toInt();
  
  if(dev_id > 40) return; //replace Device with Empty Device (remove)
  Serial.println("Dev_ID " + String(dev_id));
  Serial.println("Pin " + String(DataPin));
  Serial.println("Device_Type " + String(Device_Type));

  switch(Device_Type)
  {
     case 0:
        break;
     case 1: // DeviceType = Relay
     {
        Relay* tmp = new Relay(dev_id, DataPin);
        devicelist[dev_id] = tmp;
     }
      break;
     case 2: // DeviceType = Transmitter_433
     {
        int AdditionalInfo = curr_msg.substring(7).toInt();
        Transmitter_433* tmp = new Transmitter_433(dev_id, DataPin);
        devicelist[dev_id] = tmp;
     }
     case 3: // DeviceType = LED Stripe
     {
        LED_Stripe* tmp = new LED_Stripe(dev_id, DataPin, 20);
        devicelist[dev_id] = tmp;
     }
      break;  
        

  }
}

//Removing a Device
void RemoveDevice(String curr_msg)
{
    int dev_id = curr_msg.substring(1, 3).toInt();
    if(dev_id > 40) return; // make sure no invalid indexes are referenced
    if(devicelist[dev_id]->Device_ID == -1)
    {
      Serial.println("Device does not exist!");
      return;
    }
    Serial.println("Device " + String(dev_id) + " removed");            
    devicelist[dev_id] = new Device(); //replace Device with Empty Device (remove)
}

//Executes Actions on Devices
void Action(String curr_msg)
{
    int dev_id = curr_msg.substring(1, 3).toInt();
    if(dev_id > 40) return; //replace Device with Empty Device (remove)
    int dev_type = curr_msg.substring(3, 5).toInt();
    String Action = curr_msg.substring(5, 14);
    Action.replace("0", "");
    String ActionInfo = curr_msg.substring(14);
    switch(dev_type){
      case 1: //DeviceType = Relay
      {
        Relay* tmp = static_cast<Relay*>(devicelist[dev_id]);
        if(Action == "toggle")
        {
          Serial.println("toggle Action:" + Action + " ActionInfo:"+ActionInfo);
           if(ActionInfo == "1")
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

      case 2: //DeviceType = 433mhz Transmitter
      {
        Transmitter_433* tmp = static_cast<Transmitter_433*>(devicelist[dev_id]);
        if(Action == "toggle")
        {
          Serial.println("toggle Action2:" + Action + " ActionInfo:"+ActionInfo);
          String Code = ActionInfo.substring(1);
          if(ActionInfo.substring(0,1) == "1")
          {
            Serial.println("ON" + Code);
              tmp->On(Code);
          }
          else
          {
            Serial.println("OFF" + Code);
              tmp->Off(Code);
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

// setting up and Connecting to Wifi
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

//Setup Serial Debug
void initHardware() {
  Serial.begin(115200);
}

#pragma endregion
