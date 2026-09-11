# Memic

Command-line tool generating a video with randomly timed sounds.

## Prerequisites

- [Go](https://go.dev)
- [FFmpeg](https://ffmpeg.org) (must be available in `PATH`)

## Usage

```bash
go run main.go [options]
```

## Options

| Flag       | Default                | Description                                                              |
|------------|------------------------|--------------------------------------------------------------------------|
| `seed`     | Current Unix timestamp | Random seed for reproducibility                                          |
| `output`   | `output.mp4`           | Output video file                                                        |
| `sounds`   | `sounds`               | Directory containing sound files                                         |
| `image`    | *(none)*               | Path to an image file for the video background (black screen if omitted) |
| `interval` | `1m`                   | Average time between sounds                                              |
| `duration` | `1h`                   | Total video duration                                                     |

## Examples

```bash
# Generate a 1-hour video with default settings
go run main.go

# Use a specific background image and custom duration
go run main.go -image background.png -duration 30m

# Use a fixed seed for reproducible output
go run main.go -seed 42

# Specify custom sound directory and output file
go run main.go -sounds ./my-sounds -output meme.mp4
```

## License

Copyright (c) Alexandre Breteau

This software is released under the terms of the MIT License.
See the [LICENSE](LICENSE) file for further information.
