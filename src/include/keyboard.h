#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

// Keyboard constants
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_COMMAND_PORT 0x64

// Standard PS/2 Scancodes (Set 1) - These are hardware scancodes
// They are the same regardless of physical keyboard layout (TR, US, DE, etc.)
#define KEY_ESCAPE    0x01
#define KEY_1         0x02
#define KEY_2         0x03
#define KEY_3         0x04
#define KEY_4         0x05
#define KEY_5         0x06
#define KEY_6         0x07
#define KEY_7         0x08
#define KEY_8         0x09
#define KEY_9         0x0A
#define KEY_0         0x0B
#define KEY_MINUS     0x0C
#define KEY_EQUALS    0x0D
#define KEY_BACKSPACE 0x0E
#define KEY_TAB       0x0F
#define KEY_Q         0x10
#define KEY_W         0x11
#define KEY_E         0x12
#define KEY_R         0x13
#define KEY_T         0x14
#define KEY_Y         0x15
#define KEY_U         0x16
#define KEY_I         0x17
#define KEY_O         0x18
#define KEY_P         0x19
#define KEY_LBRACKET  0x1A
#define KEY_RBRACKET  0x1B
#define KEY_ENTER     0x1C
#define KEY_LCTRL     0x1D
#define KEY_A         0x1E
#define KEY_S         0x1F
#define KEY_D         0x20
#define KEY_F         0x21
#define KEY_G         0x22
#define KEY_H         0x23
#define KEY_J         0x24
#define KEY_K         0x25
#define KEY_L         0x26
#define KEY_SEMICOLON 0x27
#define KEY_QUOTE     0x28
#define KEY_BACKTICK  0x29
#define KEY_LSHIFT    0x2A
#define KEY_BACKSLASH 0x2B
#define KEY_Z         0x2C
#define KEY_X         0x2D
#define KEY_C         0x2E
#define KEY_V         0x2F
#define KEY_B         0x30
#define KEY_N         0x31
#define KEY_M         0x32
#define KEY_COMMA     0x33
#define KEY_PERIOD    0x34
#define KEY_SLASH     0x35
#define KEY_RSHIFT    0x36
#define KEY_KP_STAR   0x37
#define KEY_LALT      0x38
#define KEY_SPACE     0x39
#define KEY_CAPSLOCK  0x3A

// Function keys
#define KEY_F1        0x3B
#define KEY_F2        0x3C
#define KEY_F3        0x3D
#define KEY_F4        0x3E
#define KEY_F5        0x3F
#define KEY_F6        0x40
#define KEY_F7        0x41
#define KEY_F8        0x42
#define KEY_F9        0x43
#define KEY_F10       0x44
#define KEY_NUMLOCK   0x45
#define KEY_SCROLLLOCK 0x46

// Numpad and navigation keys
#define KEY_KP_7      0x47
#define KEY_KP_8      0x48
#define KEY_KP_9      0x49
#define KEY_KP_MINUS  0x4A
#define KEY_KP_4      0x4B
#define KEY_KP_5      0x4C
#define KEY_KP_6      0x4D
#define KEY_KP_PLUS   0x4E
#define KEY_KP_1      0x4F
#define KEY_KP_2      0x50
#define KEY_KP_3      0x51
#define KEY_KP_0      0x52
#define KEY_KP_DOT    0x53

// These are the same scancodes used for navigation when NumLock is off
// Or accessed via arrow keys on modern keyboards
#define KEY_HOME      0x47
#define KEY_UP        0x48
#define KEY_PGUP      0x49
#define KEY_LEFT      0x4B
#define KEY_RIGHT     0x4D
#define KEY_END       0x4F
#define KEY_DOWN      0x50
#define KEY_PGDOWN    0x51
#define KEY_INSERT    0x52
#define KEY_DELETE    0x53

// Extended key prefix
#define KEY_EXTENDED  0xE0

// Special character codes (internal use - returned by keyboard_getchar)
#define CHAR_TAB       '\t'
#define CHAR_DELETE    0x7F
#define CHAR_BACKSPACE '\b'
#define CHAR_NEWLINE   '\n'
#define CHAR_ESCAPE    27

// Function prototypes
void keyboard_init(void);
void keyboard_handler(void);
char keyboard_getchar(void);
bool keyboard_key_pressed(void);

#endif // KEYBOARD_H