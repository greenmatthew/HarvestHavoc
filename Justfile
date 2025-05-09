# Set shell for Windows
set windows-shell := ["powershell.exe", "-NoLogo", "-Command"]

# Default recipe to run when just is called without arguments
default: help

# Show available commands
help:
    just --list

# Generate Visual Studio solution
build-solution:
    if (!(Test-Path build)) { New-Item -ItemType Directory -Path build -Force }
    cmake -S . -B build -G "Visual Studio 17 2022"

# Build debug configuration
build: build-solution
    cmake --build build --config Debug

# Build release configuration
build-release: build-solution
    cmake --build build --config Release

# Run the application in debug mode
run: build
    ./bin/debug/HarvestHavoc.exe

# Run the application in release mode
run-release: build-release
    ./bin/release/HarvestHavoc.exe

# Clean binary output directory
clean:
    if (Test-Path bin) { Remove-Item -Recurse -Force bin }

# Clean all build artifacts
clean-all: clean
    if (Test-Path build) { Remove-Item -Recurse -Force build }