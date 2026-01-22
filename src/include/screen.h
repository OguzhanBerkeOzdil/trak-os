#ifndef SCREEN_H
#define SCREEN_H

#include "types.h"

// VGA colors
#define VGA_COLOR_BLACK         0
#define VGA_COLOR_BLUE          1
#define VGA_COLOR_GREEN         2
#define VGA_COLOR_CYAN          3
#define VGA_COLOR_RED           4
#define VGA_COLOR_MAGENTA       5
#define VGA_COLOR_BROWN         6
#define VGA_COLOR_LIGHT_GREY    7
#define VGA_COLOR_DARK_GREY     8
#define VGA_COLOR_LIGHT_BLUE    9
#define VGA_COLOR_LIGHT_GREEN   10
#define VGA_COLOR_LIGHT_CYAN    11
#define VGA_COLOR_LIGHT_RED     12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN   14
#define VGA_COLOR_YELLOW        14
#define VGA_COLOR_WHITE         15

// Screen dimensions
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

// Function prototypes
void screen_init(void);
void screen_clear(void);
void screen_putchar(char c);
void screen_print(const char* str);
void screen_println(const char* str);
void screen_set_color(uint8_t fg, uint8_t bg);
void screen_scroll(void);

#endif // SCREEN_H