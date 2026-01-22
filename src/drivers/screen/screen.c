#include "screen.h"
#include "io.h"

// VGA text mode buffer
static volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

// Current cursor position
static uint16_t cursor_row = 0;
static uint16_t cursor_col = 0;

// Current color
static uint8_t current_color = VGA_COLOR_LIGHT_GREY | (VGA_COLOR_BLACK << 4);

// Helper function to create VGA entry
static uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

// Update hardware cursor position
static void update_cursor(uint16_t pos) {
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void screen_init(void) {
    cursor_row = 0;
    cursor_col = 0;
    current_color = VGA_COLOR_LIGHT_GREY | (VGA_COLOR_BLACK << 4);
}

void screen_clear(void) {
    for (uint16_t y = 0; y < VGA_HEIGHT; y++) {
        for (uint16_t x = 0; x < VGA_WIDTH; x++) {
            const uint16_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', current_color);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
    update_cursor(0);
}

void screen_set_color(uint8_t fg, uint8_t bg) {
    current_color = fg | (bg << 4);
}

void screen_scroll(void) {
    // Move all lines up by one
    for (uint16_t y = 1; y < VGA_HEIGHT; y++) {
        for (uint16_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[(y - 1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
        }
    }
    
    // Clear the last line
    for (uint16_t x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', current_color);
    }
    
    cursor_row = VGA_HEIGHT - 1;
    cursor_col = 0;
}

void screen_putchar(char c) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
    } else if (c == '\r') {
        cursor_col = 0;
    } else if (c == '\t') {
        cursor_col = (cursor_col + 8) & ~(8 - 1);
    } else if (c >= ' ') {
        const uint16_t index = cursor_row * VGA_WIDTH + cursor_col;
        vga_buffer[index] = vga_entry(c, current_color);
        cursor_col++;
    }
    
    // Handle line wrapping
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }
    
    // Handle scrolling
    if (cursor_row >= VGA_HEIGHT) {
        screen_scroll();
    }
    
    // Update hardware cursor
    update_cursor(cursor_row * VGA_WIDTH + cursor_col);
}

void screen_print(const char* str) {
    while (*str) {
        screen_putchar(*str);
        str++;
    }
}

void screen_println(const char* str) {
    screen_print(str);
    screen_putchar('\n');
}