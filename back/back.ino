#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <stdlib.h>

#include "libs/constants.h"
#include "libs/coms.h"

#define PFEFFI 0
#define JAEGERMEISTER 1
#define ROTER 2
#define SAURER 3

#define DRAW1 SAURER            // left to right
#define DRAW2 PFEFFI
#define DRAW3 JAEGERMEISTER
#define DRAW4 PFEFFI
#define DRAW5 SAURER
#define DRAW6 PFEFFI

#define NUM_DRAWS 6
#define NUM_READS 25

#define PIN_BUTTON_DRAW1 A2     // left to right
#define PIN_BUTTON_DRAW2 A1
#define PIN_BUTTON_DRAW3 A5
#define PIN_BUTTON_DRAW4 A3
#define PIN_BUTTON_DRAW5 A0
#define PIN_BUTTON_DRAW6 A4

#define PIN_BACKLIGHT 7
#define NUM_LEDS_BACKLIGHT 149

#define NUM_EFFECTS 2
#define NUM_LEDS_RUNNING 10     // the amount of LEDs that should be 
                                // used for the running effect
#define DELAY_RUNNING_MIN 8   // the min time in millis between two effect
                                // steps in the running effect
#define DELAY_RUNNING_MAX 15   // the min time in millis between two effect
                                // steps in the running effect

// defining colors for the different liquors
uint8_t colors[NUM_COLORS][3] = {{COLOR_PFEFFI}, {COLOR_JAEGERMEISTER},
                                 {COLOR_ROTER}, {COLOR_SAURER}, {COLOR_BLAU}};

int draws[NUM_DRAWS] = {DRAW1, DRAW2, DRAW3, DRAW4, DRAW5, DRAW6};
int pins_draws[NUM_DRAWS] = {PIN_BUTTON_DRAW1, PIN_BUTTON_DRAW2, PIN_BUTTON_DRAW3,
                             PIN_BUTTON_DRAW4, PIN_BUTTON_DRAW5, PIN_BUTTON_DRAW6};

Adafruit_NeoPixel pixels_backlight =
    Adafruit_NeoPixel(NUM_LEDS_BACKLIGHT, PIN_BACKLIGHT, NEO_GRB + NEO_KHZ800);


void add_dimmed_pixel_loop(Adafruit_NeoPixel * pix, int len_stripe, int i_pix, uint8_t r, uint8_t g, uint8_t b) {
    float cur_intensity;
    for (int i = -5; i <= 5; i++) {
        cur_intensity = 1.0 - fabs(i / 5.0);
        pix->setPixelColor((i_pix + i) % len_stripe,
                          uint8_t(cur_intensity * r),
                          uint8_t(cur_intensity * g),
                          uint8_t(cur_intensity * b));
    }
}

void add_dimmed_pixel(Adafruit_NeoPixel * pix, int len_stripe, int i_pix, uint8_t r, uint8_t g, uint8_t b) {
    float cur_intensity;
    int pos;
    for (int i = -5; i <= 5; i++) {
        cur_intensity = 1.0 - fabs(i / 5.0);
        pos = i_pix + i;
        if (pos >= 0 && pos < len_stripe) {
            pix->setPixelColor(pos,
                              uint8_t(cur_intensity * r),
                              uint8_t(cur_intensity * g),
                              uint8_t(cur_intensity * b));
        }
    }
}

void delete_dimmed_pixel(Adafruit_NeoPixel * pix, int len_stripe, int i_pix) {
    add_dimmed_pixel(pix, len_stripe, i_pix, 0, 0, 0);
    /*int pos;
    for (int i = -5; i <= 5; i++) {
        pos = i_pix + i;
        if (pos >= 0 && pos < len_stripe) {
            pix->setPixelColor(pos, 0, 0, 0);
        }
    }*/
}

void delete_dimmed_pixel_loop(Adafruit_NeoPixel * pix, int len_stripe, int i_pix) {
    add_dimmed_pixel_loop(pix, len_stripe, i_pix, 0, 0, 0);
    /*for (int i = -5; i <= 5; i++) {
        pix->setPixelColor((i_pix + i) % len_stripe, 0, 0, 0);
    }*/
}

SoftwareSerial ser(RX_PIN, TX_PIN);

void setup() {
    ser.begin(BAUD);
    Serial.begin(9600);
    pixels_backlight.begin();
    pixels_backlight.show();

    srand(millis());
}

bool draw_open[NUM_DRAWS];
float draw_open_count[NUM_DRAWS];
bool any_draw_open;
int key_draw_open;
uint32_t last_effect_time = 0;
int effect_delay_val = 500;
int cur_effect = 0;
float cur_intensity = 1.0;
uint8_t * cur_color;
bool downwards = true;
int effect_index = 0;
int effect_index2 = 0;
bool eff_forward = 1;
bool eff_downwards = false;
float int_r = 0.;
float int_g = 0.;
float int_b = 0.;
long last = 0;
 
