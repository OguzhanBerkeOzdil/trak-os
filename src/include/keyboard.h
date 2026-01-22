#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

// Keyboard constants
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_COMMAND_PORT 0x64

// Special keys
#define KEY_ESCAPE    0x01
#define KEY_BACKSPACE 0x0E
#define KEY_TAB       0x0F
#define KEY_ENTER     0x1C
#define KEY_LSHIFT    0x2A
#define KEY_RSHIFT    0x36
#define KEY_LCTRL     0x1D
#define KEY_LALT      0x38
#define KEY_SPACE     0x39

// Function prototypes
void keyboard_init(void);
void keyboard_handler(void);
char keyboard_getchar(void);
bool keyboard_key_pressed(void);

#endif // KEYBOARD_H