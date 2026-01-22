# Alternative start script for TRAK-OS when QEMU is not available
Write-Host "TRAK-OS Alternative Launch" -ForegroundColor Green
Write-Host "=========================" -ForegroundColor Cyan

# Check if ISO exists
if (!(Test-Path "trakos.iso")) {
    Write-Host "ISO not found. Building first..." -ForegroundColor Yellow
    .\build.ps1
}

$isoSize = (Get-Item "trakos.iso").Length
Write-Host "ISO File: trakos.iso ($([math]::Round($isoSize/1MB, 2)) MB)" -ForegroundColor White

Write-Host ""
Write-Host "Since QEMU is not available, here are alternatives:" -ForegroundColor Yellow
Write-Host ""

Write-Host "Option 1: VirtualBox (Recommended)" -ForegroundColor Cyan
Write-Host "  1. Download VirtualBox from https://www.virtualbox.org/" -ForegroundColor White
Write-Host "  2. Create a new VM (Type: Other, Version: Other/Unknown)" -ForegroundColor White
Write-Host "  3. RAM: 512MB, No hard disk needed" -ForegroundColor White
Write-Host "  4. Settings > Storage > Add optical drive > Choose trakos.iso" -ForegroundColor White
Write-Host "  5. Start the VM" -ForegroundColor White
Write-Host ""

Write-Host "Option 2: VMware" -ForegroundColor Cyan
Write-Host "  1. Create new VM, boot from ISO: trakos.iso" -ForegroundColor White
Write-Host "  2. RAM: 512MB minimum" -ForegroundColor White
Write-Host ""

Write-Host "Option 3: Hyper-V (Windows Pro/Enterprise)" -ForegroundColor Cyan
Write-Host "  1. Enable Hyper-V feature in Windows" -ForegroundColor White
Write-Host "  2. Create Generation 1 VM" -ForegroundColor White
Write-Host "  3. Mount trakos.iso as DVD" -ForegroundColor White
Write-Host ""

Write-Host "Option 4: Boot from USB (Real Hardware)" -ForegroundColor Cyan
Write-Host "  1. Use Rufus or similar to create bootable USB from trakos.iso" -ForegroundColor White
Write-Host "  2. Boot computer from USB" -ForegroundColor White
Write-Host "  WARNING: Only do this if you know what you're doing!" -ForegroundColor Red
Write-Host ""

Write-Host "Expected Behavior:" -ForegroundColor Green
Write-Host "  - Blue header 'TRAKOS v1.0 - INTERACTIVE MODE'" -ForegroundColor White
Write-Host "  - Green [ OK ] messages for each driver" -ForegroundColor White
Write-Host "  - Shell prompt 'trakos> ' ready for commands" -ForegroundColor White
Write-Host "  - Type 'help' to see available commands" -ForegroundColor White
Write-Host ""

Write-Host "Known Commands:" -ForegroundColor Yellow
Write-Host "  help, clear, echo, version, date, time, calc, memory, colors" -ForegroundColor White

# Try to open the folder containing the ISO
try {
    Start-Process explorer.exe -ArgumentList (Get-Location).Path
    Write-Host "Opened current folder in Explorer" -ForegroundColor Green
} catch {
    Write-Host "Could not open Explorer" -ForegroundColor Red
}