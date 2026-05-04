// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"image"
	"image/color"
	"sort"
)

type TexturePackNode struct {
	ChildSmall *TexturePackNode
	ChildLarge *TexturePackNode
	Rect       image.Rectangle
	Source     *TextureAtlasSource
}

type TextureAtlasSource struct {
	Image      image.Image
	Name       string
	ClipBounds image.Rectangle
}

type TextureAtlasGenerator struct {
	Sources []*TextureAtlasSource
}

func (gen *TextureAtlasGenerator) Add(name string, img image.Image, tightPacking bool) {
	source := &TextureAtlasSource{
		Image:      img,
		Name:       name,
		ClipBounds: img.Bounds(),
	}
	if tightPacking {
		source.ClipBounds = clipImage(img)
	}
	gen.Sources = append(gen.Sources, source)
}

func (node *TexturePackNode) Insert(bounds image.Rectangle) *TexturePackNode {
	if !node.IsLeaf() {
		if n := node.ChildSmall.Insert(bounds); n != nil {
			return n
		}
		return node.ChildLarge.Insert(bounds)
	}

	if bounds.Dx() > node.Rect.Dx() || bounds.Dy() > node.Rect.Dy() {
		return nil
	}

	dw := node.Rect.Dx() - bounds.Dx()
	dh := node.Rect.Dy() - bounds.Dy()

	if dw > dh {
		node.ChildLarge = &TexturePackNode{
			Rect: image.Rect(
				node.Rect.Min.X+bounds.Dx(), node.Rect.Min.Y,
				node.Rect.Max.X, node.Rect.Max.Y),
		}
		node.ChildSmall = &TexturePackNode{
			Rect: image.Rect(
				node.Rect.Min.X, node.Rect.Min.Y+bounds.Dy(),
				node.Rect.Min.X+bounds.Dx(), node.Rect.Max.Y),
		}
	} else {
		node.ChildSmall = &TexturePackNode{
			Rect: image.Rect(
				node.Rect.Min.X+bounds.Dx(), node.Rect.Min.Y,
				node.Rect.Max.X, node.Rect.Min.Y+bounds.Dy()),
		}
		node.ChildLarge = &TexturePackNode{
			Rect: image.Rect(
				node.Rect.Min.X, node.Rect.Min.Y+bounds.Dy(),
				node.Rect.Max.X, node.Rect.Max.Y),
		}
	}

	clampRect := func(r *image.Rectangle) {
		if r.Max.X > node.Rect.Max.X {
			r.Max.X = node.Rect.Max.X
		}
		if r.Max.Y > node.Rect.Max.Y {
			r.Max.Y = node.Rect.Max.Y
		}
		if r.Min.X > r.Max.X {
			r.Min.X = r.Max.X
		}
		if r.Min.Y > r.Max.Y {
			r.Min.Y = r.Max.Y
		}
	}
	clampRect(&node.ChildSmall.Rect)
	clampRect(&node.ChildLarge.Rect)

	return node
}

func (node *TexturePackNode) IsLeaf() bool {
	return node.ChildSmall == nil && node.ChildLarge == nil
}

func (node *TexturePackNode) Traverse(f func(*TexturePackNode)) {
	f(node)

	if node.ChildSmall != nil {
		node.ChildSmall.Traverse(f)
	}
	if node.ChildLarge != nil {
		node.ChildLarge.Traverse(f)
	}
}

func (gen *TextureAtlasGenerator) Generate(width, height, perTileSpacing int) (*TexturePackNode, error) {
	// NOTE: Sort by width first (largest first), then stably re-sort by area
	// (largest first). The stable area sort preserves the width-based ordering
	// as a tiebreaker for sprites of equal area.
	sort.SliceStable(gen.Sources, func(i, j int) bool {
		a := gen.Sources[i].ClipBounds.Size()
		b := gen.Sources[j].ClipBounds.Size()
		return a.X > b.X
	})
	sort.SliceStable(gen.Sources, func(i, j int) bool {
		a := gen.Sources[i].ClipBounds.Size()
		b := gen.Sources[j].ClipBounds.Size()
		aa := a.X * a.Y
		bb := b.X * b.Y
		return aa > bb
	})

	root := &TexturePackNode{
		Rect: image.Rect(0, 0, width, height),
	}

	for _, source := range gen.Sources {
		bounds := source.ClipBounds
		bounds.Max.X += perTileSpacing
		bounds.Max.Y += perTileSpacing

		node := root.Insert(bounds)
		if node == nil {
			return nil, fmt.Errorf("texture atlas is full — increase width/height")
		}
		node.Source = source
	}

	return root, nil
}

func clipImage(img image.Image) image.Rectangle {
	bounds := img.Bounds()

	result := image.Rectangle{
		Min: image.Point{X: bounds.Max.X, Y: bounds.Max.Y},
		Max: image.Point{X: bounds.Min.X, Y: bounds.Min.Y},
	}

	for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
		for x := bounds.Min.X; x < bounds.Max.X; x++ {
			if _, _, _, a := img.At(x, y).RGBA(); a != 0 {
				if x < result.Min.X {
					result.Min.X = x
				}
				if y < result.Min.Y {
					result.Min.Y = y
				}
				if x+1 > result.Max.X {
					result.Max.X = x + 1
				}
				if y+1 > result.Max.Y {
					result.Max.Y = y + 1
				}
			}
		}
	}

	if result.Min.X > result.Max.X {
		result.Min.X = result.Max.X
	}
	if result.Min.Y > result.Max.Y {
		result.Min.Y = result.Max.Y
	}

	return result
}

func multiplyAlpha(img *image.NRGBA) {
	multiply := func(x, a uint8) uint8 {
		return uint8((uint32(x)*uint32(a) + 127) / 255)
	}

	for y := img.Rect.Min.Y; y < img.Rect.Max.Y; y++ {
		for x := img.Rect.Min.X; x < img.Rect.Max.X; x++ {
			c := img.NRGBAAt(x, y)
			if c.A == 255 {
				// NOTE: Skip fully opaque pixels.
				continue
			}

			d := color.NRGBA{R: 0, G: 0, B: 0, A: c.A}
			if c.A == 0 {
				// NOTE: Set RGB to 0 for fully transparent pixels to avoid color bleeding.
				d.R = 0
				d.G = 0
				d.B = 0
			} else {
				// NOTE: Premultiply alpha for partially transparent pixels.
				d.R = multiply(c.R, c.A)
				d.G = multiply(c.G, c.A)
				d.B = multiply(c.B, c.A)
			}
			img.SetNRGBA(x, y, d)
		}
	}
}
