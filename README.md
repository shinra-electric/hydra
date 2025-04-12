# Hydra

Hydra is an experimental Nintendo Switch emulator for macOS.

Use the `dev` branch for the most up-to-date version.

## Usage

### Building

First, you need to find your codesigning identity.

```sh
security find-identity -p codesigning
```

Now configure CMake and build with Make.

```sh
mkdir build
cd build
cmake .. -G "Ninja" -DCODESIGN_IDENTITY="Apple Development: SOMETHING (XXXXXXXXXX)"
make -j4
```

If you want to use the SwiftUI frontend instead of SDL3, you can use the `-DFRONTEND=SwiftUI` option.

### Running

You can run the emulator with the following command:

```sh
src/frontend/hydra
```

If you are using the SDL3 frontend, you also need to provide a path to a ROM:

```sh
src/frontend/hydra /path/to/rom
```
