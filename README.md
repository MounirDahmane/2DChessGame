# Chess Game

A 2D chess game built with C++ and SFML.

## Dependencies

- **SFML 3.0.0 (statically linked)**  
  The environment variable `SFML_ROOT` is set to the root folder of the SFML installation.
  
- **MSYS2 with MinGW (g++)**  
  Make sure you have MSYS2 installed and are using the MinGW toolchain (e.g., `C:/msys64/ucrt64/bin/g++.exe`).

## Environment Setup

1. **Set the SFML_ROOT Variable:**  

## How to Build

1. Open the project in VS Code.
2. Press <kbd>Ctrl+Shift+B</kbd> to build using the provided `tasks.json`.
   - This file is set up to compile all `.cpp` files from `src/` and link with SFML libraries (using the static versions).
   
## How to Run

- Once the build completes, run `build/Chess.exe` from the terminal or using your preferred method.