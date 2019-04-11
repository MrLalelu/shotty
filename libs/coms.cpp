#include "coms.h"

int state = -1;

int read_is_open(SoftwareSerial * ser) {
    char get;
    bool did_read = false;
    while (ser->available() > 0) {
        get = ser->read();
        did_read = true;
        Serial.write(get);
    }
    if (did_read) {
        if (get == '0') {
            state = -1;
        }
        if (get == '1') {
            state = 0;
        }
        if (get == '2') {
            state = 1;
        }
        if (get == '3') {
            state = 2;
        }
        if (get == '4') {
            state = 3;
        }
        if (get == '5') {
            state = 4;
        }
        if (get == '6') {
            state = 5;
        }
    }
    return state;
}

void send_is_open(SoftwareSerial * ser, int state) {
    if (state == 0) {
        ser->write('1');
    }
    if (state == 1) {
        ser->write('2');
    }
    if (state == 2) {
        ser->write('3');
    }
    if (state == 3) {
        ser->write('4');
    }
    if (state == 4) {
        ser->write('5');
    }
    if (state == 5) {
        ser->write('6');
    }
    else {
        ser->write('0');
    }

    ser->write(state);
}