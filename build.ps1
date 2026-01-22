# TRAK-OS Build Script
# Builds the full featured OS with C kernel and drivers

Write-Host "Building TRAK-OS..." -ForegroundColor Green
Write-Host "Full featured kernel with drivers" -ForegroundColor Cyan

$ErrorActionPreference = "Stop"

function Run-WSL {
    param([string]$Command)
    Write-Host "Running: $Command" -ForegroundColor Blue
    $result = wsl bash -c "$Command"
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed!" -ForegroundColor Red
        exit 1
    }
    return $result
}

# Clean up old builds
Write-Host "Cleaning previous builds..." -ForegroundColor Yellow
Remove-Item -Force -ErrorAction SilentlyContinue *.iso
Remove-Item -Recurse -Force -ErrorAction SilentlyContinue build, isodir

# Create directories
Write-Host "Creating directories..." -ForegroundColor Yellow
New-Item -ItemType Directory -Force build, build/drivers, build/arch, build/mm, isodir/boot/grub | Out-Null

# Compile assembly files
Write-Host "Compiling boot assembly..." -ForegroundColor Yellow
Run-WSL "gcc -m32 -c src/arch/x86/multiboot_simple.s -o build/multiboot.o"
Run-WSL "gcc -m32 -c src/arch/x86/boot.s -o build/boot.o"

# Compile C kernel
Write-Host "Compiling kernel..." -ForegroundColor Yellow
Run-WSL "gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Isrc/include -c src/kernel/kernel.c -o build/kernel.o"
Run-WSL "gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Isrc/include -c src/kernel/idt.c -o build/idt.o"

# Compile drivers
Write-Host "Compiling drivers..." -ForegroundColor Yellow
Run-WSL "gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Isrc/include -c src/drivers/screen/screen.c -o build/drivers/screen.o"
Run-WSL "gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Isrc/include -c src/drivers/keyboard/keyboard.c -o build/drivers/keyboard.o"
Run-WSL "gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Isrc/include -c src/drivers/shell/shell.c -o build/drivers/shell.o"
Run-WSL "gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Isrc/include -c src/drivers/timer/timer.c -o build/drivers/timer.o"

# Compile memory management
Write-Host "Compiling memory management..." -ForegroundColor Yellow
Run-WSL "gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Isrc/include -c src/mm/memory.c -o build/mm/memory.o"

# Compile library functions
Write-Host "Compiling libraries..." -ForegroundColor Yellow
Run-WSL "gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Isrc/include -c src/lib/filesystem.c -o build/filesystem.o"

# Compile interrupt handlers
Run-WSL "gcc -m32 -c src/arch/x86/keyboard_entry.s -o build/arch/keyboard_entry.o"
Run-WSL "gcc -m32 -c src/arch/x86/timer_entry.s -o build/arch/timer_entry.o"

Write-Host "Linking kernel..." -ForegroundColor Yellow
Run-WSL "ld -m elf_i386 -T src/kernel/linker.ld -o isodir/boot/kernel.bin build/multiboot.o build/boot.o build/kernel.o build/idt.o build/arch/keyboard_entry.o build/arch/timer_entry.o build/drivers/screen.o build/drivers/keyboard.o build/drivers/shell.o build/drivers/timer.o build/mm/memory.o build/filesystem.o"

# Create GRUB config
Write-Host "Creating GRUB config..." -ForegroundColor Yellow
@"
set timeout=3
set default=0

menuentry "TRAK-OS" {
    multiboot /boot/kernel.bin
    boot
}
"@ | Out-File -FilePath "isodir/boot/grub/grub.cfg" -Encoding ASCII

# Create ISO
Write-Host "Creating ISO..." -ForegroundColor Yellow
Run-WSL "grub-mkrescue -o trakos.iso isodir"

Write-Host ""
Write-Host "===== BUILD SUCCESSFUL! =====" -ForegroundColor Green
Write-Host "Created: trakos.iso" -ForegroundColor Cyan
Write-Host ""
Write-Host "Run with: .\start.ps1" -ForegroundColor Yellow