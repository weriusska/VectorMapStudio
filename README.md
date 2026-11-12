# 🗺️ VectorMap Studio

![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Qt](https://img.shields.io/badge/Qt-Framework-41CD52?style=for-the-badge&logo=qt&logoColor=white)
![Multithreading](https://img.shields.io/badge/Multithreading-std%3A%3Athread-FF6F00?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-blue?style=for-the-badge)

**VectorMap Studio** (formerly *ProjektRPGmaker*) is a custom, performance-oriented 2D tile-based map editor designed for retro RPGs. Built from scratch using **C++20** and the **Qt Framework** (`QGraphicsScene` / `QGraphicsView`), the engine focuses on deterministic memory management, thread-safe asynchronous I/O operations, and extensible object-oriented architecture.

---

## ✨ Key Engineering Features

* **Strict Memory Management (Zero Leaks):** Full lifecycle control of map tile objects using `std::unique_ptr` and custom move semantics, completely eliminating manual memory deallocation overhead and memory leaks.
* **Thread-Safe Asynchronous Autosave:** A dedicated background worker thread (`std::thread` + `std::atomic`) periodically serializes map state to disk every 30 seconds. Shared grid resources are protected via `std::mutex` and `std::lock_guard`, preventing race conditions during simultaneous real-time editing and file I/O.
* **C++20 Ranges & Data Aggregation:** Real-time map statistics (calculating proportions of natural terrain vs. infrastructure) powered by modern C++20 `std::ranges` and `std::views::filter` pipelines.
* **Polymorphic Tile System:** Object-oriented tile hierarchy (`ObiektMapy`) supporting interactive, stateful objects (e.g., toggleable doors responding to mouse double-click events with dynamic collision states).
* **Regex-Validated Custom Serialization:** Lightweight text-based map format parser utilizing `std::filesystem` and `std::regex` to enforce structural integrity and dimension validation upon loading.
* **Frameless Custom UI:** Custom-styled, draggable frameless Qt dialogs and main workspace featuring a retro pixel-art aesthetic.

---

## 🛠️ Tech Stack

* **Language:** C++20 (`<ranges>`, `<filesystem>`, `<thread>`, `<mutex>`, `<memory>`, `<regex>`, `<functional>`)
* **GUI & Rendering:** Qt Framework (`QtWidgets`, `QGraphicsScene`, `QGraphicsView`, `.qrc` virtual resource system)
* **Architecture:** Modular separation between Core Engine (`ZarzadcaMapy`), I/O Serialization (`SystemZapisu`), and GUI View layers.

---

## 🚀 Architecture & Refactoring Roadmap

As part of evolving **VectorMap Studio** toward commercial game-engine standards, the following architectural upgrades are actively being implemented:

| Area | Current Implementation | Target Architecture | Engineering Goal |
| :--- | :--- | :--- | :--- |
| **Spatial Memory Layout** | 2D Vector (`std::vector<std::vector<...>>`) | **Flattened 1D Vector** (`y * width + x`) | Eliminate memory fragmentation and reduce CPU cache misses during spatial queries. |
| **UI / Logic Decoupling** | Direct brush manipulation in `QGraphicsRectItem` | **Observer Pattern** (`std::function` callbacks) | Strict MVC separation; model mutations emit events to update the view. |
| **Asset Management** | Symbol-based `if/else` chains (`'#'`, `'~'`, `'.'`) | **Hash Map** (`std::unordered_map`) | $O(1)$ texture lookups cached in RAM instead of repeated `QPixmap` instantiations. |
| **Styling System** | Inline C++ raw string stylesheets | **External `.qss` Stylesheets** | Clean C++ codebase and hot-swappable UI themes via Qt Resource System (`.qrc`). |
| **Edge Blending** | Static single-tile sprites | **4-Bit Bitmask Autotiling** | Asynchronous neighbor querying (N, E, S, W) computing a `0–15` bitmask for seamless terrain transitions. |

---

## 📂 Project Structure

```text
VectorMap-Studio/
├── src/
│   ├── main.cpp                  # Application entry point & font initialization
│   ├── MenuGlowne.h              # Frameless main menu & project launcher
│   ├── ProjektRPGmaker.h/.cpp    # Main editor window, QGraphicsScene & brush controllers
│   ├── Zarzadca.h/.cpp           # Core map engine, multithreaded autosave & C++20 ranges
│   ├── Obiekt.h                  # Abstract base class for map tiles
│   ├── Kafelek.h                 # Derived tile classes (Grass, Wall, Water, Soil, Path, Door)
│   ├── SystemZapisu.h            # Regex-based file serialization and parsing
│   └── Dialogs/                  # Custom frameless modal dialogs (New, Load, Save, Stats)
├── assets/
│   ├── sprites/                  # Tile textures (.png)
│   ├── css/                      # Custom pixel font & UI backgrounds
│   └── zasoby.qrc                # Qt resource collection file
└── ZapisaneMapy/                 # Output directory for serialized map files (.txt)
