#include "coms.h"

char state = 'n';

bool read_is_open(SoftwareSerial * ser) {
    while ser->available() > 0 {
        state = ser->read()
    }
    if state == 'y' {
        return true;
    }
    return false;
}

void send_is_open(SoftwareSerial * ser, bool is_open) {
    if is_open {
        ser->write('y');
        return;
    }
    ser->write('n');
}