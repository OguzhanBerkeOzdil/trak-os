#include "../include/timer.h"
#include "../include/io.h"
#include "../include/screen.h"

// Global tick counter
static volatile uint32_t timer_ticks = 0;

void timer_init(uint32_t frequency) {
    // Calculate the divisor for the desired frequency
    uint32_t divisor = PIT_FREQUENCY / frequency;
    
    // Send command byte to PIT
    outb(TIMER_COMMAND, TIMER_MODE_RATE_GENERATOR);
    
    // Send divisor (low byte first, then high byte)
    outb(TIMER_DATA, divisor & 0xFF);
    outb(TIMER_DATA, (divisor >> 8) & 0xFF);
    
    // Reset tick counter
    timer_ticks = 0;
    
    // Enable timer interrupt (IRQ0)
    uint8_t mask = inb(0x21);
    mask &= ~0x01; // Enable IRQ0 (timer)
    outb(0x21, mask);
}

void timer_handler(void) {
    // Increment tick counter
    timer_ticks++;
    
    // Optional: Display a dot every second (if frequency is 100Hz, every 100 ticks)
    // Uncomment for debugging
    // if (timer_ticks % 100 == 0) {
    //     putchar('.');
    // }
}

uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

void timer_wait(uint32_t ticks) {
    uint32_t start_ticks = timer_ticks;
    while ((timer_ticks - start_ticks) < ticks) {
        // Wait for the specified number of ticks
        __asm__ volatile("hlt"); // Halt until next interrupt
    }
}

void timer_sleep(uint32_t milliseconds) {
    // Assuming timer frequency is 100Hz (10ms per tick)
    // Adjust this based on your timer frequency
    uint32_t ticks = milliseconds / 10;
    timer_wait(ticks);
}