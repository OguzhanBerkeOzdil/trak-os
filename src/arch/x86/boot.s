.section .note.GNU-stack,"",@progbits

# Boot entry point - NO multiboot header here (it's in multiboot_simple.s)

# Reserve space for stack
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Entry point
.section .text
.global _start
.type _start, @function
_start:
    # Set up the stack
    mov $stack_top, %esp
    
    # Disable interrupts until we set up IDT
    cli
    
    # Call the kernel main function
    call kernel_main
    
    # If kernel_main returns, halt the system
    cli
1:  hlt
    jmp 1b

# Set the size of the _start symbol to the current location minus its start
.size _start, . - _start
