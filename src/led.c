#include <avr/io.h>
#include <util/delay.h>

#include "utils.h"

// Schematics
// - = - = - = -
//
//   VCC
//    |
//    |
//    |
//    | | | |
//   +--------+
//   |        |
//   |ATTINY25|
//   |        |
//   +--------+
//    | | | |
//        | |
//   +----+ |
//   |      |
// +---+    |
// |LED|----|
// +---+    |
//          GND

int main (void) {
	// setup pb4 for output
	high(DDRB, PB4);
	// keep track of led status
	uint8_t up = 0;
	// program loop
	while(1){
		// timeout
		_delay_ms(10);
		// NOTE: I know I could toggle the up variable,
		//   but I wanto to show off the high/low commands.

		// check state
		if(up) {
			// set from low to high
			high(PORTB, PB4);
			// remember state
			up = 0;
		} else {
			// set from high to low
			low(PORTB, PB4);
			// remember state
			up = 1;
		}
	}
}
