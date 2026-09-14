# Forge3D Engine

A software 3D renderer written in C with OBJ file loading and simple lighting.

## Project Structure

```
Forge3D/
├── src/
│   ├── main.c
│   ├── mesh.c/.h
│   ├── mesh_loader.c/.h
│   ├── material.c/.h
│   ├── lighting.c/.h
│   ├── vector.c/.h
│   ├── matrix.c/.h
│   ├── projection.c/.h
│   └── camera.c/.h
├── build/
│   ├── forge3d
│   ├── forge3d.exe
├── models/
├── textures/
```

## Requirements

- **GCC** compiler
- **SDL3** development libraries  
- **Math library** (libm)
- An **OBJ file** to render

## How to Build and Run

### 1. Get an OBJ File

Place your OBJ file in the project root as `model.obj`, or edit the path in `src/main.c` (line ~97).

Free models: [TurboSquid](https://www.turbosquid.com/), [Free3D](https://free3d.com/)

### 2. Build

```bash
gcc -o build\forge3d.exe src\main.c src\vector.c src\matrix.c src\projection.c src\camera.c src\mesh.c src\mesh_loader.c src\material.c src\lighting.c src\texture.c -lSDL3 -lSDL3_image -lm -I src
```
or
```bash
gcc -o build\forge3d src\main.c src\vector.c src\matrix.c src\projection.c src\camera.c src\mesh.c src\mesh_loader.c src\material.c src\lighting.c src\texture.c -lSDL3 -lSDL3_image -lm -I src
```

### 3. Run

```bash
build\forge3d.exe
```
or
```bash
./build/forge3d.exe
```

## Controls

- **Left-click + drag**: Orbit camera
- **Scroll wheel**: Zoom
- **WASD/Q/E**: Move camera
- **ESC**: Exit

## Features

- **OBJ Loading**: Vertices, normals, texture coords, multiple face formats
- **Auto Normals**: Computed automatically if not in file
- **Lighting**: Ambient + diffuse (N·L), up to 2 lights
- **Materials**: Simple base color system
- **Orbit Camera**: Mouse-controlled scene navigation

## Troubleshooting

**"OBJ load failed"**: Check `model.obj` exists or update path in `src/main.c` line ~97

**"Mesh validation failed"**: OBJ file has invalid indices - check error message

**"SDL3 not found"**: Install SDL3 development libraries
