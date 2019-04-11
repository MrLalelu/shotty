#ifndef COMS_H
#include "Arduino.h"
#include <SoftwareSerial.h>
bool read_is_open(SoftwareSerial * ser);
void send_is_open(SoftwareSerial * ser, bool is_open);
#endif