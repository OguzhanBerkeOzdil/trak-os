# TRAK-OS

A minimal 32-bit operating system written in C and Assembly.

**Author:** Oğuzhan Berke Özdil

## Features

- **VGA Text Mode Display** - 80x25 character output with 16 colors
- **PS/2 Keyboard Driver** - Full US QWERTY layout with shift support
- **Interactive Shell** - Command-line interface with multiple commands
- **Memory Management** - Dynamic heap allocation (kmalloc/kfree)
- **In-Memory Filesystem** - Create, read, write, delete files
- **Timer Driver** - System uptime and sleep functionality
- **Interrupt Handling** - IDT setup with PIC remapping

## Project Structure

```
trak-os/
├── build.ps1              # Build script (PowerShell + WSL)
├── start.ps1              # Run script (QEMU)
├── src/
│   ├── arch/x86/          # x86 assembly code
│   │   ├── boot.s         # Entry point
│   │   ├── multiboot_simple.s
│   │   ├── keyboard_entry.s
│   │   └── timer_entry.s
│   ├── kernel/
│   │   ├── kernel.c       # Main kernel
│   │   ├── idt.c          # Interrupt descriptor table
│   │   └── linker.ld      # Linker script
│   ├── drivers/
│   │   ├── screen/        # VGA driver
│   │   ├── keyboard/      # Keyboard driver
│   │   ├── timer/         # Timer driver
│   │   └── shell/         # Shell interface
│   ├── mm/
│   │   └── memory.c       # Memory allocator
│   ├── lib/
│   │   └── filesystem.c   # In-memory filesystem
│   └── include/           # Header files
└── isodir/                # ISO output directory
```

## Shell Commands

| Command | Description |
|---------|-------------|
| `help` | Show available commands |
| `clear` | Clear the screen |
| `about` | System information |
| `version` | Kernel version |
| `time` | System uptime |
| `memory` | Memory statistics |
| `memtest` | Test memory allocation |
| `ls` | List files |
| `cat <file>` | Display file contents |
| `create <file>` | Create new file |
| `edit <file>` | Edit file contents |
| `delete <file>` | Delete file |
| `colors` | Display color test |
| `calc` | Calculator demo |
| `reboot` | Restart system |

## Building

### Prerequisites

- Windows with WSL (Windows Subsystem for Linux)
- GCC cross-compiler (32-bit)
- GRUB tools (`grub-mkrescue`)
- QEMU (optional, for testing)

### Build Commands

```powershell
# Build the ISO
.\build.ps1

# Run in QEMU (if installed)
.\start.ps1
```

The build produces `trakos.iso` which can be used with:
- QEMU (`qemu-system-i386 -cdrom trakos.iso`)
- VirtualBox
- VMware
- Real hardware (boot from CD/USB)

## Technical Details

- **Architecture:** x86 32-bit (i386)
- **Boot:** GRUB Multiboot
- **Memory:** 4MB heap starting at 0x100000
- **Video:** VGA text mode (0xB8000)
- **Keyboard:** Port 0x60/0x64

## License

MIT License - see [LICENSE](LICENSE) file.