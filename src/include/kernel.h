#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"

// Kernel constants
#define KERNEL_VERSION "TRAKOS v1.0"

// Function prototypes
void kernel_main(void);
void kernel_panic(const char* message);
void safe_mode_test(void);

#endif // KERNEL_H