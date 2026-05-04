// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"image"
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
		// NOTE: This node is too small.
		return nil
	}

	dw := node.Rect.Dx() - bounds.Dx()
	dh := node.Rect.Dy() - bounds.Dy()

	if dw > dh {
		// NOTE:
		// ```
		//                     dw
		//  node.Min       ◄────────►
		//     ┌──────────┬──────────┐
		//     │          │          │
		//     │  bounds  │          │
		//     │          │          │
		//   ▲ ├──────────┤  Child   │
		//   │ │          │  Large   │
		// dh│ │  Child   │          │
		//   │ │  Small   │          │
		//   │ │          │          │
		//   ▼ └──────────┴──────────┘
		//                         node.Max
		// ```
		node.ChildLarge = &TexturePackNode{
			Rect: image.Rect(
				node.Rect.Min.X+bounds.Dx(),
				node.Rect.Min.Y,
				node.Rect.Max.X,
				node.Rect.Max.Y),
		}
		node.ChildSmall = &TexturePackNode{
			Rect: image.Rect(
				node.Rect.Min.X,
				node.Rect.Min.Y+bounds.Dy(),
				node.Rect.Min.X+bounds.Dx(),
				node.Rect.Max.Y),
		}
	} else {
		// NOTE:
		// ```
		//                     dw
		//  node.Min       ◄────────►
		//     ┌──────────┬──────────┐
		//     │          │          │
		//     │  bounds  │  Child   │
		//     │          │  Small   │
		//   ▲ ├──────────┴──────────┤
		//   │ │                     │
		// dh│ │        Child        │
		//   │ │        Large        │
		//   │ │                     │
		//   ▼ └─────────────────────┘
		//                         node.Max
		// ```
		node.ChildSmall = &TexturePackNode{
			Rect: image.Rect(
				node.Rect.Min.X+bounds.Dx(),
				node.Rect.Min.Y,
				node.Rect.Max.X,
				node.Rect.Min.Y+bounds.Dy()),
		}
		node.ChildLarge = &TexturePackNode{
			Rect: image.Rect(
				node.Rect.Min.X,
				node.Rect.Min.Y+bounds.Dy(),
				node.Rect.Max.X,
				node.Rect.Max.Y),
		}
	}

	clampRect := func(rect *image.Rectangle) {
		rect.Max.X = minInt(rect.Max.X, node.Rect.Max.X)
		rect.Max.Y = minInt(rect.Max.Y, node.Rect.Max.Y)
		rect.Min.X = minInt(rect.Min.X, rect.Max.X)
		rect.Min.Y = minInt(rect.Min.Y, rect.Max.Y)
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

		// NOTE: Add per-tile spacing.
		bounds.Max.X += perTileSpacing
		bounds.Max.Y += perTileSpacing

		node := root.Insert(bounds)
		if node == nil {
			return nil, fmt.Errorf("texture atlas is full")
		}

		node.Source = source
	}

	return root, nil
}

func clipImage(img image.Image) image.Rectangle {
	bounds := img.Bounds()

	result := image.Rectangle{}
	result.Min.X = bounds.Max.X
	result.Max.X = bounds.Min.X
	result.Min.Y = bounds.Max.Y
	result.Max.Y = bounds.Min.Y

	for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
		for x := bounds.Min.X; x < bounds.Max.X; x++ {
			pixel := img.At(x, y)
			if _, _, _, a := pixel.RGBA(); a != 0 {
				result.Min.X = minInt(result.Min.X, x)
				result.Min.Y = minInt(result.Min.Y, y)
				result.Max.X = maxInt(result.Max.X, x+1)
				result.Max.Y = maxInt(result.Max.Y, y+1)
			}
		}
	}

	result.Min.X = minInt(result.Min.X, result.Max.X)
	result.Min.Y = minInt(result.Min.Y, result.Max.Y)

	return result
}

func minInt(a, b int) int {
	if a < b {
		return a
	}
	return b
}

func maxInt(a, b int) int {
	if a > b {
		return a
	}
	return b
}
