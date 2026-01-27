# The Helios-Project (C++)

[![Windows](https://github.com/pernicius/helios-project/actions/workflows/Windows.yml/badge.svg)](https://github.com/pernicius/helios-project/actions/workflows/Windows.yml)
[![Linux](https://github.com/pernicius/helios-project/actions/workflows/Linux.yml/badge.svg)](https://github.com/pernicius/helios-project/actions/workflows/Linux.yml)
[![MacOS](https://github.com/pernicius/helios-project/actions/workflows/MacOS.yml/badge.svg)](https://github.com/pernicius/helios-project/actions/workflows/MacOS.yml)

The Helios-Project is a long-term project to create a modern, cross-platform 3D engine in C++20. It is focused on clean, modular architecture, high performance, and the implementation of modern rendering techniques, with Vulkan as the primary graphics API.

## Core Features & Architecture

The engine is being built from the ground up with a focus on flexibility and extensibility. Key architectural features include:

- **Modern C++20 Design:** Leverages modern C++ features for clean, safe, and performant code, including RAII principles for resource management.
- **Multi-API Abstraction Layer:** A clean `Renderer` interface decouples the application logic from the underlying graphics API, enabling support for Vulkan, DirectX, and Metal.
- **Platform-Agnostic Design:** Core systems like windowing and input are abstracted to seamlessly support Windows, Linux, and macOS.
- **Advanced Build System:** Uses **[Premake](https://premake.github.io/)** to generate platform-specific project files and automates dependency setup ([Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows), [GLFW](https://www.glfw.org/), etc.) via scripts.
- **Hierarchical Configuration System:** A powerful, layered system (`Default`, `Platform`, `Project`, `User`, `Runtime`) for managing engine and project settings.
- **Integrated Virtual File System (VFS):** Abstracts file access, allowing for flexible asset loading from various sources.

## Project Status

> [!CAUTION]
> This project is in an early and heavy development phase. The API is unstable, and it cannot be guaranteed that the code will compile or run without errors at all times.

## Getting Started

While the project is designed to be cross-platform, the primary development focus is currently on **Windows** using **Visual Studio**.

### 1. Prerequisites

- **Git:** Required to clone the repository and its submodules.
- **Visual Studio:** With the "Desktop development with C++" workload.
- **Windows 10/11:** With the latest updates.

### 2. Setup (Windows)

1.  **Clone the Repository:**
    ```bash
    git clone --recursive https://github.com/pernicius/helios-project.git
    cd helios-project
    ```
    *Note: The `--recursive` flag is optional to download all Git submodules (dependencies) immediately.*

2.  **Run the Setup Script:**
    Execute the `scripts\windows\setup.bat` batch file. This script automatically handles all necessary steps:
    - **Git Submodules:** Downloads, initializes and updates all Git submodules.
    - **[Premake](https://premake.github.io/):** Downloads the workspace generator.
    - **[Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows):** Checks if the SDK is installed. If not, it downloads the official installer and starts it. Please follow the installer's instructions.
    - **Visual Studio Solution:** Generates the `Helios-Project.sln` file in the root directory.

3.  **Open the Project:**
    Open the generated `Helios-Project.sln` in Visual Studio 2022 and compile the project.

## Roadmap & Goals

Development follows an "engine-first" approach, where core systems and the rendering infrastructure are implemented first.

- **Platform Support:**
  - [x] Windows (Primary Target)
  - [ ] Linux
  - [ ] macOS
- **Graphics APIs:**
  - [ ] Vulkan (Primary Target)
  - [ ] DirectX 12
  - [ ] Metal
- **Rendering Features:**
  - [ ] Physically Based Rendering (PBR)
  - [ ] Real-Time Shadows
  - [ ] HDR and Tonemapping
  - [ ] Post-Processing Effects
- **Engine Systems:**
  - [x] Logging System
  - [x] Abstracted Windowing and Input
  - [x] Hierarchical Configuration System
  - [x] Virtual File System (VFS)
  - [ ] Asset Management
  - [ ] Entity Component System (ECS)
