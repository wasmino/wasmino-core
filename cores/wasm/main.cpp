#include "Browserino.h"

WASM_EXPORT
void __browserino_setup() {
	setup();
}

WASM_EXPORT
void __browserino_loop() {
	loop();
}
