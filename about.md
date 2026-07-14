# LinuxWineBridgeTest

Proof of concept that a Geometry Dash mod running under Proton can call native Linux functions on the host.

Wine isolates the Windows process from the host, so a mod can't directly use Linux APIs. This project works around that by bridging the two sides:

- A Wine-native DLL `nativelib` loaded by the mod provides a thin RPC interface via a Unix socket
- A native Rust process `rust-bridge` runs on the host side, receives commands over that socket, and executes them.

The mod itself is a [Geode](https://geode-sdk.org/) mod for Geometry Dash.

## Architecture

```
Geode mod under wine talks to NativeBridge.cpp -> then nativelib.dll (winegcc)
-> then Unix socket -> then rust-bridge (native) -> and then X11 / host APIs (for example)
```

## Build

The project has three independently built parts:

**Geode mod** (requires [Geode CLI](https://github.com/geode-sdk/cli)):
```sh
geode build
```

**nativelib** (requires Wine development headers and winegcc):
```sh
cd src/linux-wine-nativelib/nativelib
make
```

**rust-bridge** (requires Rust toolchain):
```sh
cd rust-bridge
make
```

**...or you can use AIO native part build**
```sh
./build-linux-native-part.sh
```

# WIP