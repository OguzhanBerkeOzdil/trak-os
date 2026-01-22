#include "idt.h"
#include "io.h"
#include "keyboard.h"
#include "timer.h"

// IDT table with 256 entries
static struct idt_entry idt_entries[256];
static struct idt_ptr idt_ptr;

// Assembly interrupt handlers
extern void irq0_handler(void);  // Timer
extern void irq1_handler(void);  // Keyboard

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_low = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    idt_entries[num].selector = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags = flags;
}

void idt_init(void) {
    idt_ptr.limit = sizeof(struct idt_entry) * 256 - 1;
    idt_ptr.base = (uint32_t)&idt_entries;
    
    // Clear IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Set timer interrupt (IRQ0 = interrupt 32)
    idt_set_gate(32, (uint32_t)irq0_handler, 0x08, 0x8E);
    
    // Set keyboard interrupt (IRQ1 = interrupt 33)
    idt_set_gate(33, (uint32_t)irq1_handler, 0x08, 0x8E);
    
    // Load IDT
    asm volatile("lidt %0" : : "m" (idt_ptr));
    
    // Remap PIC
    outb(0x20, 0x11);  // Initialize PIC1
    outb(0xA0, 0x11);  // Initialize PIC2
    outb(0x21, 0x20);  // PIC1 offset to 0x20 (32)
    outb(0xA1, 0x28);  // PIC2 offset to 0x28 (40)
    outb(0x21, 0x04);  // PIC1 uses IRQ2 to connect to PIC2
    outb(0xA1, 0x02);  // PIC2 is connected to IRQ2 of PIC1
    outb(0x21, 0x01);  // 8086 mode for PIC1
    outb(0xA1, 0x01);  // 8086 mode for PIC2
    outb(0x21, 0xFC);  // Enable IRQ0 (timer) and IRQ1 (keyboard)
    outb(0xA1, 0xFF);  // Disable all IRQs on PIC2
}