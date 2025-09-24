#ifndef PINS_H
#define PINS_H

// LEDek
#define LED_PIN         21
#define NUM_LEDS        3
#define LED_TYPE        WS2812B
#define COLOR_ORDER     GRB

// Stepper motorok
#define MOTOR_COUNT     3
const int dirPins[MOTOR_COUNT]  = {11, 2, 13};
const int stepPins[MOTOR_COUNT] = {12, 1, 14};
#define MOTOR_RESET     39

// Végálláskapcsolók+
const int LSwitch[MOTOR_COUNT] = {42, 41, 40};

// Szervó motor (gripper)
#define GRIPPER_PIN     38
#endif // PINS_H

// SX1 -> CN4 -> 42
// SX2 -> CN5 -> 41
// SX3 -> CN6 -> 40