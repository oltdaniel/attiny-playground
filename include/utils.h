#ifndef _UTILS_H
#define _UTILS_H

// alias to set io port to high
#define low(port, index) port &= ~(1 << index);
// alias to set io port to low
#define high(port, index) port |= (1 << index);

#endif