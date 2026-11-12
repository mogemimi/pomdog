// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	"github.com/mogemimi/pomdog/tools/pkg/magicavoxel"
)

// convert turns a validated voxel model into the content of a `.vox` file of
// version 150, which both old and recent releases of MagicaVoxel read. The
// file always has a palette chunk, and the dictionary entries that the voxel
// model keeps in their own fields are written only when they differ from
// what MagicaVoxel assumes without them.
func convert(m *schemas.VoxelModelT) *magicavoxel.File {
	f := &magicavoxel.File{Version: magicavoxel.Version150}

	for _, g := range m.Grids {
		model := magicavoxel.Model{
			SizeX:  int32(g.SizeX),
			SizeY:  int32(g.SizeY),
			SizeZ:  int32(g.SizeZ),
			Voxels: make([]magicavoxel.Voxel, len(g.Voxels)),
		}
		for i, v := range g.Voxels {
			model.Voxels[i] = magicavoxel.Voxel{X: v.X, Y: v.Y, Z: v.Z, ColorIndex: v.ColorIndex}
		}
		f.Models = append(f.Models, model)
	}

	for i, n := range m.Nodes {
		node := magicavoxel.Node{ID: int32(i), Attributes: namedDict(n.Name, n.Hidden, n.Attributes)}
		switch n.Type {
		case schemas.VoxelModelNodeTypeTransform:
			node.Kind = magicavoxel.TransformNode
			node.ChildID = int32(n.Children[0])
			node.ReservedID = -1
			node.LayerID = n.Layer
			for _, frame := range n.Frames {
				node.Frames = append(node.Frames, frameDict(frame, len(n.Frames) > 1))
			}
		case schemas.VoxelModelNodeTypeGroup:
			node.Kind = magicavoxel.GroupNode
			for _, child := range n.Children {
				node.ChildIDs = append(node.ChildIDs, int32(child))
			}
		case schemas.VoxelModelNodeTypeShape:
			node.Kind = magicavoxel.ShapeNode
			for _, sm := range n.Models {
				var d magicavoxel.Dict
				if len(n.Models) > 1 || sm.Frame != 0 {
					d = append(d, magicavoxel.DictEntry{Key: magicavoxel.KeyFrame, Value: fmt.Sprint(sm.Frame)})
				}
				d = appendAttributes(d, sm.Attributes)
				node.Models = append(node.Models, magicavoxel.ShapeModel{ModelID: int32(sm.Grid), Attributes: d})
			}
		}
		f.Nodes = append(f.Nodes, node)
	}

	for _, l := range m.Layers {
		f.Layers = append(f.Layers, magicavoxel.Layer{
			ID:         l.Id,
			Attributes: namedDict(l.Name, l.Hidden, l.Attributes),
			ReservedID: -1,
		})
	}

	var palette [256]magicavoxel.Color
	for i, c := range m.Palette {
		palette[i] = magicavoxel.Color{R: c.R, G: c.G, B: c.B, A: c.A}
	}
	f.Palette = &palette

	for _, mat := range m.Materials {
		f.Materials = append(f.Materials, magicavoxel.Material{ID: mat.Id, Properties: appendAttributes(nil, mat.Properties)})
	}
	return f
}

func appendAttributes(d magicavoxel.Dict, attributes []*schemas.VoxelModelAttributeT) magicavoxel.Dict {
	for _, a := range attributes {
		d = append(d, magicavoxel.DictEntry{Key: a.Key, Value: a.Value})
	}
	return d
}

func namedDict(name string, hidden bool, attributes []*schemas.VoxelModelAttributeT) magicavoxel.Dict {
	var d magicavoxel.Dict
	if name != "" {
		d = append(d, magicavoxel.DictEntry{Key: magicavoxel.KeyName, Value: name})
	}
	if hidden {
		d = append(d, magicavoxel.DictEntry{Key: magicavoxel.KeyHidden, Value: magicavoxel.FormatBool(true)})
	}
	return appendAttributes(d, attributes)
}

func frameDict(frame *schemas.VoxelModelTransformFrameT, animated bool) magicavoxel.Dict {
	var d magicavoxel.Dict
	if frame.Rotation != uint8(magicavoxel.IdentityRotation) {
		d = append(d, magicavoxel.DictEntry{Key: magicavoxel.KeyRotation, Value: magicavoxel.FormatRotation(magicavoxel.Rotation(frame.Rotation))})
	}
	if t := frame.Translation; t != nil && (t.X != 0 || t.Y != 0 || t.Z != 0) {
		d = append(d, magicavoxel.DictEntry{Key: magicavoxel.KeyTranslation, Value: magicavoxel.FormatTranslation([3]int32{t.X, t.Y, t.Z})})
	}
	if animated || frame.Frame != 0 {
		d = append(d, magicavoxel.DictEntry{Key: magicavoxel.KeyFrame, Value: fmt.Sprint(frame.Frame)})
	}
	return appendAttributes(d, frame.Attributes)
}
