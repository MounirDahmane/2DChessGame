# E2-E4 Chess Engine

A high-performance, fully featured 2D chess engine built with C++ and SFML.
Designed with a modular architecture, asynchronous AI, smooth animations, and a complete set of gameplay tools for a polished chess experience.

<p align="center">
  <img src="docs/gameplay.gif" alt="Gameplay Demo" width="480" />
</p>

## Features

### Core Engine

* Full chess rules support, including castling, en passant, and pawn promotion.
* Advanced draw detection with stalemate, the 50-move rule, and threefold repetition.
* Minimax AI with alpha-beta pruning, positional evaluation tables, and adjustable difficulty.
* Multithreaded AI execution to keep the UI responsive and avoid freezing during computation.

### Gameplay and UI

* Drag-and-drop movement with traditional click-to-move support.
* Smooth piece sliding animations and screen shake effects for captures, checks, and checkmates.
* Dual-sidebar layout with match timers, material score, captured pieces, and live move history.
* Clear visual and audio feedback, including:

  * Red highlight when the king is in check.
  * Move hints for quiet moves and captures.
  * Turn indicators and an “AI is thinking...” status display.
  * Sound effects for moves, captures, and checks.

### Tools and Customization

* Time controls with 1, 3, 5, and 10 minute modes, plus optional +5s increment.
* Multiple board themes, including Classic, Green, and Blue.
* Undo move support for quick correction and analysis.
* PGN export for saving and reviewing games externally.
* Main menu for local PvP, playing against the computer, and managing settings.

## Project Structure

The codebase is organized around a modular, refactored design for maintainability and clarity:

```text
include/Constants.hpp   — Centralized configuration for colors, UI sizing, and piece values
include/ChessBoard.hpp  — Class definitions, async threading, and game state management
src/ChessBoard.cpp      — UI rendering, AI execution, animations, and input handling
src/MoveLogic.cpp       — Move validation, check simulation, and pathfinding logic
src/main.cpp            — Application entry point and main game loop
```

## Architecture

The project separates responsibilities into focused components:

* **Rendering and input** are handled independently from move validation.
* **Game logic** is isolated from the user interface.
* **AI computation** runs asynchronously to preserve smooth gameplay.
* **Shared constants and configuration** are centralized to simplify maintenance and tuning.

## Highlights

* Clean and scalable C++ design
* Responsive SFML-based 2D interface
* Professional chess mechanics
* Background AI processing
* Smooth gameplay presentation
