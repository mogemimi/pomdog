// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"slices"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	"github.com/mogemimi/pomdog/tools/pkg/magicavoxel"
	"github.com/mogemimi/pomdog/tools/pkg/voxelmodel"
)

// droppedChunks describes the chunks that a voxel model does not keep.
var droppedChunks = map[string]string{
	"MATT": "materials of the old format",
	"rOBJ": "render settings",
	"rCAM": "cameras",
	"NOTE": "names of palette rows",
	"IMAP": "the display order of the palette",
	"META": "settings of the document, such as the range of the animation",
}

// convert turns the content of a `.vox` file into a voxel model.
func convert(f *magicavoxel.File) (*schemas.VoxelModelT, error) {
	m := &schemas.VoxelModelT{}

	palette := magicavoxel.DefaultPalette()
	if f.Palette != nil {
		palette = *f.Palette
	}
	for _, c := range palette {
		m.Palette = append(m.Palette, &schemas.VoxelModelColorT{R: c.R, G: c.G, B: c.B, A: c.A})
	}

	for i, model := range f.Models {
		for _, size := range []int32{model.SizeX, model.SizeY, model.SizeZ} {
			if size < 1 || size > voxelmodel.MaxGridSize {
				return nil, fmt.Errorf("model %d has the size %dx%dx%d, which is not between 1 and %d", i, model.SizeX, model.SizeY, model.SizeZ, voxelmodel.MaxGridSize)
			}
		}
		grid := &schemas.VoxelModelGridT{
			SizeX:  uint32(model.SizeX),
			SizeY:  uint32(model.SizeY),
			SizeZ:  uint32(model.SizeZ),
			Voxels: make([]*schemas.VoxelModelVoxelT, len(model.Voxels)),
		}
		for j, v := range model.Voxels {
			grid.Voxels[j] = &schemas.VoxelModelVoxelT{X: v.X, Y: v.Y, Z: v.Z, ColorIndex: v.ColorIndex}
		}
		m.Grids = append(m.Grids, grid)
	}

	nodes, notes, err := convertNodes(f)
	if err != nil {
		return nil, err
	}
	m.Nodes = nodes
	m.Notes = append(m.Notes, notes...)

	for _, l := range f.Layers {
		layer := &schemas.VoxelModelLayerT{Id: l.ID}
		if layer.Name, layer.Hidden, layer.Attributes, err = splitNamedDict(l.Attributes); err != nil {
			return nil, fmt.Errorf("layer %d: %w", l.ID, err)
		}
		if l.ReservedID != -1 {
			m.Notes = append(m.Notes, fmt.Sprintf("layer %d had the reserved ID %d, which a voxel model does not keep", l.ID, l.ReservedID))
		}
		m.Layers = append(m.Layers, layer)
	}

	for _, mat := range f.Materials {
		m.Materials = append(m.Materials, &schemas.VoxelModelMaterialT{Id: mat.ID, Properties: attributes(mat.Properties)})
	}

	m.Notes = append(m.Notes, describeDroppedChunks(f.OtherChunks)...)

	if err := voxelmodel.Validate(m); err != nil {
		return nil, err
	}
	return m, nil
}

func attributes(d magicavoxel.Dict) []*schemas.VoxelModelAttributeT {
	var out []*schemas.VoxelModelAttributeT
	for _, e := range d {
		out = append(out, &schemas.VoxelModelAttributeT{Key: e.Key, Value: e.Value})
	}
	return out
}

// splitNamedDict takes the name and the hidden flag out of the dictionary of
// a node or a layer.
func splitNamedDict(d magicavoxel.Dict) (name string, hidden bool, rest []*schemas.VoxelModelAttributeT, err error) {
	for _, e := range d {
		switch e.Key {
		case magicavoxel.KeyName:
			name = e.Value
		case magicavoxel.KeyHidden:
			if hidden, err = magicavoxel.ParseBool(e.Value); err != nil {
				return "", false, nil, fmt.Errorf("%s: %w", e.Key, err)
			}
		default:
			rest = append(rest, &schemas.VoxelModelAttributeT{Key: e.Key, Value: e.Value})
		}
	}
	return name, hidden, rest, nil
}

