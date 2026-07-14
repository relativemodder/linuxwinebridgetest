# Penguin Tweaks

[Geode](https://geode-sdk.org/) mod for Geometry Dash on Linux (Proton/Wine).

- Auto-deafen in MegaHack
- Native file dialogs instead of Wine ones

Wine cuts off the Windows process from the host, so mods can't call Linux APIs directly. This mod bridges the gap:

- `nativelib` — a Wine-native DLL that exposes an RPC interface over a Unix socket
- `rust-bridge` — a native Rust process on the host side that receives commands and calls the actual Linux APIs

## Architecture

```
Geode mod (Wine) -> NativeBridge.cpp -> nativelib.dll (winegcc)
  -> Unix socket -> rust-bridge (native) -> X11 / host APIs
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
cd src/rust-bridge
make
```

**...or build the native parts all at once:**
```sh
./build-linux-native-part.sh
```