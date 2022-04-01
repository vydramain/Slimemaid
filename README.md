# Slimemaid
This is graduation project

# Description
This version wrote on Linux and adapted only for this platform. Try it on MacOS and Windows at your own risk.

# Instuctions

## Install instruction
List of dependencies:
 - [gcc (GCC) 11.2](https://gcc.gnu.org/gcc-11/)
 - [GNU Make 4.3](https://www.gnu.org/software/make/)
 - [GLFW 3.3.6](https://www.glfw.org/)
 - [GLM 0.9.9.9](https://github.com/g-truc/glm)
 - [VulkanSDK 1.3.204.1](https://vulkan.lunarg.com/sdk/home)

### Conventions

* `#` means that given Linux command **must be executed with** superuser privileges.
* `$` means that given Linux command **can be executed without** superuser privileges.

### Before start
#### Vulkan packages
The most important components you'll need for developing Vulkan applications on Linux are the Vulkan loader, validation layers, and a couple of command-line utilities to test whether your machine is Vulkan-capable:

Command-line utilities, most importantly vulkaninfo and vkcube. Run these to confirm your machine supports Vulkan.

```console
# sudo apt install vulkan-tools
```
or
```console
# sudo dnf install vulkan-tools
```

Installs Vulkan loader. The loader looks up the functions in the driver at runtime, similarly to GLEW for OpenGL - if you're familiar with that.

```console
# sudo apt install libvulkan-dev
```
or
```console
# sudo dnf install vulkan-loader-devel
```

Installs the standard validation layers and required SPIR-V tools. These are crucial when debugging Vulkan applications, and we'll discuss them in the upcoming chapter.

```console    
# sudo apt install vulkan-validationlayers-dev spirv-tools
```
or
```console    
# sudo dnf install mesa-vulkan-devel vulkan-validation-layers-devel
```

#### GLFW
We'll be installing [GLFW](https://www.glfw.org/) from the following command:

```console
# sudo apt install libglfw3-dev
```
or
```console
# sudo dnf install glfw-devel
```

#### GLM
Vulkan does not include a library for linear algebra operations, so we'll have to download one. GLM is a nice library that is designed for use with graphics APIs and is also commonly used with OpenGL.

It is a header-only library that can be installed from the libglm-dev or glm-devel package:

```console
# sudo apt install libglm-dev
```
or
```console
# sudo dnf install glm-devel
```

#### VulkanSDK
I would recommend you to download [SDK Tarball](https://sdk.lunarg.com/sdk/download/1.3.204.1/linux/vulkansdk-linux-x86_64-1.3.204.1.tar.gz) version. In the project absolute ways used for the compiling. And correct system position is necessary for correct work.
But if you don't want to use you should change base SDK location variables.

After downloading use this command to unpack SDK where `x.x.x.x` is version of current SDK and `arc` is architecture of your system:

```console
$ tar -zxvf vulkansdk-linux-arc-x.x.x.x.tar.gz
```

After that move file to correct location:

```console
# mv x.x.x.x /opt/vulkansdk/x.x.x.x
```

And do not forget execute shell file to set up system environment variables:

```console
# ch /opt/vulkansdk/x.x.x.x
# sudo chmod +x setup-env.sh
# ./setup-env.sh
```

### Footnote
If you have trouble with compiling and launching project check is your [development environment](https://vulkan-tutorial.com/en/Development_environment#page_Linux) set up.
Some of this packages for RPM-based distributions can be deprecated or have different names. Check [pkgs.org](https://pkgs.org).

# Special info
Additional information about tasks or some important info about application is [here](./NOTES.rm).

 - [Vulkan swapchain examples with description by KhronosGroup](https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#swapchain-image-acquire-and-present)
 - [Information about the layout qualifier in the OpenGL wiki](https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL))
