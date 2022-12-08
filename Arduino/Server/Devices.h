#include "WString.h"
#ifndef _DEVICES_H
#define _DEVICES_H
#include "Arduino.h"
#include <RCSwitch.h>

class Device {
public:
  int Device_ID;
  Device(int _Device_ID) {
    Device_ID = _Device_ID;
  }
  Device() { Device_ID = -1; }

  int State;

  virtual void On() {}
  virtual void Off() {}
  virtual String toString() {
    return "Device: " + String(this->Device_ID);
  }
};

class Relay : public Device {
  int DataPin;
public:
  Relay(int _Device_ID, int _DataPin)
    : Device(_Device_ID) {
    pinMode(_DataPin, OUTPUT);
    DataPin = _DataPin;
  }

  virtual void On() {
    digitalWrite(DataPin, HIGH);
    Serial.println("Relay " + this->Device_ID + " turned on.")
    State = 1;
  }
  virtual void Off() {
    digitalWrite(DataPin, LOW);
    Serial.println("Relay " + this->Device_ID + " turned off.")
    State = 0;
  }

  virtual String toString() {
    return "Relay: " + String(this->Device_ID);
  }
};


class Transmitter_433 : public Device {
    int DataPin;
    RCSwitch sender;
  public:
    Transmitter_433(int _Device_ID, int _DataPin)
      : Device(_Device_ID) {
      DataPin = _DataPin;
      //Initialize Transmitter
      sender = RCSwitch();
      sender.enableTransmit(DataPin);  
      sender.setProtocol(5);
      sender.setPulseLength(505);
    }

  virtual void On(String Code) {
    String ACode_ = Code.substring(0,5);
    String BCode_ = Code.substring(5,10);
    const char* ACode = ACode_.c_str();
    const char* BCode = BCode_.c_str();
    sender.switchOn(ACode,BCode);
    State = 1;
    Serial.println("Transmitter turned Device " + this->Device_ID +  " ON: " + String(ACode) + ", " + String(BCode));
  }
  virtual void Off(String Code) {
    String ACode_ = Code.substring(0,5);
    String BCode_ = Code.substring(5,10);
    const char* ACode = ACode_.c_str();
    const char* BCode = BCode_.c_str();
    sender.switchOff(ACode,BCode);
    State = 0;
    Serial.println("Transmitter turned Device " + this->Device_ID +  " OFF: " + String(ACode) + ", " + String(BCode));
  }

};

class LED_Stripe : public Device {


};

#endif
