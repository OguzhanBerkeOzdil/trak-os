#include "shell.h"
#include "screen.h"
#include "keyboard.h"
#include "kernel.h"
#include "timer.h"
#include "memory.h"
#include "filesystem.h"
#include "io.h"

// String comparison function
static int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// String length function
static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// Command parsing - split command into parts
static int parse_command(const char* input, char* cmd, char* arg) {
    int i = 0, j = 0;
    
    // Skip leading spaces
    while (input[i] == ' ') i++;
    
    // Copy command
    while (input[i] && input[i] != ' ') {
        cmd[j++] = input[i++];
    }
    cmd[j] = '\0';
    
    // Skip spaces between command and argument
    while (input[i] == ' ') i++;
    
    // Copy argument
    j = 0;
    while (input[i]) {
        arg[j++] = input[i++];
    }
    arg[j] = '\0';
    
    return (strlen(arg) > 0) ? 2 : 1; // Return number of parts
}

// Simple number printing function
static void print_number(uint32_t num) {
    if (num == 0) {
        screen_print("0");
        return;
    }
    
    char buffer[16];
    int i = 0;
    
    // Convert number to string (reverse order)
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // Print in correct order
    for (int j = i - 1; j >= 0; j--) {
        char str[2] = {buffer[j], '\0'};
        screen_print(str);
    }
}

// Hex printing function
static void print_hex(uint32_t num) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[9]; // 8 hex digits + null terminator
    int i;
    
    for (i = 7; i >= 0; i--) {
        buffer[i] = hex_chars[num & 0xF];
        num >>= 4;
    }
    buffer[8] = '\0';
    
    screen_print(buffer);
}

// Available commands for tab completion
static const char* shell_commands[] = {
    "help", "clear", "echo", "about", "version", "time", "sleep", 
    "calc", "colors", "memory", "memtest", "ls", "cat", "create", 
    "delete", "edit", "copy", "fsinfo", "ps", "uptime", "sysinfo", "reboot"
};
#define NUM_COMMANDS (sizeof(shell_commands) / sizeof(shell_commands[0]))

// String compare with prefix matching
static int strncmp(const char* str1, const char* str2, int n) {
    for (int i = 0; i < n; i++) {
        if (str1[i] != str2[i]) return str1[i] - str2[i];
        if (str1[i] == '\0') return 0;
    }
    return 0;
}

// Tab completion function
static int tab_complete(char* buffer, int buffer_len) {
    if (buffer_len == 0) return buffer_len;
    
    const char* match = 0;
    int matches = 0;
    
    for (uint32_t i = 0; i < NUM_COMMANDS; i++) {
        if (strncmp(buffer, shell_commands[i], buffer_len) == 0) {
            match = shell_commands[i];
            matches++;
        }
    }
    
    if (matches == 1 && match) {
        // Single match - complete it
        int match_len = strlen(match);
        for (int i = buffer_len; i < match_len; i++) {
            buffer[i] = match[i];
            char str[2] = {match[i], '\0'};
            screen_print(str);
        }
        buffer[match_len] = ' ';
        buffer[match_len + 1] = '\0';
        screen_print(" ");
        return match_len + 1;
    } else if (matches > 1) {
        // Multiple matches - show options
        screen_println("");
        screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        for (uint32_t i = 0; i < NUM_COMMANDS; i++) {
            if (strncmp(buffer, shell_commands[i], buffer_len) == 0) {
                screen_print(shell_commands[i]);
                screen_print("  ");
            }
        }
        screen_println("");
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        shell_print_prompt();
        screen_print(buffer);
        return buffer_len;
    }
    
    return buffer_len;
}

void shell_init(void) {
    screen_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    screen_println("");
    screen_println("TRAKOS Shell v1.0");
    screen_println("Type 'help' for available commands");
    screen_println("");
}

