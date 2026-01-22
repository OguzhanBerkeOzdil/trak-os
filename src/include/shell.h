#ifndef SHELL_H
#define SHELL_H

#include "types.h"

#define SHELL_BUFFER_SIZE 256

// Function prototypes
void shell_init(void);
void shell_run(void);
void shell_execute_command(const char* command);
void shell_print_prompt(void);

#endif // SHELL_H