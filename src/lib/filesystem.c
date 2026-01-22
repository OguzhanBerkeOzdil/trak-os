#include "filesystem.h"
#include "memory.h"
#include "screen.h"
#include "timer.h"
#include "string.h"

// Global file system instance
static filesystem_t* fs = NULL;
static file_handle_t file_handles[8]; // Support up to 8 open files

// String functions (simple implementations)
static int fs_strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

static int fs_strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

static char* fs_strcpy(char* dest, const char* src) {
    char* original_dest = dest;
    while ((*dest++ = *src++));
    return original_dest;
}

static void* fs_memset(void* ptr, int value, uint32_t size) {
    unsigned char* p = (unsigned char*)ptr;
    while (size--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

static void* fs_memcpy(void* dest, const void* src, uint32_t size) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (size--) {
        *d++ = *s++;
    }
    return dest;
}

void fs_init(void) {
    // Allocate memory for file system
    fs = (filesystem_t*)kmalloc(sizeof(filesystem_t));
    if (!fs) {
        screen_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        screen_println("Failed to allocate memory for file system!");
        return;
    }
    
    // Initialize file system structure
    fs_memset(fs, 0, sizeof(filesystem_t));
    fs->total_files = MAX_FILES;
    fs->used_files = 0;
    fs->total_size = MAX_FILES * MAX_FILE_SIZE;
    fs->used_size = 0;
    
    // Initialize file handles
    for (int i = 0; i < 8; i++) {
        file_handles[i].file_index = -1;
        file_handles[i].position = 0;
        file_handles[i].is_open = false;
    }
    
    // Create some demo files
    fs_create_file("readme.txt", FILE_TYPE_REGULAR);
    fs_create_file("welcome.txt", FILE_TYPE_REGULAR);
    fs_create_file("docs", FILE_TYPE_DIRECTORY);
    
    // Write some content to demo files
    file_handle_t* readme = fs_open_file("readme.txt");
    if (readme) {
        const char* content = "Welcome to TRAKOS File System!\nThis is a simple in-memory file system.\nUse 'ls' to list files.";
        fs_write_file(readme, content, fs_strlen(content));
        fs_close_file(readme);
    }
    
    file_handle_t* welcome = fs_open_file("welcome.txt");
    if (welcome) {
        const char* content = "Hello from TRAKOS!\nThis file system supports:\n- Create/delete files\n- Read/write operations\n- File listing";
        fs_write_file(welcome, content, fs_strlen(content));
        fs_close_file(welcome);
    }
}

void fs_print_info(void) {
    if (!fs) {
        screen_println("File system not initialized!");
        return;
    }
    
    screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    screen_println("File System Information:");
    screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    screen_print("Total files: ");
    // Simple number printing
    char num_str[16];
    int num = fs->total_files;
    int i = 0;
    if (num == 0) {
        num_str[i++] = '0';
    } else {
        char temp[16];
        int j = 0;
        while (num > 0) {
            temp[j++] = '0' + (num % 10);
            num /= 10;
        }
        while (j > 0) {
            num_str[i++] = temp[--j];
        }
    }
    num_str[i] = '\0';
    screen_print(num_str);
    
    screen_print(" / Used: ");
    num = fs->used_files;
    i = 0;
    if (num == 0) {
        num_str[i++] = '0';
    } else {
        char temp[16];
        int j = 0;
        while (num > 0) {
            temp[j++] = '0' + (num % 10);
            num /= 10;
        }
        while (j > 0) {
            num_str[i++] = temp[--j];
        }
    }
    num_str[i] = '\0';
    screen_println(num_str);
    
    screen_print("Total space: ");
    num = fs->total_size;
    i = 0;
    char temp[16];
    int j = 0;
    while (num > 0) {
        temp[j++] = '0' + (num % 10);
        num /= 10;
    }
    while (j > 0) {
        num_str[i++] = temp[--j];
    }
    num_str[i] = '\0';
    screen_print(num_str);
    
    screen_print(" bytes / Used: ");
    num = fs->used_size;
    i = 0;
    j = 0;
    while (num > 0) {
        temp[j++] = '0' + (num % 10);
        num /= 10;
    }
    while (j > 0) {
        num_str[i++] = temp[--j];
    }
    num_str[i] = '\0';
    screen_print(num_str);
    screen_println(" bytes");
}

int fs_create_file(const char* name, uint8_t type) {
    if (!fs || !name) return -1;
    
    // Check if file already exists
    if (fs_file_exists(name)) {
        return -2; // File already exists
    }
    
    // Find free file entry
    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs->files[i].in_use) {
            // Initialize file entry
            fs_strcpy(fs->files[i].name, name);
            fs->files[i].type = type;
            fs->files[i].permissions = FILE_PERM_READ | FILE_PERM_WRITE;
            fs->files[i].size = 0;
            fs->files[i].data_offset = i * MAX_FILE_SIZE;
            fs->files[i].created_time = timer_get_ticks();
            fs->files[i].in_use = true;
            
            fs->used_files++;
            return i; // Return file index
        }
    }
    
    return -3; // No free slots
}

