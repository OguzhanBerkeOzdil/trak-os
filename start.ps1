# Start TRAK-OS
Write-Host "Starting TRAK-OS..." -ForegroundColor Green

if (!(Test-Path "trakos.iso")) {
    Write-Host "ISO not found. Building first..." -ForegroundColor Yellow
    .\build.ps1
}

Write-Host ""
Write-Host "=== TRAK-OS ===" -ForegroundColor Cyan
Write-Host "✅ Minimal kernel" -ForegroundColor Green
Write-Host "✅ Direct hardware access" -ForegroundColor Green
Write-Host "✅ Assembly-based" -ForegroundColor Green
Write-Host "✅ Fast boot" -ForegroundColor Green
Write-Host ""
Write-Host "Controls:" -ForegroundColor Yellow
Write-Host "  Should display 'SUCCESS! KERNEL WORKS' when booted" -ForegroundColor White
Write-Host "  Press Ctrl+C to exit QEMU" -ForegroundColor White
Write-Host ""

# Try different emulators
if (Get-Command qemu-system-i386 -ErrorAction SilentlyContinue) {
    Write-Host "Starting with QEMU..." -ForegroundColor Blue
    qemu-system-i386 -cdrom trakos.iso -m 128
} elseif (Get-Command qemu-system-x86_64 -ErrorAction SilentlyContinue) {
    Write-Host "Starting with QEMU x86_64..." -ForegroundColor Blue
    qemu-system-x86_64 -cdrom trakos.iso -m 128
} else {
    Write-Host "QEMU not found. You can run the ISO file manually:" -ForegroundColor Yellow
    Write-Host "  - Use VirtualBox, VMware, or any VM software" -ForegroundColor White
    Write-Host "  - Boot from: trakos.iso" -ForegroundColor White
    Write-Host "  - Memory: 128MB or more" -ForegroundColor White
}