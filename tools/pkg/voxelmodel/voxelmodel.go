// Package voxelmodel reads, writes, and validates `.voxelmodel` files, the
// FlatBuffers voxel data of Pomdog that converts to and from MagicaVoxel.
// See schemas/voxel_model.fbs for the format.
package voxelmodel

import (
	"fmt"
	"io/fs"
	"os"
	"path/filepath"

	flatbuffers "github.com/google/flatbuffers/go"
	schemas "github.com/mogemimi/pomdog/build/schemas"
	"github.com/mogemimi/pomdog/tools/pkg/magicavoxel"
)

// MaxGridSize is the largest size of a grid along an axis.
const MaxGridSize = 256

// PaletteSize is the number of colors in a palette.
const PaletteSize = 256

// reservedNodeKeys are the dictionary keys that VoxelModelNode and
// VoxelModelLayer store in their own fields.
var reservedNodeKeys = []string{magicavoxel.KeyName, magicavoxel.KeyHidden}

// reservedFrameKeys are the dictionary keys that VoxelModelTransformFrame
// stores in its own fields.
var reservedFrameKeys = []string{magicavoxel.KeyRotation, magicavoxel.KeyTranslation, magicavoxel.KeyFrame}

// reservedShapeModelKeys are the dictionary keys that VoxelModelShapeModel
// stores in its own fields.
var reservedShapeModelKeys = []string{magicavoxel.KeyFrame}

func checkAttributes(attributes []*schemas.VoxelModelAttributeT, reserved []string) error {
	for _, a := range attributes {
		if a == nil {
			return fmt.Errorf("null attribute")
		}
		if a.Key == "" {
			return fmt.Errorf("attribute with an empty key")
		}
		for _, key := range reserved {
			if a.Key == key {
				return fmt.Errorf("attribute %q belongs in its own field", a.Key)
			}
		}
	}
	return nil
}

// Validate checks the rules that schemas/voxel_model.fbs describes.
func Validate(m *schemas.VoxelModelT) error {
	if len(m.Palette) != PaletteSize {
		return fmt.Errorf("the palette has %d colors, not %d", len(m.Palette), PaletteSize)
	}
	for i, c := range m.Palette {
		if c == nil {
			return fmt.Errorf("palette color %d is null", i)
		}
	}

	for i, g := range m.Grids {
		if err := validateGrid(g); err != nil {
			return fmt.Errorf("grid %d: %w", i, err)
		}
	}

	if err := validateNodes(m); err != nil {
		return err
	}

	layerIDs := map[int32]bool{}
	for i, l := range m.Layers {
		if l == nil {
			return fmt.Errorf("layer %d is null", i)
		}
		if layerIDs[l.Id] {
			return fmt.Errorf("two layers have the ID %d", l.Id)
		}
		layerIDs[l.Id] = true
		if err := checkAttributes(l.Attributes, reservedNodeKeys); err != nil {
			return fmt.Errorf("layer %d: %w", l.Id, err)
		}
	}

	materialIDs := map[int32]bool{}
	for i, mat := range m.Materials {
		if mat == nil {
			return fmt.Errorf("material %d is null", i)
		}
		if materialIDs[mat.Id] {
			return fmt.Errorf("two materials have the ID %d", mat.Id)
		}
		materialIDs[mat.Id] = true
		if err := checkAttributes(mat.Properties, nil); err != nil {
			return fmt.Errorf("material %d: %w", mat.Id, err)
		}
	}
	return nil
}

func validateGrid(g *schemas.VoxelModelGridT) error {
	if g == nil {
		return fmt.Errorf("null grid")
	}
	for _, size := range []uint32{g.SizeX, g.SizeY, g.SizeZ} {
		if size < 1 || size > MaxGridSize {
			return fmt.Errorf("size %dx%dx%d is not between 1 and %d", g.SizeX, g.SizeY, g.SizeZ, MaxGridSize)
		}
	}
	occupied := make([]bool, int(g.SizeX)*int(g.SizeY)*int(g.SizeZ))
	for i, v := range g.Voxels {
		if v == nil {
			return fmt.Errorf("voxel %d is null", i)
		}
		if uint32(v.X) >= g.SizeX || uint32(v.Y) >= g.SizeY || uint32(v.Z) >= g.SizeZ {
			return fmt.Errorf("voxel %d at (%d, %d, %d) is outside the grid", i, v.X, v.Y, v.Z)
		}
		if v.ColorIndex == 0 {
			return fmt.Errorf("voxel %d at (%d, %d, %d) has the color index 0", i, v.X, v.Y, v.Z)
		}
		cell := (int(v.Z)*int(g.SizeY)+int(v.Y))*int(g.SizeX) + int(v.X)
		if occupied[cell] {
			return fmt.Errorf("two voxels at (%d, %d, %d)", v.X, v.Y, v.Z)
		}
		occupied[cell] = true
	}
	return nil
}

