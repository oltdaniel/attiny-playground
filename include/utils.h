#ifndef _UTILS_H
#define _UTILS_H

// explicity set a bit on a io port to state
#define write(port, index, state) port ^= (-state ^ index) & (1 << index);
// alias to set io port to high
#define low(port, index) write(port, index, 0);
// alias to set io port to low
#define high(port, index) write(port, index, 1);

#endif