# Wrapper to launch the build menu batch from CLion or any external tool
# Usage: powershell -ExecutionPolicy Bypass -File "Automation\run_build_menu.ps1"
# It will start a new PowerShell window which runs build_menu.bat and stays open.
$script = Join-Path -Path $PSScriptRoot -ChildPath "build_menu.bat"
if (-not (Test-Path $script)) {
    Write-Error "build_menu.bat not found at $script"
    exit 1
}
# Start a new PowerShell window and run the batch file, keeping the window open
Start-Process -FilePath "powershell.exe" -ArgumentList '-NoExit', '-ExecutionPolicy', 'Bypass', '-Command', "& '$script'" -WindowStyle Normal

