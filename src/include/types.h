/*
 * Trak-OS Type Definitions
 * Copyright (c) 2025 Oğuzhan Berke Özdil
 * 
 * Standard type definitions for the kernel
 */

#ifndef TYPES_H
#define TYPES_H

// Fixed-size integer types
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long long uint64_t;

typedef signed char  int8_t;
typedef signed short int16_t;
typedef signed int   int32_t;
typedef signed long long int64_t;

// Boolean type
typedef uint8_t bool;
#define true  1
#define false 0

// Pointer-sized integers
typedef uint32_t uintptr_t;
typedef int32_t intptr_t;

// Size type
typedef uint32_t size_t;

// Boolean definitions
#define true  1
#define false 0
typedef uint8_t bool;

// Language definitions
typedef enum {
    LANG_ENGLISH = 0,
    LANG_TURKISH = 1
} language_t;

// Global language setting (extern declaration)
extern language_t global_language;

// NULL pointer
#ifndef NULL
#define NULL ((void*)0)
#endif

// Variadic arguments support
typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)
#define va_copy(dest, src) __builtin_va_copy(dest, src)

#endif // TYPES_H