int fs_delete_file(const char* name) {
    if (!fs || !name) return -1;
    
    // Find file
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].in_use && fs_strcmp(fs->files[i].name, name) == 0) {
            // Clear file entry
            fs_memset(&fs->files[i], 0, sizeof(file_entry_t));
            fs->used_files--;
            fs->used_size -= fs->files[i].size;
            return 0; // Success
        }
    }
    
    return -2; // File not found
}

file_handle_t* fs_open_file(const char* name) {
    if (!fs || !name) return NULL;
    
    // Find file
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].in_use && fs_strcmp(fs->files[i].name, name) == 0) {
            file_index = i;
            break;
        }
    }
    
    if (file_index == -1) return NULL; // File not found
    
    // Find free handle
    for (int i = 0; i < 8; i++) {
        if (!file_handles[i].is_open) {
            file_handles[i].file_index = file_index;
            file_handles[i].position = 0;
            file_handles[i].is_open = true;
            return &file_handles[i];
        }
    }
    
    return NULL; // No free handles
}

void fs_close_file(file_handle_t* handle) {
    if (handle && handle->is_open) {
        handle->file_index = -1;
        handle->position = 0;
        handle->is_open = false;
    }
}

int fs_read_file(file_handle_t* handle, void* buffer, uint32_t size) {
    if (!handle || !handle->is_open || !buffer || !fs) return -1;
    
    file_entry_t* file = &fs->files[handle->file_index];
    if (!file->in_use) return -1;
    
    // Check bounds
    if (handle->position >= file->size) return 0; // EOF
    
    uint32_t available = file->size - handle->position;
    uint32_t to_read = (size < available) ? size : available;
    
    // Copy data
    uint8_t* src = &fs->data_area[file->data_offset + handle->position];
    fs_memcpy(buffer, src, to_read);
    
    handle->position += to_read;
    return to_read;
}

int fs_write_file(file_handle_t* handle, const void* buffer, uint32_t size) {
    if (!handle || !handle->is_open || !buffer || !fs) return -1;
    
    file_entry_t* file = &fs->files[handle->file_index];
    if (!file->in_use) return -1;
    
    // Check if write would exceed file size limit
    if (handle->position + size > MAX_FILE_SIZE) {
        size = MAX_FILE_SIZE - handle->position;
    }
    
    if (size == 0) return 0;
    
    // Copy data
    uint8_t* dest = &fs->data_area[file->data_offset + handle->position];
    fs_memcpy(dest, buffer, size);
    
    handle->position += size;
    
    // Update file size if we wrote past the end
    if (handle->position > file->size) {
        fs->used_size += (handle->position - file->size);
        file->size = handle->position;
    }
    
    return size;
}

int fs_seek_file(file_handle_t* handle, uint32_t position) {
    if (!handle || !handle->is_open || !fs) return -1;
    
    file_entry_t* file = &fs->files[handle->file_index];
    if (!file->in_use) return -1;
    
    if (position > file->size) return -1; // Beyond EOF
    
    handle->position = position;
    return 0;
}

void fs_list_files(void) {
    if (!fs) {
        screen_println("File system not initialized!");
        return;
    }
    
    screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    screen_println("Directory Listing:");
    screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    bool found_any = false;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].in_use) {
            found_any = true;
            
            // Print file type indicator
            if (fs->files[i].type == FILE_TYPE_DIRECTORY) {
                screen_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
                screen_print("[DIR] ");
            } else {
                screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                screen_print("[FILE]");
            }
            
            screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            screen_print(" ");
            screen_print(fs->files[i].name);
            
            // Print size
            screen_print(" (");
            // Simple number printing for size
            char num_str[16];
            uint32_t num = fs->files[i].size;
            int j = 0;
            if (num == 0) {
                num_str[j++] = '0';
            } else {
                char temp[16];
                int k = 0;
                while (num > 0) {
                    temp[k++] = '0' + (num % 10);
                    num /= 10;
                }
                while (k > 0) {
                    num_str[j++] = temp[--k];
                }
            }
            num_str[j] = '\0';
            screen_print(num_str);
            screen_println(" bytes)");
        }
    }
    
    if (!found_any) {
        screen_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
        screen_println("No files found.");
    }
}

bool fs_file_exists(const char* name) {
    if (!fs || !name) return false;
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].in_use && fs_strcmp(fs->files[i].name, name) == 0) {
            return true;
        }
    }
    
    return false;
}

uint32_t fs_get_file_size(const char* name) {
    if (!fs || !name) return 0;
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].in_use && fs_strcmp(fs->files[i].name, name) == 0) {
            return fs->files[i].size;
        }
    }
    
    return 0;
}

const char* fs_get_type_string(uint8_t type) {
    switch (type) {
        case FILE_TYPE_REGULAR: return "FILE";
        case FILE_TYPE_DIRECTORY: return "DIR";
        default: return "UNKNOWN";
    }
}

uint32_t fs_get_free_space(void) {
    if (!fs) return 0;
    return fs->total_size - fs->used_size;
}