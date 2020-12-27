#include "Wasmino.h"

#define _BSD_SOURCE
#include <time.h>
#include <limits.h>

extern "C" {

#define DEFAULT_PIN_COUNT 16
#define MAX_PIN_COUNT 256

#define WASMINO_ATTR_INTERRUPT_ENABLED     (1 << 0)
#define WASMINO_ATTR_IN_INTERRUPT          (1 << 1)

#define WASMINO_PIN_ATTR_OUTPUT            (1 << 0)
#define WASMINO_PIN_ATTR_ANALOG            (1 << 1)
#define WASMINO_PIN_ATTR_INTERRUPT_RISING  (1 << 2)
#define WASMINO_PIN_ATTR_INTERRUPT_FALLING (1 << 3)

unsigned long microseconds = 0;

#define ASYNCIFY_BUFFER_SIZE 1024
uint32_t asyncifyBuffer[1024] = { 0 };

struct wasminoPinState {
	unsigned long attr;
	float input;
	float output;
	void (*isr)();
};

struct wasminoState {
	unsigned long attr;
	unsigned long pinCount;
	struct wasminoPinState pinStates[MAX_PIN_COUNT];
	struct timespec uptime;
	float interruptThreshold;
};

struct wasminoState state = {
	.attr = WASMINO_ATTR_INTERRUPT_ENABLED,
	.pinCount = DEFAULT_PIN_COUNT,
	.pinStates = { 0 },
	.uptime = { 0 },
	.interruptThreshold = 0.5f,
};

struct wasminoPinState *pinStates = state.pinStates;


#ifdef WASMINO_EXT_NANOSLEEP
extern void _nanosleep(unsigned long s, unsigned long ns) __attribute__((import_module("wasmino"), import_name("nanosleep")));
#else

static void _nanosleep(unsigned long s, unsigned long ns) {
	struct timespec req = {
		.tv_sec = s,
		.tv_nsec = ns,
	};
	struct timespec rem;
	do {
		rem = { 0 };
		nanosleep(&req, &rem);
		req = rem;
	} while (req.tv_sec > 0 || req.tv_nsec > 0);
}
#endif

static void wasminoNanosleep(unsigned long s, unsigned long ns) {
	if (state.attr & WASMINO_ATTR_IN_INTERRUPT) {
		// disable sleep during interrupt
		return;
	}
	struct timespec ts;
	ts = state.uptime;
	_nanosleep(s, ns);
	ts.tv_sec += s;
	ts.tv_nsec += ns;
	if (ts.tv_nsec > 1000000000) {
		ts.tv_nsec -= 1000000000;
		ts.tv_sec += 1;
	}
	state.uptime = ts;
}

unsigned long millis(void) {
	return state.uptime.tv_sec * 1000 + state.uptime.tv_nsec / 1000000;
}

unsigned long micros(void) {
	return state.uptime.tv_sec * 1000000 + state.uptime.tv_nsec / 1000;
}

void delay(unsigned long ms) {
	wasminoNanosleep(ms / 1000, (ms % 1000) * 1000000);
}

void delayMicroseconds(unsigned int us) {
	wasminoNanosleep(us / 1000000, (us % 1000000) * 1000);
}

void pinMode(uint8_t pin, uint8_t mode) {
	if (pin >= state.pinCount) {
		return;
	}
	struct wasminoPinState *ps = &pinStates[pin];
	switch (mode)
	{
	case INPUT:
		ps->attr &= WASMINO_PIN_ATTR_OUTPUT;
		break;
	case INPUT_PULLUP:
		ps->attr &= WASMINO_PIN_ATTR_OUTPUT;
		ps->output = 1.f;
		break;
	case OUTPUT:
		ps->attr |= WASMINO_PIN_ATTR_OUTPUT;
	default:
		break;
	}
}

int digitalRead(uint8_t pin)
{
	if (pin >= state.pinCount) {
		return LOW;
	}
	return pinStates[pin].input ? HIGH : LOW;
}

void digitalWrite(uint8_t pin, uint8_t val)
{
	if (pin >= state.pinCount) {
		return;
	}
	pinStates[pin].attr &= WASMINO_PIN_ATTR_ANALOG;
	pinStates[pin].output = val ? 1.f : 0.f;
}

int analogRead(uint8_t pin)
{
	if (pin >= state.pinCount) {
		return 0;
	}
	float value = pinStates[pin].input;
	return value * 255;
}

void analogReference(uint8_t mode)
{
	// NOT IMPLEMENTED
}

void analogWrite(uint8_t pin, int val)
{
	if (pin >= state.pinCount) {
		return;
	}
	if (val < 0) {
		val = 0;
	} else if (val > 255) {
		val = 255;
	}
	pinStates[pin].attr |= WASMINO_PIN_ATTR_ANALOG;
	pinStates[pin].output = val / 255.0f;
}

void attachInterrupt(uint8_t intr, void (*isr)(), int mode) {
	if (intr >= state.pinCount) {
		return;
	}
	pinStates[intr].attr &= WASMINO_PIN_ATTR_INTERRUPT_RISING;
	pinStates[intr].attr &= WASMINO_PIN_ATTR_INTERRUPT_FALLING;
	pinStates[intr].isr = isr;
	switch (mode) {
	case RISING:
		pinStates[intr].attr |= WASMINO_PIN_ATTR_INTERRUPT_RISING;
		break;
	case FALLING:
		pinStates[intr].attr |= WASMINO_PIN_ATTR_INTERRUPT_FALLING;
		break;
	case CHANGE:
		pinStates[intr].attr |= WASMINO_PIN_ATTR_INTERRUPT_RISING;
		pinStates[intr].attr |= WASMINO_PIN_ATTR_INTERRUPT_FALLING;
		break;
	default:
		break;
	}
}

void detachInterrupt(uint8_t intr) {
	if (intr >= state.pinCount) {
		return;
	}
	pinStates[intr].attr &= WASMINO_PIN_ATTR_INTERRUPT_RISING;
	pinStates[intr].attr &= WASMINO_PIN_ATTR_INTERRUPT_FALLING;
}

void interrupts() {
	state.attr |= WASMINO_ATTR_INTERRUPT_ENABLED;
}

void noInterrupts() {
	state.attr &= WASMINO_ATTR_INTERRUPT_ENABLED;
}

// Wasmino Extensions, client side
void rawWrite(uint8_t pin, float value)
{
	if (pin >= state.pinCount) {
		return;
	}
	pinStates[pin].output = value;
}

float rawRead(uint8_t pin)
{
	if (pin >= state.pinCount) {
		return 0.0f;
	}
	return pinStates[pin].input;
}

void setPinCount(uint8_t count)
{
	state.pinCount = count;
}

void interruptThreshold(float threshold) {
	state.interruptThreshold = threshold;
}

// Wasmino Extensions, host side
uint8_t getPinMode(uint8_t pin)
__attribute__((export_name("wasminoGetPinMode")))
{
	return pinStates[pin].attr & WASMINO_PIN_ATTR_OUTPUT ? OUTPUT : INPUT;
}

uint8_t getPinCount()
__attribute__((export_name("wasminoGetPinCount")))
{
	return state.pinCount;
}

float wasminoReadPin(uint8_t pin)
__attribute__((export_name("wasminoReadPin")))
{
	if (pin >= state.pinCount) {
		return 0.0f;
	}
	return pinStates[pin].output;
}

void wasminoWritePin(uint8_t pin, float value)
__attribute__((export_name("wasminoWritePin")))
{
	if (pin >= state.pinCount) {
		return;
	}
	struct wasminoPinState *ps = &pinStates[pin];
	float orig = ps->input;
	ps->input = value;
	if (!(state.attr & WASMINO_ATTR_INTERRUPT_ENABLED)) {
		return;
	}
	if (
		(
			(ps->attr & WASMINO_PIN_ATTR_INTERRUPT_RISING) &&
			(orig < state.interruptThreshold) &&
			(value >= state.interruptThreshold)
		) ||
		(
			(ps->attr & WASMINO_PIN_ATTR_INTERRUPT_FALLING) &&
			(orig > state.interruptThreshold) &&
			(value <= state.interruptThreshold)
		)
	) {
		ps->attr |= WASMINO_ATTR_IN_INTERRUPT;
		pinStates[pin].isr();
		ps->attr &= WASMINO_ATTR_IN_INTERRUPT;
	}
}

void wasminoSetUptime(unsigned long s, unsigned long ns)
__attribute__((export_name("wasminoSetUptime")))
{
	state.uptime.tv_sec = s;
	state.uptime.tv_nsec = ns;
}

void *_wasminoMalloc(size_t size)
__attribute__((export_name("malloc")))
{
	return malloc(size);
}

void _wasminoFree(void *ptr)
__attribute__((export_name("free")))
{
	free(ptr);
}

// main func
int main(int argc, const char *argv[])
{
	int iter = -1;
	if (argc >= 2) {
		iter = atoi(argv[1]);
	}
	delay(0);
	setup();
	delay(0);
	while (iter != 0) {
		loop();
		delay(0);
		if (iter > 0) {
			iter--;
		}
	}
}

}
