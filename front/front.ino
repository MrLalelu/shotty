#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <stdlib.h>
#include <SoftwareSerial.h>

#include "libs/coms.h"
#include "libs/constants.h"
#define PFEFFI 0
#define JAEGERMEISTER 1
#define ROTER 2
#define SAURER 3

#define COLOR_PFEFFI 0, 255, 0
#define COLOR_JAEGERMEISTER 255, 110, 0
#define COLOR_ROTER 255, 0, 0
#define COLOR_SAURER 209, 255, 228
#define COLOR_BLAU 0, 0, 255

#define DRAW1 PFEFFI            // left to right
#define DRAW2 JAEGERMEISTER
#define DRAW3 ROTER
#define DRAW4 ROTER
#define DRAW5 PFEFFI
#define DRAW6 SAURER

#define NUM_DRAWS 6
#define NUM_READS 25

#define PIN_BUTTON_DRAW1 A2     // left to right
#define PIN_BUTTON_DRAW2 A1
#define PIN_BUTTON_DRAW3 A5
#define PIN_BUTTON_DRAW4 A3
#define PIN_BUTTON_DRAW5 A0
#define PIN_BUTTON_DRAW6 A4

#define PIN_INSIDE 6
#define NUM_LEDS_INSIDE 24

#define OPEN_THRESHOLD 0.5

// software serial
#define RX_PORT 10
#define TX_PORT 11

// wareSerial mySerial(10, 11); // RX, TX

// defining colors for the different liquors
uint8_t colors[NUM_COLORS][3] = {{COLOR_PFEFFI}, {COLOR_JAEGERMEISTER},
                                 {COLOR_ROTER}, {COLOR_SAURER}, {COLOR_BLAU}};

int draws[NUM_DRAWS] = {DRAW1, DRAW2, DRAW3, DRAW4, DRAW5, DRAW6};
int pins_draws[NUM_DRAWS] = {PIN_BUTTON_DRAW1, PIN_BUTTON_DRAW2, PIN_BUTTON_DRAW3,
                            PIN_BUTTON_DRAW4, PIN_BUTTON_DRAW5, PIN_BUTTON_DRAW6};

bool draw_open[NUM_DRAWS];
float draw_open_count[NUM_DRAWS];
bool any_draw_open;
int key_draw_open;

long last_time_send = 0;


Adafruit_NeoPixel pixels_inside =
    Adafruit_NeoPixel(NUM_LEDS_INSIDE, PIN_INSIDE, NEO_GRB + NEO_KHZ800);
SoftwareSerial ser(RX_PIN, TX_PIN);



void setup() {
    ser.begin(BAUD);
    // defining which liqur is in which draw
    for (int i = 0; i < NUM_DRAWS; i++) {
        pinMode(pins_draws[i], INPUT);
    }
    pixels_inside.begin();
    pixels_inside.show();

    for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
        pixels_inside.setPixelColor(i, 0, 0, 255);
    }

}




void loop() {
    // reading whether a drawer is open. Read NUM_READS to get rid of noise
    for (int i = 0; i < NUM_DRAWS; i++) {
        draw_open_count[i] = 0;
    }

    for (int i = 0; i < NUM_DRAWS; i++) {
        draw_open_count[i] = 0;
        for(int i2 = 0; i2 < NUM_READS; i2++) {
            draw_open_count[i] += digitalRead(pins_draws[i]);
        }
        draw_open[i] = draw_open_count[i] / NUM_READS > OPEN_THRESHOLD;
    }
    any_draw_open = false;

    for(int i = 0; i < NUM_DRAWS; i++) {
        if (draw_open[i]) {    //TODO remove not after testing!!!
            any_draw_open = true;
            key_draw_open = i;
            break;
        }
    }

    if (millis() - last_time_send > 10) {
        send_is_open(&ser, any_draw_open);
        last_time_send = millis();
    }

    // if a draw is open set the backlight color to the color of the chosen
    // liqour and iluminate the draw (last 6 leds on the inside strip)
    if (any_draw_open) {
        

        for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
            pixels_inside.setPixelColor(i, colors[key_draw_open][0],
                                           colors[key_draw_open][1],
                                           colors[key_draw_open][2]);
        }
    }
    else{
        for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
            pixels_inside.setPixelColor(i, COLOR_PFEFFI);
        }
    }


    pixels_inside.show();

}
