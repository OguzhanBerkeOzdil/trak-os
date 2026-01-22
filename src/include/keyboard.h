#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

// Keyboard constants
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_COMMAND_PORT 0x64

// Special keys (scancodes)
#define KEY_ESCAPE    0x01
#define KEY_BACKSPACE 0x0E
#define KEY_TAB       0x0F
#define KEY_ENTER     0x1C
#define KEY_LSHIFT    0x2A
#define KEY_RSHIFT    0x36
#define KEY_LCTRL     0x1D
#define KEY_LALT      0x38
#define KEY_SPACE     0x39
#define KEY_DELETE    0x53
#define KEY_UP        0x48
#define KEY_DOWN      0x50
#define KEY_LEFT      0x4B
#define KEY_RIGHT     0x4D

// Special character codes (internal use)
#define CHAR_TAB      '\t'
#define CHAR_DELETE   0x7F
#define CHAR_BACKSPACE '\b'

// Function prototypes
void keyboard_init(void);
void keyboard_handler(void);
char keyboard_getchar(void);
bool keyboard_key_pressed(void);

#endif // KEYBOARD_H