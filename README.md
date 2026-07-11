# Forge3D Engine

A 3D rendering engine written in C.

## Project Structure

Organize the project as follows:

```text
Forge3D/
│
├── src/
│   ├── main.c
│   ├── vector.c
│   ├── vector.h
│   ├── matrix.c
│   ├── matrix.h
│   ├── projection.c
│   ├── projection.h
│   ├── camera.c
│   └── camera.h
│
└── build/
```

Place all `.c` and `.h` source files inside the `src` directory.

## Requirements

Before compiling, make sure you have:

- GCC (MinGW or another GCC toolchain)
- SDL3 development libraries
- Math library (`libm`, included with GCC)

Ensure that the SDL3 headers and libraries are properly installed and accessible to GCC.

## Building

Open **Command Prompt** and navigate to the project root directory.

Compile the project with:

```bash
gcc src/main.c src/vector.c src/matrix.c src/projection.c src/camera.c -lSDL3 -lm -o build/forge3d.exe
```

> **Note:** Depending on your system, you may need to configure include paths and library paths for SDL3 if they are not already available to GCC.

## Running

After a successful build, run the executable with:

```bash
build\forge3d
```

or

```bash
build\forge3d.exe
```

## Controls

- **Left-click + drag**: Rotate camera around the scene (orbit controls)
- **Scroll wheel**: Zoom in/out
- **WASD/Q/E keys**: Legacy direct camera movement (kept for fallback)
- **ESC**: Exit the application

## Notes

- The `build` directory stores the compiled executable.
- All source (`.c`) and header (`.h`) files should remain inside the `src` directory.
- Make sure all required SDL3 dependencies are installed before compiling.
