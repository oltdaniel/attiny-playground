#include <avr/io.h>
#include <util/delay.h>

#include "utils.h"

// Schematics
// - = - = - = -
//
//   VCC
//    |     +------+ +----
//    |     |      | |   |
//    |     |      | |   |
//    | | | |      | |   |
//   +--------+   +---+  |
//   |        |   |But|  |
//   |ATTINY25|   |ton|  |
//   |        |   +---+  |
//   +--------+          |
//    | | | |            |
//        | |            |
//   +----+ |------------+
//   |      |
// +---+    |
// |LED|----|
// +---+    |
//          GND

int main (void) {
	// setup pb4 for output
	high(DDRB, PB4);
    // setup pb0 explicity to input
    low(DDRB, PB0);
    // enable internal pull-up on pb0
    high(PORTB, PB0);
	// program loop
	while(1){
        // check if bit is set on pin
        if(!(PINB & (1 << PB0))) {
            // if button pressed, turn led on
            high(PORTB, PB4);
            // wait for signal to end
            while(!(PINB & (1 << PB0)));
        } else {
            // default set to led off
            low(PORTB, PB4);
        }
    }
}