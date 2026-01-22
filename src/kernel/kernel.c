#include "../include/kernel.h"
#include "../include/screen.h"
#include "../include/keyboard.h"
#include "../include/idt.h"
#include "../include/shell.h"
#include "../include/timer.h"
#include "../include/memory.h"
#include "../include/filesystem.h"
#include "../include/io.h"
#include "../include/types.h"

// Set to 0 for interrupt mode, 1 for safe polling mode
#define SAFE_MODE 1

// Display the boot logo with colors
static void display_boot_logo(void) {
    // Set cyan background with dark text for banner area
    screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLUE);
    screen_println("                                                                                ");
    
    screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    screen_println("  ████████╗██████╗  █████╗ ██╗  ██╗     ██████╗ ███████╗                        ");
    screen_println("  ╚══██╔══╝██╔══██╗██╔══██╗██║ ██╔╝    ██╔═══██╗██╔════╝                        ");
    screen_println("     ██║   ██████╔╝███████║█████╔╝     ██║   ██║███████╗                        ");
    screen_println("     ██║   ██╔══██╗██╔══██║██╔═██╗     ██║   ██║╚════██║                        ");
    screen_println("     ██║   ██║  ██║██║  ██║██║  ██╗    ╚██████╔╝███████║                        ");
    screen_println("     ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝     ╚═════╝ ╚══════╝                        ");
    
    screen_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLUE);
    screen_println("            T R A K Y A   O P E R A T I N G   S Y S T E M                       ");
    
    screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLUE);
    screen_println("                                                                                ");
    
    screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    screen_println("");
}

void kernel_main(void) {
    // Initialize screen driver
    screen_init();
    screen_clear();
    
    // Display boot logo
    display_boot_logo();
    
    // Display system startup
    screen_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    screen_print("[ OK ] "); screen_println("VGA Display Driver");
    screen_print("[ OK ] "); screen_println("Screen Functions");
    screen_print("[ OK ] "); screen_println("Color Support");
    
    // Initialize IDT
    idt_init();
    screen_print("[ OK ] "); screen_println("Interrupt Descriptor Table");
    
    // Initialize timer (100Hz frequency)
    timer_init(100);
    screen_print("[ OK ] "); screen_println("Timer Driver (100Hz)");
    
    // Initialize memory management
    memory_init();
    screen_print("[ OK ] "); screen_println("Memory Management (4MB Heap)");
    
    // Initialize file system
    fs_init();
    screen_print("[ OK ] "); screen_println("File System (In-Memory)");
    
    // Initialize keyboard
    keyboard_init();
    screen_print("[ OK ] "); screen_println("Keyboard Driver (Universal PS/2)");
    
    // Initialize shell
    shell_init();
    screen_print("[ OK ] "); screen_println("Shell Interface");
    screen_println("");
    
    if (SAFE_MODE) {
        screen_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
        screen_println("System Status: SAFE MODE (Polling)");
        screen_println("Features: Keyboard polling test");
        screen_println("");
        
        safe_mode_test();
    } else {
        screen_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        screen_println("System Status: INTERACTIVE MODE");
        screen_println("Features: Full interrupt-driven interface");
        screen_println("");
        
        // Enable interrupts
        asm volatile("sti");
        screen_print("[ OK ] "); screen_println("Interrupts Enabled");
        screen_println("");
        
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        screen_println("Welcome to TRAKOS! Type 'help' for available commands.");
        
        // Start the shell
        shell_run();
    }
}

void safe_mode_test(void) {
    screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    screen_println("Running in SAFE MODE with polling keyboard...");
    screen_println("All features available, but using polling instead of interrupts.");
    screen_println("");
    
    screen_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    screen_println("Testing keyboard... Press any key to continue to shell:");
    screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    // Test keyboard input first
    keyboard_getchar();
    screen_println("Keyboard works! Starting shell...");
    screen_println("");
    
    screen_println("Welcome to TRAKOS! Type 'help' for available commands.");
    
    // Start the shell in polling mode
    shell_run();
}

void kernel_panic(const char* message) {
    screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    screen_println("");
    screen_println("KERNEL PANIC!");
    screen_print("Error: ");
    screen_println(message);
    screen_println("System halted.");
    
    // Disable interrupts and halt
    asm volatile("cli");
    while(1) {
        asm volatile("hlt");
    }
}