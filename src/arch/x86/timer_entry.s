.section .note.GNU-stack,"",@progbits

.section .text

# IRQ0 (Timer) interrupt handler
.global irq0_handler
.type irq0_handler, @function
irq0_handler:
    # Save ALL registers and flags
    pushf                    # Save flags
    pusha                    # Save all general registers
    
    # Save segment registers
    push %ds
    push %es
    push %fs
    push %gs
    
    # Set up kernel data segments
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    
    # Call C timer handler
    call timer_handler
    
    # Send EOI (End of Interrupt) to PIC
    mov $0x20, %al
    out %al, $0x20
    
    # Restore segment registers
    pop %gs
    pop %fs
    pop %es
    pop %ds
    
    # Restore all registers and flags
    popa                     # Restore all general registers
    popf                     # Restore flags
    iret                     # Return from interrupt

.size irq0_handler, . - irq0_handler
