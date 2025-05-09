# Slimemaid

A lightweight game engine and slime simulation built with Vulkan and ECS.

***

## Overview

Slimemaid is an experimental game engine and slime simulation leveraging
the Entity-Component-System (ECS) pattern for efficient logic management and
a direct Vulkan renderer for high-performance graphics. This project is
designed for Linux systems, with plans for future Android support.

## Features

- ECS Architecture: Modular and scalable game logic inspired by modern engine design.
- Vulkan Renderer: Fast, low-level graphics rendering for optimal performance.
- Slime Simulation: Unique physics-based simulation for interactive slime behavior.
- Open-Source Libraries: Built with GLFW, GLM, spdlog, and more for robust functionality.

## Instuctions

### Prerequisites

Before installing, ensure you have the following dependencies:

- [gcc (GCC) 11.2](https://gcc.gnu.org/gcc-11/)
- [GNU Make 4.3](https://www.gnu.org/software/make/)
- [CMake 3.22.2](https://cmake.org/)
- [VulkanSDK 1.3.204.1](https://vulkan.lunarg.com/sdk/home)
- [GLFW 3.3.6](https://www.glfw.org/)
- [GLM 0.9.9.9](https://github.com/g-truc/glm)
- [spdlog 1.10.0](https://github.com/gabime/spdlog)
- [stb collection](https://github.com/nothings/stb)
- [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)

## Installation

Follow these steps to set up Slimemaid on your Linux system.

### Before start

#### Vulkan packages

The most important components you'll need for developing Vulkan applications
on Linux are the Vulkan loader, validation layers, and a couple of command-line
utilities to test whether your machine is Vulkan-capable:

##### Vulkan tools

Command-line utilities, most importantly vulkaninfo and vkcube.
Run these to confirm your machine supports Vulkan.

If you use apt package manager:

```console
sudo apt install vulkan-tools
```

If you use dnf package manager:

```console
sudo dnf install vulkan-tools
```

If you use yay wrapper:

```console
yay vulkan-tools
```

##### Vulkan library for develop

Installs Vulkan loader. The loader looks up the functions in the driver
at runtime, similarly to GLEW for OpenGL - if you're familiar with that.

If you use apt package manager:

```console
sudo apt install libvulkan-dev
```

If you use dnf package manager:

```console
sudo dnf install vulkan-loader-devel
```

##### Vulkan validation layers

If you use apt package manager:

```console
sudo apt install vulkan-validationlayers
```

If you use dnf package manager:

```console
sudo dnf install vulkan-validation-layers
```

If you use yay wrapper:

```console
yay vulkan-validation
```

Additionaly, if you use Steam Deck with SteamOS try:

```console
yay libglvnd
```

##### VulkanSDK

I would recommend you to download [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#linux).
In the project absolute ways used for the compiling. And correct system position
is necessary for correct work. But if you don't want to use you should change
base SDK location variables.

After downloading use this command to unpack SDK where `x.x.x.x` is version
of current SDK and `arc` is architecture of your system:

```console
tar -xvf vulkansdk-linux-arc-x.x.x.x.tar.gz
```

After that move file to correct location:

```console
mv x.x.x.x /opt/vulkansdk/x.x.x.x
```

And do not forget execute shell file to set up system environment variables:

```console
cd /opt/vulkansdk/x.x.x.x
sudo chmod +x setup-env.sh
./setup-env.sh
```

#### SPIR-V

Installs the standard validation layers and required SPIR-V tools.
These are crucial when debugging Vulkan applications, and we'll discuss them
in the upcoming chapter.

If you use apt package manager:

```console
sudo apt install vulkan-validationlayers-dev spirv-tools
```

If you use dnf package manager:

```console
sudo dnf install mesa-vulkan-devel vulkan-validation-layers-devel
```

#### GLFW

We'll be installing [GLFW](https://www.glfw.org/) from the following command:

If you use apt package manager:

```console
sudo apt install libglfw3-dev
```

If you use dnf package manager:

```console
sudo dnf install glfw-devel
```

If you use yay wrapper:

```console
yay glfw
```

#### GLM

Vulkan does not include a library for linear algebra operations,
so we'll have to download one. GLM is a nice library that is designed
for use with graphics APIs and is also commonly used with OpenGL.

It is a header-only library that can be installed from the libglm-dev
or glm-devel package:

If you use apt package manager:

```console
sudo apt install libglm-dev
```

If you use dnf package manager:

```console
sudo dnf install glm-devel
```

If you use yay wrapper:

```console
yay glm
```

#### SPDLOG

We use simple, very fast, header-only/compiled, C++ logging library.

If you use apt package manager:

```console
sudo apt install libspdlog-dev
```

If you use dnf package manager:

```console
sudo dnf install spdlog
```

#### libXi

The libXi library is part of the X11 development libraries, specifically for
the X Input extension.

If you use apt package manager:

```console
sudo apt install libxi-dev
```

If you use dnf package manager:

```console
sudo dnf install libXi-devel
```

If you use yay wrapper:

```console
yay libxi
```

---

### Troubleshooting

- **Compilation Issues**: Ensure your [development environment](https://vulkan-tutorial.com/en/Development_environment#page_Linux) is correctly set up.
- **Vulkan Environment**: If you have issues with setting up your vulkan environment check [getting started](https://vulkan.lunarg.com/doc/sdk/1.4.309.0/linux/getting_started.html).
- **Package Names**: Some packages may have different names or be deprecated on RPM-based distributions. Check [pkgs.org](https://pkgs.org) or [rpmfind.net](https://rpmfind.net/).
- **Custom Builds**: If a package is unavailable, compile it manually and package it as an [RPM](https://rpm-packaging-guide.github.io/).

## Special info

Additional information about tasks or some important info about application is [here](./NOTES.md).

- [Vulkan swapchain examples with description by KhronosGroup](https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#swapchain-image-acquire-and-present)
- [Information about the layout qualifier in the OpenGL wiki](https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL))
- [Matrices and operations with them by OpenGL Tutorial](https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/)
- [Vulkan expects the data in your structure to be aligned in memory in a specific way](https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/chap15.html#interfaces-resources-layout)
- [Vulkan also allows to copy pixels from a `VkBuffer` to an image and the API for this is actually faster on some hardware](https://developer.nvidia.com/vulkan-memory-management)
- [Nvidia's book: GPUGems](https://developer.nvidia.com/gpugems/gpugems/contributors)
- An easy way to find such models is to look for 3D scans on [Sketchfab](https://sketchfab.com/).
