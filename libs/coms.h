#ifndef COMS_H
#include "Arduino.h"
#include <SoftwareSerial.h>
int read_is_open(SoftwareSerial * ser);
void send_is_open(SoftwareSerial * ser, int is_open);
#endif