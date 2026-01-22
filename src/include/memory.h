#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

// Memory constants
#define PAGE_SIZE           4096
#define HEAP_START          0x400000   // 4MB
#define HEAP_SIZE           0x400000   // 4MB heap
#define HEAP_END            (HEAP_START + HEAP_SIZE)

// Memory block structure
typedef struct memory_block {
    uint32_t size;
    bool is_free;
    struct memory_block* next;
    struct memory_block* prev;
} memory_block_t;

// Function prototypes
void memory_init(void);
void* kmalloc(uint32_t size);
void kfree(void* ptr);
void* krealloc(void* ptr, uint32_t new_size);
uint32_t memory_get_total(void);
uint32_t memory_get_used(void);
uint32_t memory_get_free(void);
void memory_print_stats(void);

// Utility functions
void* memcpy(void* dest, const void* src, uint32_t n);
void* memset(void* ptr, int value, uint32_t n);
int memcmp(const void* ptr1, const void* ptr2, uint32_t n);

#endif // MEMORY_H