# The Helios Project (C++)

[![Windows](https://github.com/pernicius/helios-project/actions/workflows/Windows.yml/badge.svg)](https://github.com/pernicius/helios-project/actions/workflows/Windows.yml)
[![Linux](https://github.com/pernicius/helios-project/actions/workflows/Linux.yml/badge.svg)](https://github.com/pernicius/helios-project/actions/workflows/Linux.yml)
[![MacOS](https://github.com/pernicius/helios-project/actions/workflows/MacOS.yml/badge.svg)](https://github.com/pernicius/helios-project/actions/workflows/MacOS.yml)
<!--
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/11306/badge)](https://www.bestpractices.dev/projects/11306)
-->

The ultimate goal is to create the foundation of a high-performance 3D engine written in modern C++, using Vulkan as the default graphics API.
It is a long-term project focused on performance, clean architecture, and modern rendering techniques.
> [!CAUTION]
> At the moment the project is under heavy development and it is possible that it will in no way work/compile.

***

## Getting Started
Personally, I prefer Visual Studio; other development environments are officially untested. Although the project aims to support other operating systems, the primary focus at the moment is on Windows.

<ins>**1. Download:**</ins>

- Start by cloning the repository with:<br />
  `git clone https://github.com/pernicius/helios-project.git`

<ins>**2. Configure the dependencies and generate workspace:**</ins>

- **Windows / Visual Studio**

  - Start the `scripts/windows/setup.bat` batch file. Which will then download and install all the required prerequisites for the projekt if they are not present yet.
    - **[Premake](https://premake.github.io/):** Will be downloaded. Premake is the main workspace generator for this project.
    - **[Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows):** If not present the script will download it and execute the installer.
    - **Git Submodules:** All submodules will be pulled.
    - Start of the *generate.bat* batch file is done automatically.
    
	*Note: If changes are made, or if you want to regenerate the project files, just rerun the* `scripts/windows/generate.bat` *batch file.*

- **Linux**

    *TODO: will be supported in the future*

- **MacOS**

    *TODO: will be supported in the future*

***

# Goals

- **Maximum performance:** Efficient use of CPU and GPU resources to deliver high frame rates even in complex scenes.
- **Modern rendering:** Implementation of advanced features such as:
  - Physically Based Rendering (PBR)
  - Real-time shadows
  - HDR and tonemapping
  - Post-processing effects
- **Modular design:** A clean and extensible codebase to support long-term growth.
- **Engine-first approach:** Core systems and rendering prioritized before tooling or editor integration.

# Development Status

## Work In Progress

  - [ ] Renderer Classes<br />
    ~~Based on [NVRHI](https://github.com/NVIDIA-RTX/NVRHI) to support Vulkan, DirectX11 and DirectX12<br />~~
	~~Alternatively i'll have a look into [SDL3](https://github.com/libsdl-org/SDL) which also supports OpenGL and Metal~~
  - [ ] Shader Compiler
  - [ ] Assets Class
 
 ## Platforms

  - [ ] Windows
  - [ ] Linux (not now but soon)
  - [ ] MacOS (not by me but the build/code is prepared)

## Render API's

 - [ ] Vulkan (default target for all platforms)
 - [ ] DirectX (target for Windows only)
 - [ ] Metal (eventually? if so, much later! -- target for Apple only)
 - [ ] OpenGL (eventually? if so, much later! -- fallback target if Vulkan doesn't work/exists)

 ## Later

  - [ ] App/Game Template(s)
  - [ ] Custom window caption
 
 And much more...
