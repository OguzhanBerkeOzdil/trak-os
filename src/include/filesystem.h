#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "types.h"

// File system constants
#define MAX_FILES 32
#define MAX_FILENAME_LENGTH 16
#define MAX_FILE_SIZE 1024
#define FS_BLOCK_SIZE 512

// File types
#define FILE_TYPE_REGULAR 1
#define FILE_TYPE_DIRECTORY 2

// File permissions
#define FILE_PERM_READ    0x01
#define FILE_PERM_WRITE   0x02
#define FILE_PERM_EXECUTE 0x04

// File system structures
typedef struct {
    char name[MAX_FILENAME_LENGTH];
    uint8_t type;
    uint8_t permissions;
    uint32_t size;
    uint32_t data_offset;  // Offset in data area
    uint32_t created_time; // Timestamp
    bool in_use;
} file_entry_t;

typedef struct {
    uint32_t total_files;
    uint32_t used_files;
    uint32_t total_size;
    uint32_t used_size;
    file_entry_t files[MAX_FILES];
    uint8_t data_area[MAX_FILES * MAX_FILE_SIZE];
} filesystem_t;

// File handle for operations
typedef struct {
    int file_index;
    uint32_t position;
    bool is_open;
} file_handle_t;

// File system functions
void fs_init(void);
void fs_print_info(void);

// File operations
int fs_create_file(const char* name, uint8_t type);
int fs_delete_file(const char* name);
file_handle_t* fs_open_file(const char* name);
void fs_close_file(file_handle_t* handle);

// File I/O operations
int fs_read_file(file_handle_t* handle, void* buffer, uint32_t size);
int fs_write_file(file_handle_t* handle, const void* buffer, uint32_t size);
int fs_seek_file(file_handle_t* handle, uint32_t position);

// Directory operations
void fs_list_files(void);
bool fs_file_exists(const char* name);
uint32_t fs_get_file_size(const char* name);

// Utility functions
const char* fs_get_type_string(uint8_t type);
uint32_t fs_get_free_space(void);

#endif