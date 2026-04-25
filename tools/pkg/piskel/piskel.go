// Copyright mogemimi. Distributed under the MIT license.

package piskel

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"image"
	"image/png"
	"os"
	"strings"
)

type Piskel struct {
	ModelVersion int        `json:"modelVersion"`
	Piskel       PiskelBody `json:"piskel"`
}

type PiskelBody struct {
	Name            string         `json:"name"`
	Description     string         `json:"description"`
	FPS             int            `json:"fps"`
	Height          int            `json:"height"`
	Width           int            `json:"width"`
	LayersAsStrings []string       `json:"layers"`
	Layers          []*PiskelLayer `json:"-"`
}

type PiskelLayer struct {
	Name       string        `json:"name"`
	Opacity    float64       `json:"opacity"`
	FrameCount int           `json:"frameCount"`
	Chunks     []PiskelChunk `json:"chunks"`
}

type PiskelChunk struct {
	Layout    [][]int `json:"layout"`
	Base64PNG string  `json:"base64PNG"`
}

func (piskel *Piskel) ReadFile(file string) error {
	buf, err := os.ReadFile(file)
	if err != nil {
		return fmt.Errorf("os.ReadFile(): %w", err)
	}

	if err := json.Unmarshal(buf, piskel); err != nil {
		return fmt.Errorf("json.Unmarshal(): %w", err)
	}

	for _, layerStr := range piskel.Piskel.LayersAsStrings {
		layer := &PiskelLayer{}
		if err := json.Unmarshal([]byte(layerStr), &layer); err != nil {
			return fmt.Errorf("json.Unmarshal(): %w", err)
		}
		piskel.Piskel.Layers = append(piskel.Piskel.Layers, layer)
	}

	return nil
}

func (piskel *Piskel) FramesCount() int {
	framesCount := 0
	for _, layer := range piskel.Piskel.Layers {
		if framesCount < layer.FrameCount {
			framesCount = layer.FrameCount
		}
	}

	return framesCount
}

func (chunk *PiskelChunk) Image() (image.Image, error) {
	source := strings.TrimPrefix(chunk.Base64PNG, "data:image/png;base64,")

	img, err := png.Decode(base64.NewDecoder(base64.StdEncoding, strings.NewReader(source)))
	if err != nil {
		return nil, err
	}
	return img, nil
}
