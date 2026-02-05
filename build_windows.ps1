# Build and run raycaster on native Windows
# Requires: Visual Studio 2022 (or Build Tools) with C++ workload

$ErrorActionPreference = "Stop"
$ProjectRoot = $PSScriptRoot

# 1. Ensure CMake is available
$CmakePath = "C:\Program Files\CMake\bin\cmake.exe"
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    if (Test-Path $CmakePath) {
        $env:Path = "C:\Program Files\CMake\bin;" + $env:Path
    } else {
        Write-Host "Installing CMake via winget..."
        winget install Kitware.CMake --accept-package-agreements --accept-source-agreements
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "User")
    }
}

# 2. Set up vcpkg
$VcpkgRoot = Join-Path $ProjectRoot "vcpkg"
if (-not (Test-Path $VcpkgRoot)) {
    Write-Host "Cloning vcpkg..."
    git clone https://github.com/microsoft/vcpkg $VcpkgRoot
}
$BootstrapScript = Join-Path $VcpkgRoot "bootstrap-vcpkg.bat"
if (-not (Test-Path (Join-Path $VcpkgRoot "vcpkg.exe"))) {
    Write-Host "Bootstrapping vcpkg..."
    Push-Location $VcpkgRoot
    & $BootstrapScript
    Pop-Location
}
$VcpkgExe = Join-Path $VcpkgRoot "vcpkg.exe"
$ToolchainFile = (Resolve-Path (Join-Path $VcpkgRoot "scripts\buildsystems\vcpkg.cmake")).Path

# 3. Configure and build
$BuildDir = Join-Path $ProjectRoot "build"
if (-not (Test-Path $BuildDir)) { New-Item -ItemType Directory -Path $BuildDir | Out-Null }

Write-Host "Configuring with CMake..."
Push-Location $BuildDir
& cmake .. "-DCMAKE_TOOLCHAIN_FILE=$ToolchainFile" -A x64
if ($LASTEXITCODE -ne 0) { Pop-Location; exit 1 }

Write-Host "Building..."
cmake --build . --config Release
if ($LASTEXITCODE -ne 0) { Pop-Location; exit 1 }
Pop-Location

# 4. Copy SDL2.dll (vcpkg installs to build dir, but ensure it's next to exe)
$ExeDir = Join-Path $BuildDir "Release"
$SdlDll = Join-Path $VcpkgRoot "installed" "x64-windows" "bin" "SDL2.dll"
if (Test-Path $SdlDll) {
    Copy-Item $SdlDll -Destination $ExeDir -Force
}

# 5. Run the game
$ExePath = Join-Path $ExeDir "raycaster.exe"
Write-Host "Launching game: $ExePath"
& $ExePath
