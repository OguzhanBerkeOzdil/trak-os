#include "keyboard.h"
#include "io.h"
#include "screen.h"

// US QWERTY keyboard layout
static const char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

// Shift map for uppercase letters and symbols
static const char keyboard_shift_map[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

// Keyboard state
static bool shift_pressed = false;
static char last_key = 0;
static bool key_available = false;

void keyboard_init(void) {
    // Enable keyboard interrupts
    outb(0x21, inb(0x21) & 0xFD);  // Clear bit 1 (IRQ1)
}

void keyboard_handler(void) {
    // Check if data is ready before reading
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
    
    if (!(status & 0x01)) {
        // No data available, return (EOI handled by assembly wrapper)
        return;
    }
    
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Handle key release (scancode & 0x80)
    if (scancode & 0x80) {
        scancode &= 0x7F;  // Remove release flag
        
        // Handle shift release
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = false;
        }
        return;
    }
    
    // Handle key press
    if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
        shift_pressed = true;
        return;
    }
    
    // Convert scancode to character
    char key = 0;
    if (scancode < 128) {
        if (shift_pressed) {
            key = keyboard_shift_map[scancode];
        } else {
            key = keyboard_map[scancode];
        }
    }
    
    if (key != 0) {
        last_key = key;
        key_available = true;
    }
}

char keyboard_getchar(void) {
    // Pure polling mode - keep checking for keys
    while (1) {
        // Check keyboard status
        uint8_t status = inb(KEYBOARD_STATUS_PORT);
        if (status & 0x01) {
            uint8_t scancode = inb(KEYBOARD_DATA_PORT);
            
            // Handle key release
            if (scancode & 0x80) {
                scancode &= 0x7F;
                if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
                    shift_pressed = false;
                }
                continue;
            }
            
            // Handle key press
            if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
                shift_pressed = true;
                continue;
            }
            
            // Convert scancode to character
            char key = 0;
            if (scancode < 128) {
                if (shift_pressed) {
                    key = keyboard_shift_map[scancode];
                } else {
                    key = keyboard_map[scancode];
                }
            }
            
            if (key != 0) {
                return key;
            }
        }
        
        // Check if interrupt mode has a key ready
        if (key_available) {
            char key = last_key;
            key_available = false;
            return key;
        }
        
        // Small delay for polling - no halt instruction
        for (volatile int i = 0; i < 10000; i++) {}
    }
}

bool keyboard_key_pressed(void) {
    return key_available;
}