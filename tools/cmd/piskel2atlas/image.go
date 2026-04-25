// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"errors"
	"fmt"
	"image"
	"image/color"
	"math"

	piskel "github.com/mogemimi/pomdog/tools/pkg/piskel"
)

func renderTile(tile *Tile, p *piskel.Piskel) (*image.NRGBA, error) {
	frameIndex := tile.Index - 1

	if frameIndex >= p.FramesCount() {
		return nil, errors.New("tile.Index must be < piskel.FramesCount()")
	}
	if frameIndex < 0 {
		return nil, errors.New("tile.Index must be >= 0")
	}

	layerMerged := image.NewNRGBA(image.Rect(0, 0, p.Piskel.Width, p.Piskel.Height))

	for layerIndex, layer := range p.Piskel.Layers {
		xoffset := frameIndex * p.Piskel.Width
		chunk := layer.Chunks[0]

		img, err := chunk.Image()
		if err != nil {
			return nil, fmt.Errorf("chunk.Image() failed: %w", err)
		}

		if !tile.LayerEnabled(layerIndex) {
			continue
		}
		if tile.OpacityLayer != nil && *tile.OpacityLayer == layerIndex {
			continue
		}

		if layer.Opacity < 1.0 {
			for y := 0; y < p.Piskel.Height; y++ {
				for x := 0; x < p.Piskel.Width; x++ {
					pixel := img.At(x+xoffset, y)
					r, g, b, a := pixel.RGBA()
					if a == 0 {
						continue
					}

					// NOTE: Alpha blending
					srcA := from16BitToUniform(a) * layer.Opacity
					invertSrcA := 1.0 - srcA

					dst := layerMerged.NRGBAAt(x, y)
					out := dst

					out.R = fromUniformTo8bit((from8BitToUniform(dst.R) * invertSrcA) + (from16BitToUniform(r) * srcA))
					out.G = fromUniformTo8bit((from8BitToUniform(dst.G) * invertSrcA) + (from16BitToUniform(g) * srcA))
					out.B = fromUniformTo8bit((from8BitToUniform(dst.B) * invertSrcA) + (from16BitToUniform(b) * srcA))
					out.A = fromUniformTo8bit(math.Min(from8BitToUniform(dst.A)*invertSrcA+srcA, 1.0))

					layerMerged.Set(x, y, out)
				}
			}
		} else {
			for y := 0; y < p.Piskel.Height; y++ {
				for x := 0; x < p.Piskel.Width; x++ {
					pixel := img.At(x+xoffset, y)
					_, _, _, a := pixel.RGBA()
					if a == 0 {
						continue
					}
					layerMerged.Set(x, y, pixel)
				}
			}
		}
	}

	if tile.OpacityLayer != nil {
		layerIndex := *tile.OpacityLayer
		if layerIndex >= len(p.Piskel.Layers) {
			return nil, fmt.Errorf("invalid opacity layer (%d) must be <= %d", layerIndex, len(p.Piskel.Layers))
		}

		layer := p.Piskel.Layers[layerIndex]

		xoffset := frameIndex * p.Piskel.Width
		chunk := layer.Chunks[0]

		img, err := chunk.Image()
		if err != nil {
			return nil, fmt.Errorf("chunk.Image() failed: %w", err)
		}

		for y := 0; y < p.Piskel.Height; y++ {
			for x := 0; x < p.Piskel.Width; x++ {
				pixel := img.At(x+xoffset, y)
				_, g, _, a := pixel.RGBA()
				if a == 0 {
					continue
				}

				// NOTE: Use the green channel as the alpha mask value.
				dst := layerMerged.NRGBAAt(x, y)
				dst.A = uint8(g >> 8)
				layerMerged.Set(x, y, dst)
			}
		}
	}

	if tile.MaskRect != nil {
		for y := 0; y < p.Piskel.Height; y++ {
			for x := 0; x < p.Piskel.Width; x++ {
				if x >= tile.MaskRect.MinX && x < tile.MaskRect.MaxX &&
					y >= tile.MaskRect.MinY && y < tile.MaskRect.MaxY {
					continue
				}
				layerMerged.Set(x, y, color.NRGBA{})
			}
		}
	}

	return layerMerged, nil
}

func from16BitToUniform(x uint32) float64 {
	return float64(x) / 65535.0
}

func from8BitToUniform(x uint8) float64 {
	return float64(x) / 255.0
}

func fromUniformTo8bit(x float64) uint8 {
	return uint8(255.0 * math.Min(x, 1.0))
}

func multiplyAlpha(img *image.NRGBA) error {
	multiply := func(x, a uint8) uint8 {
		f := ((float64(x) / 255.0) * (float64(a) / 255.0)) * 255.0
		d := uint32(f)
		if d > 255 {
			d = 255
		}
		return uint8(d)
	}

	for y := 0; y < img.Bounds().Max.Y; y++ {
		for x := 0; x < img.Bounds().Max.X; x++ {
			pixel := color.RGBAModel.Convert(img.At(x, y)).(color.RGBA)
			if pixel.A < 255 {
				pixel.R = multiply(pixel.R, pixel.A)
				pixel.G = multiply(pixel.G, pixel.A)
				pixel.B = multiply(pixel.B, pixel.A)
			}
			img.Set(x, y, pixel)
		}
	}
	return nil
}

func resizeImage(img *image.NRGBA, scale int) (*image.NRGBA, error) {
	if scale <= 1 {
		return img, nil
	}

	rect := image.Rect(0, 0, img.Bounds().Size().X*scale, img.Bounds().Size().Y*scale)
	resized := image.NewNRGBA(rect)

	for y := 0; y < rect.Size().Y; y++ {
		for x := 0; x < rect.Size().X; x++ {
			pixel := color.RGBAModel.Convert(img.At(x/scale, y/scale)).(color.RGBA)
			resized.Set(x, y, pixel)
		}
	}
	return resized, nil
}
