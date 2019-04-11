#include "coms.h"

char state = 'n';

bool read_is_open() {
    while Serial.available() > 0 {
        state = Serial.read()
    }
    if state == 'y' {
        return true;
    }
    return false;
}

void send_is_open(bool is_open) {
    if is_open {
        Serial.write('y');
        return;
    }
    Serial.write('n');
}