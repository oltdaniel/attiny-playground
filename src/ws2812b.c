/**
 * This code controls an WS2812B led string.
 * 
 * Code inspired by: https://github.com/cpldcpu/light_ws2812 
 * 
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/**
 * These are our definitions where and what the pixel strip is.
 */
#define PIXEL_PORT PORTB
#define PIXEL_PIN 3
#define PIXEL_NUM 12

/**
 * We need some helpers to calculate the nanoseconds to cycles.
 */
#define NS_PER_SECOND (1000000000L)
#define CYCLES_PER_SECOND (F_CPU)
#define NS_PER_CYCLE (NS_PER_SECOND / CYCLES_PER_SECOND)
#define NS_TO_CYCLES(n) ((n) / NS_PER_CYCLE)

/**
 * These are some fancy cycle consumers from light_ws2812
 */
#define w_nop1 "nop      \n\t"
#define w_nop2 "brid .+0 \n\t"
#define w_nop4 w_nop2 w_nop2
#define w_nop8 w_nop4 w_nop4
#define w_nop16 w_nop8 w_nop8

/**
 * The WS2812B led can send to codes:
 * 
 * - Code0: 400ns +- 150ns high followed by 850ns +- 150ns low
 * - Code1: 850ns +- 150ns high followed by 400ns +- 150ns low
 * 
 * The high singal has an minimum of 400ns, so we have a base time
 * of 400ns for our high signal. If this time passed, and we want
 * to send Code0, we pull the signal low. If not, continue with
 * an high signal. We will then wait for the delta time
 * (Code0 high - Code1 high) to allow Code1 to finish its high signal
 * as we will pull it down after these 400ns passed.
 * 
 * This will then follow by the reaming time (450ns) to fulfill our
 * 1250ns data impuls. This will result in:
 * 
 * - Code0:
 * -----| 850ns
 * 400ns|________
 * 
 *      |  |
 *       ^^- this is the delta between the high signals
 * 
 * - Code1:
 * --------|400ns
 * 850ns---|_____
 * 
 * As we use nop's to wait for the certain cycles, we need to watch
 * our own code overhead of cycles. There we have 2 for the base cycles,
 * 4 for our delta cycles and a total of 8 cycles.
 */
#define BASE_CYCLES (NS_TO_CYCLES(400) - 2)
#define DELTA_CYCLES (NS_TO_CYCLES(400) - 4)
#define FINAL_CYCLES (NS_TO_CYCLES(450) - 8)

/**
 * We take the num_bytes bytes our of data and send them out to the defined port.
 * The global interrupt will be disabled and if it was set, enabled again in the end.
 */
static inline void sendBytes(uint8_t *data, uint8_t num_bytes)
{
    // this little helper, will be counted down for the shifts
    uint8_t i;
    // point to the current byte
    uint8_t *cur = data;
    // save our current GIEB SREG state
    uint8_t gieb_prev = SREG & (1 << 7);
    // clear interrupe bit
    cli();
    while (num_bytes--)
    {
        // throw the byte out to the defined port
        __asm__ volatile(
            "ldi %[i], 8 \n\t" // set our i to 8 for counting down
            "loop: \n\t" // define our internal loop
            "sbi %[port], %[pin] \n\t" // set pin to high to start signal
// these base cycles represent the 400ns the signal will always be up independet of code0 or code1
#if (BASE_CYCLES & 1)
    w_nop1
#endif
#if (BASE_CYCLES & 2)
    w_nop2
#endif
#if (BASE_CYCLES & 4)
    w_nop4
#endif
#if (BASE_CYCLES & 8)
    w_nop8
#endif
#if (BASE_CYCLES & 16)
    w_nop16
#endif

            "sbrs %[data], 7 \n\t" // if the bit 7 (current) is set, ignore the next line
            "cbi %[port], %[pin] \n\t" // if not, pull the signal down
            "lsl %[data] \n\t" // do the shift for our data already

// these delat cycles represent the difference between the high signal of code0 and code1
#if (DELTA_CYCLES & 1)
    w_nop1
#endif
#if (DELTA_CYCLES & 2)
    w_nop2
#endif
#if (DELTA_CYCLES & 4)
    w_nop4
#endif
#if (DELTA_CYCLES & 8)
    w_nop8
#endif
#if (DELTA_CYCLES & 16)
    w_nop16
#endif

            "cbi %[port], %[pin] \n\t" // pull the signal down (same result if already pulled down)

// these cycles represent the reaming low time to finish the code
#if (FINAL_CYCLES & 1)
    w_nop1
#endif
#if (FINAL_CYCLES & 2)
    w_nop2
#endif
#if (FINAL_CYCLES & 4)
    w_nop4
#endif
#if (FINAL_CYCLES & 8)
    w_nop8
#endif
#if (FINAL_CYCLES & 16)
    w_nop16
#endif

            "dec %[i] \n\t" // decrement our loop counter
            "brne loop \n\t" // if we reached zero, exit, else jump back to loop
            : [i] "=&d"(i)
            :
            [data] "r"(*cur), // we need the value of the current byte we want to send
            [port] "I"(_SFR_IO_ADDR(PIXEL_PORT)), // as we use assembly, we need to address for our port
            [pin] "I"(PIXEL_PIN)); // the pin we use for communication
        // next byte
        cur++;
    }
    // restore sreg state
    SREG |= gieb_prev;
}

/**
 * The WS2812B define their reset time for 50µs in the datasheet.
 */
static inline void show()
{
    // waiting for 50µs
    _delay_us(50);
}

/**
 * Here we will set all leds to the same color.
 */
void showColor(uint8_t r, uint8_t g, uint8_t b)
{
    // create the data array to show on the WS2812B
    // important: the will interpret the colors with grb not rgb.
    uint8_t data[] = {g, r, b};
    for (uint8_t i = 0; i < PIXEL_NUM; i++)
    {
        sendBytes(data, 3);
    }
    // make sure to send the reset signal to update the colors.
    show();
}

int main()
{
    // initialize the pixel pin as an output
    DDRB |= (1 << PIXEL_PIN);
    // make sure the pin is low
    PORTB &= ~(1 <<PIXEL_PIN);
    // our program loop
    for (;;)
    {
        // make sure red works
        showColor(0xFF, 0, 0);
        _delay_ms(1000);
        // make sure red works
        showColor(0, 0xFF, 0);
        _delay_ms(1000);
        // make sure red works
        showColor(0, 0, 0xFF);
        _delay_ms(1000);
    }
}