# E2-E4 Chess Engine

A high-performance, modular 2D Chess Game built with C++ and SFML. This project focuses on clean software architecture, separating the graphical user interface from the core movement and validation logic.

## 🚀 Features

### Core Gameplay
- **Full Rule Set:** Includes standard moves, Castling, and En Passant.
- **Movement Engine:** Validates moves in real-time, preventing players from entering or remaining in Check.
- **Game States:** Main Menu with 1vs1 mode and a dedicated game over state.
- **Pawn Promotion:** Automatic promotion to Queen upon reaching the final rank.

### Advanced UI/UX
- **Symmetric Sidebar:** A dual-panel interface tracking timers, material score, and captured pieces for both players.
- **Visual Cues:**
  - **Turn Indicator:** A vibrant Cyan bar highlights the active player.
  - **Dynamic Hints:** Black circles for quiet moves, Red circles for captures.
  - **Check Alert:** Red inner-glow on the board when a King is under attack.
  - **Winner Glow:** The victorious player's timer and name glow in Gold.

## 📂 Project Structure

The project follows a refactored, modular design for better maintainability:

```

include/Constants.hpp   — Centralized configuration (Colors, UI sizing, Piece values)
include/ChessBoard.hpp  — Class definitions and state management
src/ChessBoard.cpp      — UI rendering, input handling, and sidebar management
src/MoveLogic.cpp       — The "Brain": move validation, check simulation, and pathfinding
src/main.cpp            — Application entry point and Game State Machine

````

## 🛠️ Prerequisites

To build this project, you need the **SFML 2.6.1** development libraries installed on your system.

**On Ubuntu/Debian:**

```bash
sudo apt update
sudo apt install libsfml-dev
````

## 🏗️ Building and Running

Clone the repository:

```bash
git clone https://github.com/yourusername/2DChessGame.git
cd 2DChessGame
```

Run the provided build script:

```bash
chmod +x run.sh
./run.sh
```

**Note:** The `build/` directory is ignored by Git to keep the repository clean and portable.

## 🧹 Code Quality

The project follows the **Allman brace style** and is strictly formatted using **clang-format**.

To format the entire project, run:

```bash
find . -type f -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i -style=file
```

## 🗺️ Roadmap

* [ ] Pawn Promotion Menu: Manual selection of Queen, Knight, Rook, or Bishop
* [ ] PGN Move Log: A scrollable history of moves in the sidebar
* [ ] AI Integration: Minimax algorithm with Alpha-Beta pruning
* [ ] Piece Animations: Linear interpolation for smooth piece movement

---

**Tech Stack:** C++, SFML, CMake, Clang-Format