void loop() {
    key_draw_open = read_is_open(&ser);
    any_draw_open = key_draw_open >= 0;
    /*if (millis() - last > 100) {
        if (any_draw_open) {
            if (key_draw_open == 0) {
                Serial.write("0");
            }
            if (key_draw_open == 1) {
                Serial.write("1");
            }
            if (key_draw_open == 2) {
                Serial.write("2");
            }
            if (key_draw_open == 3) {
                Serial.write("3");
            }
            if (key_draw_open == 4) {
                Serial.write("4");
            }
            if (key_draw_open == 5) {
                Serial.write("5");
            }
        }
        else {
            Serial.write('n');
        }
        last = millis();
    }*/

    // if a draw is open set the backlight color to the color of the chosen
    // liqour and iluminate the draw (last 6 leds on the inside strip)
    if (any_draw_open) {
        if (millis() - last_effect_time > 10) {
            last_effect_time = millis();
            // set the backlight to constant color that slightly changes in
            // intensity over time
            cur_color = colors[draws[key_draw_open]];
            for (int i = 0; i < NUM_LEDS_BACKLIGHT; i++) {
                pixels_backlight.setPixelColor(i,
                                               int(cur_intensity * cur_color[0]),
                                               int(cur_intensity * cur_color[1]),
                                               int(cur_intensity * cur_color[2]));
            }
            if (downwards) {
                if (cur_intensity > 0.3) {
                    cur_intensity -= 0.05;
                }
                else {
                    downwards = false;
                }
            }
            else {
                if (cur_intensity < 1.0) {
                    cur_intensity += 0.05;
                }
                else {
                    downwards = true;
                }
            }

            // and iluminate the draw
            //pixels_inside.setPixelColor(NUM_LEDS_INSIDE - 6 + key_draw_open,
            //                            colors[draws[key_draw_open]]);
            // reset the effect such that a new effect starts when the draw
            // is closed
            cur_effect = 0;
        }
    }

    // otherwise display an effect on the backlight
    else {
        switch (cur_effect) {
            case 0:
                // last effect is finished start a new one
                cur_effect = (rand() % NUM_EFFECTS) + 1;
                switch(cur_effect) {
                    case 1:
                        effect_delay_val = DELAY_RUNNING_MIN +
                            rand() % (DELAY_RUNNING_MAX - DELAY_RUNNING_MIN);
                        cur_color = colors[rand() % NUM_COLORS];
                        cur_intensity = 1.0;
                        break;
                    case 2:
                        effect_delay_val = DELAY_RUNNING_MIN +
                            rand() % (DELAY_RUNNING_MAX - DELAY_RUNNING_MIN);
                        cur_intensity = 0.2;
                    case 3:
                        effect_index = 0;
                        effect_index2 = NUM_LEDS_BACKLIGHT / 4;

                        break;
                    case 4:
                        int_r = 1.0;
                        int_g = 0.0;
                        int_b = 0.0;
                        break;
                }
                last_effect_time = 0;
                effect_index = 0;
                effect_index2 = 0;
                eff_forward = true;
                downwards = true;

                // and disable the light in the draws as a draw
                // might have been opened before
                //for (int i = 0; i < 6; i++) {
                //    pixels_inside.setPixelColor(NUM_LEDS_INSIDE - 6 + i, 0, 0, 0);
                //}
                for (int i = 0; i < NUM_LEDS_BACKLIGHT; i++) {
                    pixels_backlight.setPixelColor(i, 0, 0, 0);
                }
                break;
            case 1:
                // running fixed color
                if (millis() - last_effect_time > effect_delay_val) {
                    last_effect_time = millis();
                    add_dimmed_pixel_loop(&pixels_backlight,
                                          NUM_LEDS_BACKLIGHT,
                                          effect_index2,
                                          uint8_t(cur_intensity * cur_color[0]),
                                          uint8_t(cur_intensity * cur_color[1]),
                                          uint8_t(cur_intensity * cur_color[2]));

                    effect_index++;
                    if (eff_forward) {
                        effect_index2++;
                    }
                    else {
                        effect_index2--;
                    }
                    if (effect_index % NUM_LEDS_BACKLIGHT == 0 && rand() > RAND_MAX / 2 ) {
                        eff_forward = !eff_forward;
                    }
                    if (effect_index == 2 * NUM_LEDS_BACKLIGHT) {
                        cur_effect = 0;
                    }
                }
                break;

            case 2:
                // random spots
                if (cur_intensity > 0.0) {
                    if (millis() - last_effect_time > effect_delay_val) {
                        last_effect_time = millis();
                        add_dimmed_pixel_loop(&pixels_backlight, NUM_LEDS_BACKLIGHT,
                                              effect_index2,
                                              uint8_t(cur_intensity * cur_color[0]),
                                              uint8_t(cur_intensity * cur_color[1]),
                                              uint8_t(cur_intensity * cur_color[2]));
                        if (eff_forward && cur_intensity < 1.0) {
                            cur_intensity += 0.05;
                        }
                        else {
                            eff_forward = false;
                            cur_intensity -= 0.05;
                        }
                    }

                }
                else {
                    delete_dimmed_pixel_loop(&pixels_backlight,
                                             NUM_LEDS_BACKLIGHT,
                                             effect_index2);
                    cur_intensity = 0.05;
                    eff_forward = true;
                    cur_color = colors[rand() % NUM_COLORS];
                    effect_index2 = rand() % NUM_LEDS_BACKLIGHT;
                    effect_index++;
                    if (effect_index > 10 && rand() > RAND_MAX / 2) {
                        cur_effect = 0;
                    }
                }

            case 3:
                // catching  one another
                cur_effect = 0;
                break;

            /*case 4:
                // rainbow
                if (millis() - last_effect_time > effect_delay_val) {
                    last_effect_time = millis();
                    if (int_r == 1. && int_b == 0. && int_b <= 1.0) {
                        int_b += 0.05
                    }
                    else if 
                }
                cur_effect = 0;
                break;*/
        }
    }


    pixels_backlight.show();

}

