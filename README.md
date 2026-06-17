# Joltman - Zombie Shooter

A simple top-down zombie shooter game built with modern C++ and [Raylib](https://www.raylib.com/).

## Features
- Fast-paced zombie survival gameplay
- Multiple weapons (Pistol, Shotgun, Machine Gun)
- Dynamic blood, impact particles, and body fragmentation effects
- Fully resizable window and fullscreen support
- Auto-aim system so you can focus on dodging!

## Controls
- **W, A, S, D**: Move around
- **SPACE**: Hold to auto-aim and shoot
- **1, 2, 3**: Switch weapons
- **F11 / ALT+ENTER**: Toggle Fullscreen
- **R**: Restart game (when you die)

## How to Compile & Run locally
You will need Meson, Ninja, and a C++ compiler.

1. Setup the build directory:
```bash
meson setup build
```
2. Compile the game:
```bash
meson compile -C build
```
3. Run it!
```bash
./build/joltman
```

## Automated Builds
This project is set up with GitHub Actions! Every time you push to the repository, it will automatically build and package the game for **Windows, macOS, and Linux**. The compiled zip files can be downloaded from the "Actions" tab on GitHub.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
