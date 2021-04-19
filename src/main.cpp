#include <Arduino.h>

#define STORE_CLOCK (10)
#define SHIFT_CLOCK (11)
#define SERIAL_DATA (12)

void setup() {
	pinMode( STORE_CLOCK, OUTPUT);
	pinMode( SHIFT_CLOCK, OUTPUT);
	pinMode( SERIAL_DATA, OUTPUT);
}

void loop() {
	for (int i = 0; i < 255; i++) {
		digitalWrite( STORE_CLOCK, LOW );
		shiftOut(SERIAL_DATA,
				SHIFT_CLOCK,
				MSBFIRST,
				i);
		digitalWrite(STORE_CLOCK, HIGH);
		delay(200);
	}
}
