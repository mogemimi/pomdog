package main

import (
	"flag"
	"fmt"
	"log"
	"math"
	"os"

	"github.com/go-audio/audio"
	"github.com/go-audio/wav"
)

func main() {
	var options struct {
		InFile        string
		OutFile       string
		FadeSamples   int
		FadeSeconds   float64
		TrimLoopStart bool
		AlignSamples  int
		FadeMethod    string
	}
	flag.StringVar(&options.InFile, "i", "", "input file (*.wav)")
	flag.StringVar(&options.OutFile, "o", "", "output file (*.wav)")
	flag.IntVar(&options.FadeSamples, "samples", 0, "fade section (samples)")
	flag.Float64Var(&options.FadeSeconds, "seconds", 0, "fade section (seconds)")
	flag.BoolVar(&options.TrimLoopStart, "trimstart", false, "trim the loop start")
	flag.IntVar(&options.AlignSamples, "alignsamples", 0, "trim buffer alignment samples (samples)")
	flag.StringVar(&options.FadeMethod, "fade", "equal_power", "crossfade method: linear, equal_power, logarithmic")
	flag.Parse()

	runner := Runner{
		InFile:        options.InFile,
		OutFile:       options.OutFile,
		FadeSamples:   options.FadeSamples,
		FadeSeconds:   options.FadeSeconds,
		TrimLoopStart: options.TrimLoopStart,
		AlignSamples:  options.AlignSamples,
		FadeMethod:    options.FadeMethod,
	}

	if err := run(&runner); err != nil {
		log.Fatal(err)
	}
}

type Runner struct {
	InFile        string
	OutFile       string
	FadeSamples   int
	FadeSeconds   float64
	TrimLoopStart bool
	AlignSamples  int
	FadeMethod    string
}

func run(runner *Runner) error {
	in, err := os.Open(runner.InFile)
	if err != nil {
		return fmt.Errorf("failed to open input file: %w", err)
	}
	defer in.Close()

	decoder := wav.NewDecoder(in)
	if !decoder.IsValidFile() {
		return fmt.Errorf("invalid wav file")
	}

	buf, err := decoder.FullPCMBuffer()
	if err != nil {
		return fmt.Errorf("failed to decode wav file: %w", err)
	}

	sampleRate := decoder.SampleRate
	numChannels := buf.Format.NumChannels

	fadeSamples := runner.FadeSamples

	if fadeSamples == 0 {
		if runner.FadeSeconds < 0 {
			return fmt.Errorf("invalid fadeSeconds")
		}
		fadeSamples = int(runner.FadeSeconds * float64(sampleRate))
	}

	if fadeSamples < 0 {
		return fmt.Errorf("invalid fadeSamples")
	}

	if fadeSamples <= 0 {
		return fmt.Errorf("fadeSamples is zero")
	}

	if len(buf.Data) < fadeSamples*2*numChannels {
		return fmt.Errorf("input file is too short for crossfade")
	}

	if runner.AlignSamples > 0 {
		loopSamples := len(buf.Data)
		if runner.TrimLoopStart {
			loopSamples = len(buf.Data) - (fadeSamples * numChannels)
		}
		if err := alignSamples(buf, runner.AlignSamples, loopSamples); err != nil {
			return fmt.Errorf("alignSamples() failed: %w", err)
		}
	}

	// NOTE: Compute crossfade
	fadeFunc, err := getFadeFunc(runner.FadeMethod)
	if err != nil {
		return err
	}
	crossfade(buf, fadeSamples, numChannels, fadeFunc)

	if runner.TrimLoopStart {
		// NOTE: Trim the loop start
		fadeLength := fadeSamples * numChannels
		buf.Data = buf.Data[fadeLength:]
	}

	out, err := os.Create(runner.OutFile)
	if err != nil {
		return fmt.Errorf("failed to create output file: %w", err)
	}
	defer out.Close()

	encoder := wav.NewEncoder(
		out,
		int(sampleRate),
		int(decoder.BitDepth),
		numChannels,
		int(decoder.WavAudioFormat))
	if err = encoder.Write(buf); err != nil {
		return fmt.Errorf("e.Write() failed: %w", err)
	}
	if err = encoder.Close(); err != nil {
		return fmt.Errorf("Close() failed: %w", err)
	}
	out.Close()

	return nil
}

// fadeFunc takes a normalized position t in [0,1] and returns (fadeIn, fadeOut) gain factors.
type fadeFunc func(t float64) (float64, float64)

func getFadeFunc(method string) (fadeFunc, error) {
	switch method {
	case "linear":
		return func(t float64) (float64, float64) {
			return t, 1.0 - t
		}, nil
	case "equal_power":
		return func(t float64) (float64, float64) {
			return math.Sin(t * math.Pi / 2.0), math.Cos(t * math.Pi / 2.0)
		}, nil
	case "logarithmic":
		return func(t float64) (float64, float64) {
			return math.Log10(1.0 + 9.0*t), math.Log10(1.0 + 9.0*(1.0-t))
		}, nil
	default:
		return nil, fmt.Errorf("unknown fade method: %q (expected linear, equal_power, or logarithmic)", method)
	}
}

func crossfade(buffer *audio.IntBuffer, fadeSamples int, numChannels int, fade fadeFunc) {
	data := buffer.Data
	totalSamples := len(data) / numChannels

	fadeLength := fadeSamples * numChannels
	nonfadeLength := (totalSamples - fadeSamples) * numChannels

	start := make([]int, fadeLength)
	copy(start, data[:fadeLength])

	end := make([]int, fadeLength)
	copy(end, data[nonfadeLength:])

	for i := 0; i < fadeSamples; i++ {
		t := float64(i) / float64(fadeSamples)
		fadeInFactor, fadeOutFactor := fade(t)

		for c := 0; c < numChannels; c++ {
			sampleIndex := i*numChannels + c

			fading := float64(start[sampleIndex])*fadeInFactor + float64(end[sampleIndex])*fadeOutFactor
			start[sampleIndex] = int(fading)
		}
	}

	copy(data[nonfadeLength:], start)
}

func alignSamples(buf *audio.IntBuffer, alignmentSamples, loopSamples int) error {
	sampleCount := len(buf.Data)
	if loopSamples > sampleCount {
		return fmt.Errorf("loopSamples must be <= sampleCount")
	}
	crossfadeSamples := sampleCount - loopSamples

	alignedSampleCount := (loopSamples / alignmentSamples) * alignmentSamples

	if sampleCount <= alignedSampleCount {
		return nil
	}

	trimSamples := alignedSampleCount + crossfadeSamples
	if trimSamples > sampleCount {
		return fmt.Errorf("trimSamples must be <= sampleCount")
	}

	buf.Data = buf.Data[:trimSamples]
	return nil
}
