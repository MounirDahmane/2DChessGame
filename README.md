# E2-E4 Chess Engine

A high-performance, modular 2D chess engine built with **C++17** and **SFML**.  
This project features a strictly decoupled architecture, asynchronous multithreading for AI computation, and a polished UI with dynamic animations.

<p align="center">
  <img src="docs/gameplay.gif" alt="E2-E4 Gameplay" width="600" />
</p>

## 🚀 Key Features

### 🧠 Core Engine & AI
- **Complete Chess Rules:** Standard moves, castling, en passant, and pawn promotion.
- **Asynchronous AI:** A Minimax AI with alpha-beta pruning that runs on background threads to prevent UI lag.
- **Advanced Logic:** Detection for stalemate, the 50-move rule, and threefold repetition.

### 🎨 Visuals & UX
- **Smooth Animations:** Linear interpolation for piece movement and screen-shake effects.
- **Interactive UI:** Drag-and-drop support, real-time move hints, and a dual sidebar tracking match history and timers.
- **Audio Feedback:** Custom sound triggers for moves, captures, and checks.

---

## 📂 Repository Structure

The project is organized into focused directories for a clean separation of assets, logic, and build scripts:

```text
.
├── assets/             # Game resources (images, fonts, sounds)
├── docs/               # Documentation and gameplay previews
├── include/            # Header files (.hpp)
├── src/                # Source implementations (.cpp)
├── thirdparty/         # Local SFML libraries and external dependencies
├── CMakeLists.txt      # Primary build configuration
├── config.sh           # Utility for setting up the environment
├── format.sh           # Script to trigger clang-format across the repo
└── run.sh              # One-click script to compile and launch the game
````

---

## 🛠️ Installation & Build

### Prerequisites

Ensure you have the **SFML 2.6.1** development libraries installed.

**On Ubuntu/Debian:**

```bash
sudo apt update
sudo apt install libsfml-dev
```

### Fast-Track Setup

Use the helper scripts to manage the build process:

1. **Configure** the build directory:

```bash
./config.sh
```

2. **Build & Run** the engine:

```bash
chmod +x run.sh
./run.sh
```

---

## 🧹 Developer Tools

### Automatic Formatting

To maintain the **Allman brace style** and consistent indentation across the codebase, use the formatting script:

```bash
./format.sh
```

*Requires `clang-format` to be installed.*

### Build System

The project uses **CMake 3.15+**. It is configured to automatically detect all source files in `src/` recursively and sync assets to the build folder after compilation.

---

## 🧰 Tech Stack

**C++17**, **SFML 2.6.1**, **CMake**, **Clang-Format**, **Asynchronous Threading**
