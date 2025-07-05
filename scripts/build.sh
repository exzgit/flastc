#!/bin/bash

# flast Compiler Build Script
# This script builds the flast compiler and adds it to the system PATH

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

print_status "Building flast Compiler..."
print_status "Project root: $PROJECT_ROOT"

# Check if we're in the right directory
if [ ! -f "$PROJECT_ROOT/CMakeLists.txt" ]; then
    print_error "CMakeLists.txt not found. Please run this script from the flast project directory."
    exit 1
fi

# Check for required dependencies
print_status "Checking dependencies..."

# Check for CMake
if ! command -v cmake &> /dev/null; then
    print_error "CMake is not installed. Please install CMake first."
    exit 1
fi

# Check for LLVM
if ! command -v llvm-config &> /dev/null; then
    print_error "LLVM is not installed or not in PATH. Please install LLVM first."
    exit 1
fi

# Check for C++ compiler
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    print_error "No C++ compiler found. Please install g++ or clang++."
    exit 1
fi

print_success "All dependencies found!"

# Create build directory
BUILD_DIR="$PROJECT_ROOT/build"
print_status "Creating build directory: $BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Navigate to build directory
cd "$BUILD_DIR"

# Configure with CMake
print_status "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
print_status "Building flast compiler..."
make -j$(nproc)

# Check if build was successful
if [ ! -f "flast" ]; then
    print_error "Build failed! flast executable not found."
    exit 1
fi

print_success "Build completed successfully!"

# Make the executable executable (in case it wasn't)
chmod +x flast

# Test the compiler
print_status "Testing the compiler..."
if ./flast --help &> /dev/null || ./flast -h &> /dev/null; then
    print_success "Compiler test passed!"
else
    print_warning "Could not test compiler help output, but executable exists."
fi

# Create user local bin directory if it doesn't exist
USER_BIN_DIR="$HOME/.local/bin"
print_status "Installing flast to $USER_BIN_DIR..."
mkdir -p "$USER_BIN_DIR"

# Copy to user local bin
if [ -e "$USER_BIN_DIR/flast" ] || [ -L "$USER_BIN_DIR/flast" ]; then
    print_status "Removing old flast from $USER_BIN_DIR..."
    rm -f "$USER_BIN_DIR/flast"
fi
cp flast "$USER_BIN_DIR/"
chmod +x "$USER_BIN_DIR/flast"

# Check if ~/.local/bin is in PATH
if [[ ":$PATH:" != *":$USER_BIN_DIR:"* ]]; then
    print_warning "$USER_BIN_DIR is not in your PATH."
    print_status "Adding to PATH for current session..."
    export PATH="$PATH:$USER_BIN_DIR"
    
    # Add to shell profile
    SHELL_PROFILE=""
    if [ -f "$HOME/.bashrc" ]; then
        SHELL_PROFILE="$HOME/.bashrc"
    elif [ -f "$HOME/.bash_profile" ]; then
        SHELL_PROFILE="$HOME/.bash_profile"
    elif [ -f "$HOME/.zshrc" ]; then
        SHELL_PROFILE="$HOME/.zshrc"
    fi
    
    if [ -n "$SHELL_PROFILE" ]; then
        if ! grep -q "$USER_BIN_DIR" "$SHELL_PROFILE"; then
            echo "" >> "$SHELL_PROFILE"
            echo "# Add local bin to PATH for flast compiler" >> "$SHELL_PROFILE"
            echo "export PATH=\"\$PATH:$USER_BIN_DIR\"" >> "$SHELL_PROFILE"
            print_status "Added $USER_BIN_DIR to PATH in $SHELL_PROFILE"
            print_warning "Please restart your terminal or run: source $SHELL_PROFILE"
        fi
    fi
fi

# Verify installation
if command -v flast &> /dev/null; then
    print_success "flast is now available globally!"
    print_status "You can now use 'flast' from anywhere in your system."
    
    # Show version info if available
    if flast --version &> /dev/null; then
        flast --version
    elif flast -v &> /dev/null; then
        flast -v
    fi
else
    print_warning "flast was installed but may not be in PATH. Please restart your terminal."
fi

print_success "Build and installation completed!"
print_status "You can now use:"
print_status "  - 'flast' (global installation from ~/.local/bin)"
print_status "  - '$BUILD_DIR/flast' (from build directory)"

print_success "🎉 flast compiler is ready to use!"
