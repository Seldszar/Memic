# Memic

Command-line tool playing random sounds at random delays in realtime.

Each loop waits a random delay, then picks a random sound file and plays it with a random stereo pan and pitch. Playback uses [miniaudio](https://miniaud.io) through the default output device, so volume is controlled via the OS audio mixer (e.g. Windows volume mixer).

Supported sound formats: MP3, WAV, OGG, FLAC.

## Prerequisites

- [CMake](https://cmake.org) >= 3.15
- A C++23 compiler (e.g. MSVC, GCC 13+, Clang 16+)

## Build

```bash
cmake -B build
cmake --build build --config Release
```

The executable is produced as `memic` (`memic.exe` on Windows) inside the build directory, e.g. `build/Release/memic` or `build/Debug/memic.exe` depending on the selected configuration and generator.

## Usage

```bash
memic [options]
```

All regular files directly inside the input directory are used as clips. Subdirectories are ignored. If no file is found, the program exits with an error. Press `Ctrl+C` to stop the loop.

## Options

| Flag        | Default                | Description                                                  |
|-------------|------------------------|--------------------------------------------------------------|
| `input`     | `sounds`               | Directory containing sound files                             |
| `seed`      | Current Unix timestamp | Random seed for reproducibility                              |
| `delay[lo]` | `60`                   | Minimum delay between sounds, in seconds                     |
| `delay[hi]` | `120`                  | Maximum delay between sounds, in seconds                     |
| `pan[lo]`   | `0`                    | Minimum stereo pan (`-1` = left, `1` = right)                |
| `pan[hi]`   | `0`                    | Maximum stereo pan (`-1` = left, `1` = right)                |
| `pitch[lo]` | `0.5`                  | Minimum pitch (`< 1` = slower/deeper, `> 1` = faster/higher) |
| `pitch[hi]` | `2`                    | Maximum pitch (`< 1` = slower/deeper, `> 1` = faster/higher) |

## Examples

```bash
# Run with default settings
memic

# Use a fixed seed for reproducible output
memic -seed 42

# Specify a custom sound directory and delay range
memic -input ./my-sounds -delay[lo]=30 -delay[hi]=60

# Enable random stereo panning and keep the default pitch range
memic -pan[lo]=-1 -pan[hi]=1

# Disable pitch shifting
memic -pitch[lo]=1 -pitch[hi]=1
```

## License

Copyright (c) Alexandre Breteau

This software is released under the terms of the MIT License.
See the [LICENSE](LICENSE) file for further information.
