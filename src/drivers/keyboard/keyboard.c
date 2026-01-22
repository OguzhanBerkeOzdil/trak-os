#include "keyboard.h"
#include "io.h"
#include "screen.h"

// US QWERTY keyboard layout (works with most keyboards regardless of physical layout)
// Scancodes are hardware-level and layout-independent
static const char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   // 0x00-0x0E
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',    // 0x0F-0x1C
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,         // 0x1D-0x29 (0x1D=LCTRL)
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',            // 0x2A-0x37 (0x2A=LSHIFT, 0x36=RSHIFT)
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                             // 0x38-0x46 (ALT, SPACE, CAPS, F1-F10)
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0x7F, 0, 0,                     // 0x47-0x56 (Numpad, 0x53=DEL)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                            // 0x57-0x66
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                            // 0x67-0x76
    0, 0, 0, 0, 0, 0, 0, 0                                                      // 0x77-0x7F
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
static bool ctrl_pressed = false;
static char last_key = 0;
static bool key_available = false;

// Special key codes returned for non-printable keys
#define SPECIAL_KEY_UP    0x80
#define SPECIAL_KEY_DOWN  0x81
#define SPECIAL_KEY_LEFT  0x82
#define SPECIAL_KEY_RIGHT 0x83
#define SPECIAL_KEY_HOME  0x84
#define SPECIAL_KEY_END   0x85
#define SPECIAL_KEY_PGUP  0x86
#define SPECIAL_KEY_PGDN  0x87
#define SPECIAL_KEY_INS   0x88

void keyboard_init(void) {
    // Enable keyboard interrupts
    outb(0x21, inb(0x21) & 0xFD);  // Clear bit 1 (IRQ1)
}

// Process a single scancode and return the character (or 0 if none)
static char process_scancode(uint8_t scancode) {
    // Handle key release (scancode & 0x80)
    if (scancode & 0x80) {
        scancode &= 0x7F;  // Remove release flag
        
        // Handle modifier releases
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = false;
        } else if (scancode == KEY_LCTRL) {
            ctrl_pressed = false;
        }
        return 0;
    }
    
    // Handle modifier key presses
    if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
        shift_pressed = true;
        return 0;
    }
    if (scancode == KEY_LCTRL) {
        ctrl_pressed = true;
        return 0;
    }
    
    // Handle special keys that aren't in the regular map
    switch (scancode) {
        case KEY_TAB:       return '\t';
        case KEY_BACKSPACE: return '\b';
        case KEY_ENTER:     return '\n';
        case KEY_DELETE:    return 0x7F;  // DEL character
        case KEY_UP:        return SPECIAL_KEY_UP;
        case KEY_DOWN:      return SPECIAL_KEY_DOWN;
        case KEY_LEFT:      return SPECIAL_KEY_LEFT;
        case KEY_RIGHT:     return SPECIAL_KEY_RIGHT;
        case KEY_ESCAPE:    return 27;
    }
    
    // Convert scancode to character
    if (scancode < 128) {
        char key;
        if (shift_pressed) {
            key = keyboard_shift_map[scancode];
        } else {
            key = keyboard_map[scancode];
        }
        
        // Handle Ctrl+key combinations
        if (ctrl_pressed && key >= 'a' && key <= 'z') {
            return key - 'a' + 1;  // Ctrl+A = 1, Ctrl+C = 3, etc.
        }
        
        return key;
    }
    
    return 0;
}

void keyboard_handler(void) {
    // Check if data is ready before reading
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
    
    if (!(status & 0x01)) {
        return;
    }
    
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    char key = process_scancode(scancode);
    
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
            char key = process_scancode(scancode);
            
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