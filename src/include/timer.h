#ifndef TIMER_H
#define TIMER_H

#include "types.h"

// Timer constants
#define PIT_FREQUENCY    1193180
#define TIMER_IRQ        0
#define TIMER_COMMAND    0x43
#define TIMER_DATA       0x40

// Timer configuration
#define TIMER_MODE_RATE_GENERATOR    0x34

// Function prototypes
void timer_init(uint32_t frequency);
void timer_handler(void);
uint32_t timer_get_ticks(void);
void timer_wait(uint32_t ticks);
void timer_sleep(uint32_t milliseconds);

// External assembly function
extern void irq0_handler(void);

#endif // TIMER_H