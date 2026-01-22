.section .note.GNU-stack,"",@progbits

# Multiboot specification constants
.set MULTIBOOT_MAGIC,       0x1BADB002
.set MULTIBOOT_FLAGS,       0x00000000  # No flags - simplest possible
.set MULTIBOOT_CHECKSUM,    -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

# Multiboot header - MUST be in first 8KB of kernel
.section .multiboot,"a"
.align 4
multiboot_header:
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long MULTIBOOT_CHECKSUM
