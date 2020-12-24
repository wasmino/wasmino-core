#ifndef Browserino_h
#define Browserino_h

#include "Arduino.h"
#include "BrowserinoSyscall.h"

#define WASM_EXPORT __attribute__((visibility("default")))

// list of components supported
#define BUTTON 1
#define LED    2

#define COMPONENT_ARDUINO 0

// uint8_t placeComponent(uint8_t type, int8_t x = -1, int8_t y = -1, uint8_t w = 1, uint8_t h = 1);
// void connectComponent(uint8_t a, uint8_t pinA, uint8_t b, uint8_t pinB);

#endif
