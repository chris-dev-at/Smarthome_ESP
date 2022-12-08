#include "WString.h"
#ifndef _DEVICES_H
#define _DEVICES_H
#include "Arduino.h"

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

    // Testing
    //digitalWrite(LED_BUILTIN, HIGH); 

    digitalWrite(DataPin, HIGH);
    State = 1;
    Serial.println("RELAY TURN ON");
  }
  virtual void Off() {
    //Testing
    //digitalWrite(LED_BUILTIN, LOW); 

    digitalWrite(DataPin, LOW);
    State = 0;
    Serial.println("RELAY TURN OFF");
  }

  virtual String toString() {
    return "Relay: " + String(this->Device_ID);
  }
};


class Transmitter_433 : public Device {
};


#endif
