package main

import (
	"flag"
	"fmt"
	"log"
	"math"
	"math/rand"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"time"
)

func main() {
	seed := *flag.Int64("seed", time.Now().Unix(), "Random seed")
	output := *flag.String("output", "output.mp4", "Output video file")

	soundsPath := *flag.String("sounds", "sounds", "Directory containing sound files")
	imagePath := *flag.String("image", "", "Path to an image file for the video background")

	avgInterval := *flag.Duration("interval", time.Minute, "Average time between sounds")
	totalDuration := *flag.Duration("duration", time.Hour, "Total video duration")

	flag.Parse()

	rng := rand.New(rand.NewSource(seed))
	entries, err := os.ReadDir(soundsPath)

	if err != nil {
		log.Fatalln(err)
	}

	var sounds []string

	for _, e := range entries {
		if e.IsDir() {
			continue
		}

		sounds = append(sounds, filepath.Join(soundsPath, e.Name()))
	}

	args := []string{
		"-y",
	}

	switch imagePath {
	case "":
		args = append(args, "-f", "lavfi", "-i", "color=black")

	default:
		args = append(args, "-loop", "1", "-framerate", "1", "-i", imagePath)
	}

	var filterParts, mixInputs []string

	soundDelay := avgInterval
	index := 0

	for {
		soundDelay += time.Duration(-avgInterval.Seconds() * math.Log(1-rng.Float64()) * float64(time.Second))
		index += 1

		if soundDelay >= totalDuration {
			break
		}

		filterParts = append(filterParts, fmt.Sprintf("[%d:a]adelay=%d|%d[a%d]", index, soundDelay.Milliseconds(), soundDelay.Milliseconds(), index))
		mixInputs = append(mixInputs, fmt.Sprintf("[a%d]", index))

		args = append(args, "-i", sounds[rng.Intn(len(sounds))])
	}

	filterParts = append(filterParts, fmt.Sprintf("%samix=inputs=%d:duration=longest:dropout_transition=0:normalize=0", strings.Join(mixInputs, ""), len(filterParts)))

	args = append(args,
		"-vf", "scale=1920x1080",
		"-filter_complex", strings.Join(filterParts, ";"),
		"-t", strconv.FormatFloat(totalDuration.Seconds(), 'f', 3, 64),
		output,
	)

	cmd := exec.Command("ffmpeg", args...)

	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	if err := cmd.Run(); err != nil {
		log.Fatalln(err)
	}

	log.Println("Output saved to ", output)
}
