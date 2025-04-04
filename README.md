# Hydra

Hydra is an experimental Nintendo Switch emulator for macOS.

Use the `dev` branch for the most up-to-date version.

## Building

You need to provide a codesigning identity when configuring CMake.

```
mkdir build
cd build
cmake .. -DCODESIGN_IDENTITY="Apple Development: SOMETHING@icloud.com (XXXXXXXXXX)"
make -j4
```
