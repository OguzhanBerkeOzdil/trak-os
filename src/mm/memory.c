#include "../include/memory.h"
#include "../include/screen.h"

// Forward declaration of print_number function
static void print_number(uint32_t num);

// Global memory management state
static memory_block_t* heap_start = NULL;
static bool memory_initialized = false;
static uint32_t total_allocated = 0;

void memory_init(void) {
    // Initialize the heap with a single free block
    heap_start = (memory_block_t*)HEAP_START;
    heap_start->size = HEAP_SIZE - sizeof(memory_block_t);
    heap_start->is_free = true;
    heap_start->next = NULL;
    heap_start->prev = NULL;
    
    memory_initialized = true;
    total_allocated = 0;
}

void* kmalloc(uint32_t size) {
    if (!memory_initialized) {
        memory_init();
    }
    
    if (size == 0) {
        return NULL;
    }
    
    // Align to 4-byte boundary
    size = (size + 3) & ~3;
    
    memory_block_t* current = heap_start;
    
    // Find a free block that's large enough
    while (current) {
        if (current->is_free && current->size >= size) {
            // Split the block if it's significantly larger
            if (current->size > size + sizeof(memory_block_t) + 32) {
                memory_block_t* new_block = (memory_block_t*)((uint8_t*)current + sizeof(memory_block_t) + size);
                new_block->size = current->size - size - sizeof(memory_block_t);
                new_block->is_free = true;
                new_block->next = current->next;
                new_block->prev = current;
                
                if (current->next) {
                    current->next->prev = new_block;
                }
                current->next = new_block;
                current->size = size;
            }
            
            current->is_free = false;
            total_allocated += current->size;
            
            // Return pointer to the data area (after the header)
            return (uint8_t*)current + sizeof(memory_block_t);
        }
        current = current->next;
    }
    
    return NULL; // Out of memory
}

void kfree(void* ptr) {
    if (!ptr || !memory_initialized) {
        return;
    }
    
    // Get the block header
    memory_block_t* block = (memory_block_t*)((uint8_t*)ptr - sizeof(memory_block_t));
    
    if (block->is_free) {
        return; // Already freed
    }
    
    block->is_free = true;
    total_allocated -= block->size;
    
    // Merge with next block if it's free
    if (block->next && block->next->is_free) {
        block->size += block->next->size + sizeof(memory_block_t);
        if (block->next->next) {
            block->next->next->prev = block;
        }
        block->next = block->next->next;
    }
    
    // Merge with previous block if it's free
    if (block->prev && block->prev->is_free) {
        block->prev->size += block->size + sizeof(memory_block_t);
        if (block->next) {
            block->next->prev = block->prev;
        }
        block->prev->next = block->next;
    }
}

void* krealloc(void* ptr, uint32_t new_size) {
    if (!ptr) {
        return kmalloc(new_size);
    }
    
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    memory_block_t* block = (memory_block_t*)((uint8_t*)ptr - sizeof(memory_block_t));
    
    if (block->size >= new_size) {
        return ptr; // Current block is large enough
    }
    
    // Allocate new block and copy data
    void* new_ptr = kmalloc(new_size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, block->size < new_size ? block->size : new_size);
        kfree(ptr);
    }
    
    return new_ptr;
}

uint32_t memory_get_total(void) {
    return HEAP_SIZE;
}

uint32_t memory_get_used(void) {
    return total_allocated;
}

uint32_t memory_get_free(void) {
    return HEAP_SIZE - total_allocated;
}

void memory_print_stats(void) {
    screen_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    screen_println("Memory Statistics:");
    screen_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    screen_print("Total heap size: ");
    print_number(HEAP_SIZE / 1024);
    screen_println(" KB");
    
    screen_print("Used memory: ");
    print_number(total_allocated);
    screen_println(" bytes");
    
    screen_print("Free memory: ");
    print_number(memory_get_free());
    screen_println(" bytes");
    
    screen_print("Usage: ");
    if (HEAP_SIZE > 0) {
        print_number((total_allocated * 100) / HEAP_SIZE);
        screen_println("%");
    } else {
        screen_println("0%");
    }
}

// Utility functions
void* memcpy(void* dest, const void* src, uint32_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    
    for (uint32_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    
    return dest;
}

void* memset(void* ptr, int value, uint32_t n) {
    uint8_t* p = (uint8_t*)ptr;
    
    for (uint32_t i = 0; i < n; i++) {
        p[i] = (uint8_t)value;
    }
    
    return ptr;
}

int memcmp(const void* ptr1, const void* ptr2, uint32_t n) {
    const uint8_t* p1 = (const uint8_t*)ptr1;
    const uint8_t* p2 = (const uint8_t*)ptr2;
    
    for (uint32_t i = 0; i < n; i++) {
        if (p1[i] < p2[i]) return -1;
        if (p1[i] > p2[i]) return 1;
    }
    
    return 0;
}

// Helper function declaration (should be defined elsewhere)
// extern void print_number(uint32_t num);

// Simple number printing function (local implementation)
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