void shell_print_prompt(void) {
    screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    screen_print("trakos> ");
    screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void shell_execute_command(const char* input) {
    char command[64];
    char argument[64];
    int parts = parse_command(input, command, argument);
    
    if (strcmp(command, "help") == 0) {
        screen_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
        screen_println("Available commands:");
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        screen_println("  help      - Show this help message");
        screen_println("  clear     - Clear the screen");
        screen_println("  echo      - Echo text back");
        screen_println("  about     - Show system information");
        screen_println("  version   - Show kernel version");
        screen_println("  time      - Show system uptime");
        screen_println("  sleep     - Sleep for 2 seconds (demo)");
        screen_println("  calc      - Simple calculator (add 5+3)");
        screen_println("  colors    - Show color test");
        screen_println("  memory    - Show memory statistics");
        screen_println("  memtest   - Test memory allocation");
        screen_println("  ls        - List files in file system");
        screen_println("  cat <file> - Display file contents");
        screen_println("  create <file> - Create new file");
        screen_println("  delete <file> - Delete file");
        screen_println("  edit <file> - Edit file contents");
        screen_println("  copy <src> <dst> - Copy file");
        screen_println("  fsinfo    - Show file system information");
        screen_println("  ps        - Show system processes");
        screen_println("  uptime    - Show detailed system uptime");
        screen_println("  sysinfo   - Show complete system info");
        screen_println("  reboot    - Restart the system");
        screen_println("  memory    - Show memory statistics");
        screen_println("  memtest   - Test memory allocation");
        screen_println("  reboot    - Restart the system");
        
    } else if (strcmp(command, "clear") == 0) {
        screen_clear();
        
    } else if (strcmp(command, "version") == 0) {
        screen_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        screen_println("TRAKOS Kernel Version 1.0");
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        screen_println("Build date: 2024");
        screen_println("Features: Timer, Interrupts, Shell, Memory");
        
    } else if (strcmp(command, "about") == 0) {
        screen_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        screen_println("TRAKOS - Operating System");
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        screen_println("Version: 1.0");
        screen_println("Features: VGA Display, Keyboard, Shell, Timer, Memory");
        screen_println("Architecture: x86 32-bit");
        screen_println("Kernel: C with Assembly");
        
    } else if (strcmp(command, "calc") == 0) {
        screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        screen_println("Simple Calculator Demo:");
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        screen_print("5 + 3 = ");
        print_number(5 + 3);
        screen_println("");
        screen_print("10 - 4 = ");
        print_number(10 - 4);
        screen_println("");
        screen_print("6 * 7 = ");
        print_number(6 * 7);
        screen_println("");
        screen_print("20 / 4 = ");
        print_number(20 / 4);
        screen_println("");
        
    } else if (strcmp(command, "colors") == 0) {
        screen_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        screen_print("RED ");
        screen_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        screen_print("GREEN ");
        screen_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
        screen_print("BLUE ");
        screen_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
        screen_print("YELLOW ");
        screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        screen_print("CYAN ");
        screen_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
        screen_print("MAGENTA");
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        screen_println("");
        
    } else if (strcmp(command, "memory") == 0) {
        memory_print_stats();
        
    } else if (strcmp(command, "memtest") == 0) {
        screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        screen_println("Memory Allocation Test:");
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        
        // Test allocations
        void* ptr1 = kmalloc(1024);
        screen_print("Allocated 1024 bytes at: 0x");
        print_hex((uint32_t)ptr1);
        screen_println("");
        
        void* ptr2 = kmalloc(2048);
        screen_print("Allocated 2048 bytes at: 0x");
        print_hex((uint32_t)ptr2);
        screen_println("");
        
        screen_println("Freeing first allocation...");
        kfree(ptr1);
        
        screen_println("Freeing second allocation...");
        kfree(ptr2);
        
        screen_println("Memory test complete!");
        
    } else if (strcmp(command, "time") == 0) {
        uint32_t ticks = timer_get_ticks();
        uint32_t seconds = ticks / 100; // 100Hz timer
        uint32_t minutes = seconds / 60;
        uint32_t hours = minutes / 60;
        
        screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        screen_print("System uptime: ");
        
        // Simple number to string conversion
        if (hours > 0) {
            print_number(hours);
            screen_print("h ");
        }
        if (minutes % 60 > 0 || hours > 0) {
            print_number(minutes % 60);
            screen_print("m ");
        }
        print_number(seconds % 60);
        screen_println("s");
        
        screen_print("Total ticks: ");
        print_number(ticks);
        screen_println("");
        
    } else if (strcmp(command, "sleep") == 0) {
        screen_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
        screen_println("Sleeping for 2 seconds...");
        timer_sleep(2000); // 2000 milliseconds
        screen_println("Done sleeping!");
        
    } else if (strcmp(command, "echo") == 0) {
        screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        screen_println("Echo: Type something after 'echo' command");
        
    } else if (strcmp(command, "ls") == 0) {
        fs_list_files();
        
    } else if (strcmp(command, "fsinfo") == 0) {
        fs_print_info();
        
    } else if (strcmp(command, "cat") == 0) {
        if (parts < 2) {
            screen_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            screen_println("Usage: cat <filename>");
            return;
        }
        
        file_handle_t* file = fs_open_file(argument);
        if (file) {
            screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
            screen_print("Content of ");
            screen_print(argument);
            screen_println(":");
            screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            
            char buffer[512];
            int bytes_read = fs_read_file(file, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                screen_println(buffer);
            } else {
                screen_println("File is empty or error reading file.");
            }
            fs_close_file(file);
        } else {
            screen_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            screen_print("File '");
            screen_print(argument);
            screen_println("' not found!");
        }
        
    } else if (strcmp(command, "create") == 0) {
        if (parts < 2) {
            screen_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            screen_println("Usage: create <filename>");
            return;
        }
        
        int result = fs_create_file(argument, FILE_TYPE_REGULAR);
        if (result >= 0) {
            screen_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            screen_print("Created file '");
            screen_print(argument);
            screen_println("' successfully!");
            
            // Write some default content
            file_handle_t* file = fs_open_file(argument);
            if (file) {
                const char* content = "New file created by TRAKOS!\nEdit this file with 'edit filename'";
                fs_write_file(file, content, strlen(content));
                fs_close_file(file);
                screen_println("Added default content to the file.");
            }
        } else {
            screen_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            if (result == -2) {
                screen_print("File '");
                screen_print(argument);
                screen_println("' already exists!");
            } else {
                screen_println("Failed to create file!");
            }
        }
        
    } else if (strcmp(command, "delete") == 0) {
        if (parts < 2) {
            screen_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            screen_println("Usage: delete <filename>");
            return;
        }
        
        int result = fs_delete_file(argument);
        if (result == 0) {
            screen_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            screen_print("Deleted file '");
            screen_print(argument);
            screen_println("' successfully!");
        } else {
            screen_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            screen_print("File '");
            screen_print(argument);
            screen_println("' not found or error deleting!");
        }
        
    } else if (strcmp(command, "edit") == 0) {
        if (parts < 2) {
            screen_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            screen_println("Usage: edit <filename>");
            return;
        }
        
        screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        screen_print("Simple Text Editor for: ");
        screen_println(argument);
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        screen_println("Type your text (press ESC when done):");
        screen_println("---");
        
        // Simple text input for file editing
        char file_content[512] = {0};
        uint32_t content_pos = 0;
        
        while (content_pos < sizeof(file_content) - 1) {
            char key = keyboard_getchar();
            if (key == 27) { // ESC key
                break;
            } else if (key == '\n') {
                file_content[content_pos++] = '\n';
                screen_println("");
            } else if (key == '\b') {
                if (content_pos > 0) {
                    content_pos--;
                    screen_print("\b \b");
                }
            } else if (key >= 32 && key <= 126) { // Printable characters
                file_content[content_pos++] = key;
                char str[2] = {key, '\0'};
                screen_print(str);
            }
        }
        
        // Save the file
        int result = fs_create_file(argument, FILE_TYPE_REGULAR);
        if (result == -2) { // File exists, delete and recreate
            fs_delete_file(argument);
            result = fs_create_file(argument, FILE_TYPE_REGULAR);
        }
        
        if (result >= 0) {
            file_handle_t* file = fs_open_file(argument);
            if (file) {
                fs_write_file(file, file_content, content_pos);
                fs_close_file(file);
                screen_println("");
                screen_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
                screen_print("File '");
                screen_print(argument);
                screen_println("' saved successfully!");
            }
        }
        
    } else if (strcmp(command, "ps") == 0) {
        screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        screen_println("System Processes:");
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        screen_println("  PID  NAME        STATUS   MEMORY");
        screen_println("   1   kernel      running   4MB");
        screen_println("   2   shell       running   1MB");
        screen_println("   3   filesystem  running   2MB");
        screen_println("   4   timer       running   <1MB");
        screen_println("Total: 4 processes");
        
    } else if (strcmp(command, "uptime") == 0) {
        uint32_t ticks = timer_get_ticks();
        uint32_t seconds = ticks / 100;
        uint32_t minutes = seconds / 60;
        uint32_t hours = minutes / 60;
        uint32_t days = hours / 24;
        
        screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        screen_println("System Uptime Details:");
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        
        screen_print("Total uptime: ");
        if (days > 0) {
            print_number(days);
            screen_print(" days, ");
        }
        if (hours % 24 > 0) {
            print_number(hours % 24);
            screen_print(" hours, ");
        }
        print_number(minutes % 60);
        screen_print(" minutes, ");
        print_number(seconds % 60);
        screen_println(" seconds");
        
        screen_print("Timer ticks: ");
        print_number(ticks);
        screen_println(" (100Hz)");
        
        screen_print("Average load: ");
        print_number((ticks % 1000) / 10);
        screen_println("%");
        
    } else if (strcmp(command, "sysinfo") == 0) {
        screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        screen_println("TRAKOS System Information:");
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        
        screen_println("OS Name: TRAKOS");
        screen_println("Version: 1.0 Enhanced");
        screen_println("Architecture: x86 32-bit");
        screen_println("Kernel: C with Assembly");
        screen_println("");
        
        screen_println("Hardware:");
        screen_println("  CPU: x86 Compatible");
        screen_println("  Memory: 4MB heap + system");
        screen_println("  Display: VGA Text Mode 80x25");
        screen_println("  Timer: PIT 100Hz");
        screen_println("");
        
        screen_println("Features:");
        screen_println("  - Interrupt-driven I/O");
        screen_println("  - Memory management");
        screen_println("  - File system (in-memory)");
        screen_println("  - Shell with 20+ commands");
        screen_println("  - Text editor");
        screen_println("  - Process simulation");
        
    } else if (strcmp(command, "reboot") == 0) {
        screen_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        screen_println("Rebooting system...");
        // Reboot using keyboard controller
        outb(0x64, 0xFE);
        
    } else if (command[0] == '\0') {
        // Empty command, do nothing
        
    } else {
        screen_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        screen_print("Unknown command: ");
        screen_println(command);
        screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        screen_println("Type 'help' for available commands");
    }
}

void shell_run(void) {
    shell_print_prompt();
    
    char command_buffer[SHELL_BUFFER_SIZE];
    int buffer_index = 0;
    
    while (1) {
        char key = keyboard_getchar();
        if (key) {
            if (key == '\n') {
                command_buffer[buffer_index] = '\0';
                screen_println("");
                
                shell_execute_command(command_buffer);
                
                buffer_index = 0;
                shell_print_prompt();
            } else if (key == '\b') {
                // Backspace - delete character before cursor
                if (buffer_index > 0) {
                    buffer_index--;
                    command_buffer[buffer_index] = '\0';
                    screen_print("\b \b");
                }
            } else if (key == 0x7F) {
                // Delete key - same as backspace in terminal mode
                if (buffer_index > 0) {
                    buffer_index--;
                    command_buffer[buffer_index] = '\0';
                    screen_print("\b \b");
                }
            } else if (key == '\t') {
                // Tab - auto-complete
                command_buffer[buffer_index] = '\0';
                buffer_index = tab_complete(command_buffer, buffer_index);
            } else if (key >= 32 && key < 127 && buffer_index < SHELL_BUFFER_SIZE - 1) {
                // Regular printable character
                command_buffer[buffer_index++] = key;
                char str[2] = {key, '\0'};
                screen_print(str);
            }
        }
    }
}