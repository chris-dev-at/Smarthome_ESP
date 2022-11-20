#include "Arduino.h"
#include "Devices.h"

Device::Device(int _Device_ID) {
    Device_ID = _Device_ID;
    Serial.println("Created Device: " + _Device_ID);
  }
Device::Device(){}