func validateNodes(m *schemas.VoxelModelT) error {
	nodes := m.Nodes
	if len(nodes) == 0 {
		return nil
	}
	for i, n := range nodes {
		if n == nil {
			return fmt.Errorf("node %d is null", i)
		}
		if err := validateNode(m, n); err != nil {
			return fmt.Errorf("node %d: %w", i, err)
		}
	}
	if nodes[0].Type != schemas.VoxelModelNodeTypeTransform {
		return fmt.Errorf("the root node is not a transform")
	}

	parents := make([]int, len(nodes))
	for i := range parents {
		parents[i] = -1
	}
	for i, n := range nodes {
		for _, child := range n.Children {
			if int(child) >= len(nodes) {
				return fmt.Errorf("node %d: child %d is out of range", i, child)
			}
			if child == 0 {
				return fmt.Errorf("node %d: the root node cannot be a child", i)
			}
			if parents[child] >= 0 {
				return fmt.Errorf("node %d has two parents, %d and %d", child, parents[child], i)
			}
			parents[child] = i
			childType := nodes[child].Type
			switch n.Type {
			case schemas.VoxelModelNodeTypeTransform:
				if childType != schemas.VoxelModelNodeTypeGroup && childType != schemas.VoxelModelNodeTypeShape {
					return fmt.Errorf("node %d: the child of a transform must be a group or a shape", i)
				}
			case schemas.VoxelModelNodeTypeGroup:
				if childType != schemas.VoxelModelNodeTypeTransform {
					return fmt.Errorf("node %d: the children of a group must be transforms", i)
				}
			}
		}
	}

	// NOTE: Every node but the root has one parent, so the nodes form a tree
	// when the root reaches all of them.
	reached := 0
	stack := []uint32{0}
	for len(stack) > 0 {
		i := stack[len(stack)-1]
		stack = stack[:len(stack)-1]
		reached++
		stack = append(stack, nodes[i].Children...)
	}
	if reached != len(nodes) {
		return fmt.Errorf("%d nodes are not reachable from the root node", len(nodes)-reached)
	}
	return nil
}

func validateNode(m *schemas.VoxelModelT, n *schemas.VoxelModelNodeT) error {
	if err := checkAttributes(n.Attributes, reservedNodeKeys); err != nil {
		return err
	}
	switch n.Type {
	case schemas.VoxelModelNodeTypeTransform:
		if len(n.Children) != 1 {
			return fmt.Errorf("a transform has %d children, not 1", len(n.Children))
		}
		if len(n.Frames) == 0 {
			return fmt.Errorf("a transform has no frames")
		}
		for i, f := range n.Frames {
			if f == nil {
				return fmt.Errorf("frame %d is null", i)
			}
			if f.Frame < 0 || (i > 0 && f.Frame <= n.Frames[i-1].Frame) {
				return fmt.Errorf("the frames are not in ascending order from 0")
			}
			if !magicavoxel.Rotation(f.Rotation).IsValid() {
				return fmt.Errorf("frame %d has an invalid rotation %d", f.Frame, f.Rotation)
			}
			if err := checkAttributes(f.Attributes, reservedFrameKeys); err != nil {
				return fmt.Errorf("frame %d: %w", f.Frame, err)
			}
		}
		if len(n.Models) != 0 {
			return fmt.Errorf("a transform has models")
		}
	case schemas.VoxelModelNodeTypeGroup:
		if len(n.Frames) != 0 || len(n.Models) != 0 || n.Layer != -1 {
			return fmt.Errorf("a group has frames, models, or a layer")
		}
	case schemas.VoxelModelNodeTypeShape:
		if len(n.Children) != 0 {
			return fmt.Errorf("a shape has children")
		}
		if len(n.Models) == 0 {
			return fmt.Errorf("a shape has no models")
		}
		for i, sm := range n.Models {
			if sm == nil {
				return fmt.Errorf("model %d is null", i)
			}
			if int(sm.Grid) >= len(m.Grids) {
				return fmt.Errorf("model %d refers to grid %d, which is out of range", i, sm.Grid)
			}
			if sm.Frame < 0 || (i > 0 && sm.Frame <= n.Models[i-1].Frame) {
				return fmt.Errorf("the models are not in ascending order of frames from 0")
			}
			if err := checkAttributes(sm.Attributes, reservedShapeModelKeys); err != nil {
				return fmt.Errorf("model %d: %w", i, err)
			}
		}
		if len(n.Frames) != 0 || n.Layer != -1 {
			return fmt.Errorf("a shape has frames or a layer")
		}
	default:
		return fmt.Errorf("unknown node type %d", n.Type)
	}
	return nil
}

// Decode reads and validates the content of a `.voxelmodel` file.
func Decode(data []byte) (m *schemas.VoxelModelT, err error) {
	// NOTE: The Go runtime of FlatBuffers has no verifier and panics on a
	// buffer that is not a voxel model, so the panic becomes an error.
	func() {
		defer func() {
			if r := recover(); r != nil {
				m = nil
				err = fmt.Errorf("not a voxel model file: %v", r)
			}
		}()
		m = schemas.GetRootAsVoxelModel(data, 0).UnPack()
	}()
	if err != nil {
		return nil, err
	}
	if err := Validate(m); err != nil {
		return nil, err
	}
	return m, nil
}

// Encode validates a voxel model and writes the content of a `.voxelmodel`
// file.
func Encode(m *schemas.VoxelModelT) ([]byte, error) {
	if err := Validate(m); err != nil {
		return nil, err
	}
	builder := flatbuffers.NewBuilder(0)
	builder.Finish(m.Pack(builder))
	return builder.FinishedBytes(), nil
}

// ReadFile reads and validates a `.voxelmodel` file.
func ReadFile(path string) (*schemas.VoxelModelT, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("os.ReadFile() failed: %w", err)
	}
	m, err := Decode(data)
	if err != nil {
		return nil, fmt.Errorf("%s: %w", path, err)
	}
	return m, nil
}

// WriteFile validates a voxel model and writes it to a `.voxelmodel` file,
// making the directory of the file when it is missing.
func WriteFile(path string, m *schemas.VoxelModelT) error {
	data, err := Encode(m)
	if err != nil {
		return fmt.Errorf("%s: %w", path, err)
	}
	if err := os.MkdirAll(filepath.Dir(path), fs.ModePerm); err != nil {
		return fmt.Errorf("os.MkdirAll() failed: %w", err)
	}
	if err := os.WriteFile(path, data, fs.ModePerm); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}
	return nil
}
