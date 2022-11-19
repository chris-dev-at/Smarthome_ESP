#ifndef _DEVICES_H
#define _DEVICES_H
#include "Arduino.h"

class Device {
  public: 
    int Device_ID;
    Device::Device(int _Device_ID)
    {
      Serial.println("Starting");
       Device_ID = _Device_ID;
    }

    int State;

    virtual void On();
    virtual void Off();
    
  };

class Relay : public Device{
  int DataPin;
  public:
    Relay(int _Device_ID, int _DataPin) : Device(_Device_ID)
    {
        pinMode(_DataPin, OUTPUT);
        DataPin = _DataPin;
    }

    virtual void On()
    {
        digitalWrite(DataPin, HIGH);
        State = 1;
    }
    virtual void Off()
    {
        digitalWrite(DataPin, LOW);
        State = 0;
    }
  
};


class Transmitter_433 : public Device{


};


#endif
