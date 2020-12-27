#ifndef Wasmino_h
#define Wasmino_h

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

// Wasmino Extensions
void rawWrite(uint8_t pin, float value);
float rawRead(uint8_t pin);

void setPinCount(uint8_t count);
inline static void pinCount(uint8_t count) {
	setPinCount(count);
}

void interruptThreshold(float threshold);

#ifdef __cplusplus
}
#endif

#endif