// convertNodes turns the scene graph nodes into voxel model nodes, whose
// indices follow the order of the node IDs, so that the root with the ID 0
// comes first.
func convertNodes(f *magicavoxel.File) ([]*schemas.VoxelModelNodeT, []string, error) {
	if len(f.Nodes) == 0 {
		return nil, nil, nil
	}

	ids := make([]int32, 0, len(f.Nodes))
	for _, n := range f.Nodes {
		ids = append(ids, n.ID)
	}
	slices.Sort(ids)
	indices := map[int32]uint32{}
	for i, id := range ids {
		if i > 0 && ids[i-1] == id {
			return nil, nil, fmt.Errorf("two nodes have the ID %d", id)
		}
		indices[id] = uint32(i)
	}
	if ids[0] != 0 {
		return nil, nil, fmt.Errorf("the scene graph has no root node with the ID 0")
	}
	index := func(id int32) (uint32, error) {
		i, ok := indices[id]
		if !ok {
			return 0, fmt.Errorf("node %d does not exist", id)
		}
		return i, nil
	}

	var notes []string
	nodes := make([]*schemas.VoxelModelNodeT, len(f.Nodes))
	for _, n := range f.Nodes {
		node := &schemas.VoxelModelNodeT{Layer: -1}
		var err error
		if node.Name, node.Hidden, node.Attributes, err = splitNamedDict(n.Attributes); err != nil {
			return nil, nil, fmt.Errorf("node %d: %w", n.ID, err)
		}

		switch n.Kind {
		case magicavoxel.TransformNode:
			node.Type = schemas.VoxelModelNodeTypeTransform
			node.Layer = n.LayerID
			child, err := index(n.ChildID)
			if err != nil {
				return nil, nil, fmt.Errorf("transform node %d: %w", n.ID, err)
			}
			node.Children = []uint32{child}
			if n.ReservedID != -1 {
				notes = append(notes, fmt.Sprintf("transform node %d had the reserved ID %d, which a voxel model does not keep", n.ID, n.ReservedID))
			}
			for _, d := range n.Frames {
				frame, err := convertFrame(d)
				if err != nil {
					return nil, nil, fmt.Errorf("transform node %d: %w", n.ID, err)
				}
				node.Frames = append(node.Frames, frame)
			}
		case magicavoxel.GroupNode:
			node.Type = schemas.VoxelModelNodeTypeGroup
			for _, id := range n.ChildIDs {
				child, err := index(id)
				if err != nil {
					return nil, nil, fmt.Errorf("group node %d: %w", n.ID, err)
				}
				node.Children = append(node.Children, child)
			}
		case magicavoxel.ShapeNode:
			node.Type = schemas.VoxelModelNodeTypeShape
			for _, sm := range n.Models {
				if sm.ModelID < 0 || int(sm.ModelID) >= len(f.Models) {
					return nil, nil, fmt.Errorf("shape node %d refers to model %d, which does not exist", n.ID, sm.ModelID)
				}
				model := &schemas.VoxelModelShapeModelT{Grid: uint32(sm.ModelID)}
				for _, e := range sm.Attributes {
					if e.Key == magicavoxel.KeyFrame {
						if model.Frame, err = magicavoxel.ParseInt32(e.Value); err != nil {
							return nil, nil, fmt.Errorf("shape node %d: %s: %w", n.ID, e.Key, err)
						}
						continue
					}
					model.Attributes = append(model.Attributes, &schemas.VoxelModelAttributeT{Key: e.Key, Value: e.Value})
				}
				node.Models = append(node.Models, model)
			}
		}
		nodes[indices[n.ID]] = node
	}
	return nodes, notes, nil
}

func convertFrame(d magicavoxel.Dict) (*schemas.VoxelModelTransformFrameT, error) {
	frame := &schemas.VoxelModelTransformFrameT{Rotation: uint8(magicavoxel.IdentityRotation)}
	for _, e := range d {
		switch e.Key {
		case magicavoxel.KeyFrame:
			v, err := magicavoxel.ParseInt32(e.Value)
			if err != nil {
				return nil, fmt.Errorf("%s: %w", e.Key, err)
			}
			frame.Frame = v
		case magicavoxel.KeyRotation:
			r, err := magicavoxel.ParseRotation(e.Value)
			if err != nil {
				return nil, fmt.Errorf("%s: %w", e.Key, err)
			}
			frame.Rotation = uint8(r)
		case magicavoxel.KeyTranslation:
			t, err := magicavoxel.ParseTranslation(e.Value)
			if err != nil {
				return nil, fmt.Errorf("%s: %w", e.Key, err)
			}
			// NOTE: A zero translation stays absent, as MagicaVoxel writes it.
			if t != [3]int32{} {
				frame.Translation = &schemas.VoxelModelPoint3T{X: t[0], Y: t[1], Z: t[2]}
			}
		default:
			frame.Attributes = append(frame.Attributes, &schemas.VoxelModelAttributeT{Key: e.Key, Value: e.Value})
		}
	}
	return frame, nil
}

// describeDroppedChunks writes a note for each kind of chunk that a voxel
// model does not keep, in the order that the kinds first appear.
func describeDroppedChunks(chunks []magicavoxel.Chunk) []string {
	var order []string
	counts := map[string]int{}
	for _, c := range chunks {
		if counts[c.ID] == 0 {
			order = append(order, c.ID)
		}
		counts[c.ID]++
	}
	var notes []string
	for _, id := range order {
		what, ok := droppedChunks[id]
		if !ok {
			what = "an unknown kind of data"
		}
		notes = append(notes, fmt.Sprintf("dropped %d %q chunks: %s", counts[id], id, what))
	}
	return notes
}
