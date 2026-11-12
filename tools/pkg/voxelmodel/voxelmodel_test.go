package voxelmodel

import (
	"encoding/json"
	"strings"
	"testing"

	schemas "github.com/mogemimi/pomdog/build/schemas"
)

func palette() []*schemas.VoxelModelColorT {
	p := make([]*schemas.VoxelModelColorT, PaletteSize)
	for i := range p {
		p[i] = &schemas.VoxelModelColorT{R: uint8(i), A: 255}
	}
	return p
}

// sampleModel returns a valid model: a root transform, a group with two
// transforms, and a shape under each of them.
func sampleModel() *schemas.VoxelModelT {
	return &schemas.VoxelModelT{
		Palette: palette(),
		Grids: []*schemas.VoxelModelGridT{
			{SizeX: 2, SizeY: 1, SizeZ: 1, Voxels: []*schemas.VoxelModelVoxelT{{X: 0, ColorIndex: 1}, {X: 1, ColorIndex: 2}}},
		},
		Nodes: []*schemas.VoxelModelNodeT{
			{Type: schemas.VoxelModelNodeTypeTransform, Children: []uint32{1}, Layer: -1, Frames: []*schemas.VoxelModelTransformFrameT{{Rotation: 4}}},
			{Type: schemas.VoxelModelNodeTypeGroup, Children: []uint32{2, 4}, Layer: -1},
			{Type: schemas.VoxelModelNodeTypeTransform, Name: "a", Children: []uint32{3}, Layer: 0, Frames: []*schemas.VoxelModelTransformFrameT{{Rotation: 4}, {Frame: 2, Rotation: 17}}},
			{Type: schemas.VoxelModelNodeTypeShape, Layer: -1, Models: []*schemas.VoxelModelShapeModelT{{Grid: 0}}},
			{Type: schemas.VoxelModelNodeTypeTransform, Children: []uint32{5}, Layer: 0, Frames: []*schemas.VoxelModelTransformFrameT{{Rotation: 4}}},
			{Type: schemas.VoxelModelNodeTypeShape, Layer: -1, Models: []*schemas.VoxelModelShapeModelT{{Grid: 0}}},
		},
		Layers:    []*schemas.VoxelModelLayerT{{Id: 0, Name: "layer"}},
		Materials: []*schemas.VoxelModelMaterialT{{Id: 1, Properties: []*schemas.VoxelModelAttributeT{{Key: "_type", Value: "_metal"}}}},
	}
}

func TestEncodeDecode(t *testing.T) {
	m := sampleModel()
	data, err := Encode(m)
	if err != nil {
		t.Fatal(err)
	}
	decoded, err := Decode(data)
	if err != nil {
		t.Fatal(err)
	}
	// NOTE: UnPack() turns absent vectors into empty slices, which Pack()
	// writes as empty vectors, so the comparison goes through JSON.
	again, err := Encode(decoded)
	if err != nil {
		t.Fatal(err)
	}
	decodedAgain, err := Decode(again)
	if err != nil {
		t.Fatal(err)
	}
	a, _ := json.Marshal(decoded)
	b, _ := json.Marshal(decodedAgain)
	if string(a) != string(b) {
		t.Fatal("encoding the decoded model changed it")
	}
	if decoded.Nodes[2].Name != "a" || decoded.Nodes[2].Frames[1].Rotation != 17 || decoded.Nodes[1].Layer != -1 {
		t.Fatal("the decoded model lost data")
	}
	if _, err := Decode([]byte{1, 2, 3}); err == nil {
		t.Fatal("decoding garbage succeeded")
	}
}

func TestValidate(t *testing.T) {
	tests := []struct {
		name   string
		modify func(m *schemas.VoxelModelT)
		want   string
	}{
		{"palette size", func(m *schemas.VoxelModelT) { m.Palette = m.Palette[:255] }, "255 colors"},
		{"grid size zero", func(m *schemas.VoxelModelT) { m.Grids[0].SizeY = 0 }, "not between 1 and 256"},
		{"grid size large", func(m *schemas.VoxelModelT) { m.Grids[0].SizeZ = 257 }, "not between 1 and 256"},
		{"voxel outside", func(m *schemas.VoxelModelT) { m.Grids[0].Voxels[1].X = 2 }, "outside the grid"},
		{"color zero", func(m *schemas.VoxelModelT) { m.Grids[0].Voxels[0].ColorIndex = 0 }, "color index 0"},
		{"duplicate voxel", func(m *schemas.VoxelModelT) { m.Grids[0].Voxels[1].X = 0 }, "two voxels"},
		{"root type", func(m *schemas.VoxelModelT) { m.Nodes = m.Nodes[3:4] }, "root node is not a transform"},
		{"transform children", func(m *schemas.VoxelModelT) { m.Nodes[2].Children = []uint32{3, 5} }, "has 2 children"},
		{"transform child type", func(m *schemas.VoxelModelT) { m.Nodes[0].Children = []uint32{2} }, "group or a shape"},
		{"group child type", func(m *schemas.VoxelModelT) { m.Nodes[1].Children = []uint32{2, 3} }, "must be transforms"},
		{"child range", func(m *schemas.VoxelModelT) { m.Nodes[1].Children = []uint32{2, 9} }, "out of range"},
		{"root child", func(m *schemas.VoxelModelT) { m.Nodes[1].Children = []uint32{2, 0} }, "root node cannot be a child"},
		{"two parents", func(m *schemas.VoxelModelT) { m.Nodes[4].Children = []uint32{3} }, "two parents"},
		{"unreachable", func(m *schemas.VoxelModelT) { m.Nodes[1].Children = []uint32{2} }, "not reachable"},
		{"no frames", func(m *schemas.VoxelModelT) { m.Nodes[0].Frames = nil }, "no frames"},
		{"frame order", func(m *schemas.VoxelModelT) { m.Nodes[2].Frames[1].Frame = 0 }, "ascending"},
		{"rotation", func(m *schemas.VoxelModelT) { m.Nodes[0].Frames[0].Rotation = 0 }, "invalid rotation"},
		{"reserved frame key", func(m *schemas.VoxelModelT) {
			m.Nodes[0].Frames[0].Attributes = []*schemas.VoxelModelAttributeT{{Key: "_t", Value: "0 0 0"}}
		}, "own field"},
		{"reserved node key", func(m *schemas.VoxelModelT) {
			m.Nodes[0].Attributes = []*schemas.VoxelModelAttributeT{{Key: "_name", Value: "x"}}
		}, "own field"},
		{"empty key", func(m *schemas.VoxelModelT) {
			m.Materials[0].Properties = []*schemas.VoxelModelAttributeT{{Value: "x"}}
		}, "empty key"},
		{"group layer", func(m *schemas.VoxelModelT) { m.Nodes[1].Layer = 0 }, "a group has"},
		{"shape grid", func(m *schemas.VoxelModelT) { m.Nodes[3].Models[0].Grid = 1 }, "out of range"},
		{"shape models", func(m *schemas.VoxelModelT) { m.Nodes[3].Models = nil }, "no models"},
		{"shape children", func(m *schemas.VoxelModelT) { m.Nodes[3].Children = []uint32{5} }, "a shape has children"},
		{"node type", func(m *schemas.VoxelModelT) { m.Nodes[5].Type = 3 }, "unknown node type"},
		{"layer id", func(m *schemas.VoxelModelT) { m.Layers = append(m.Layers, &schemas.VoxelModelLayerT{Id: 0}) }, "two layers"},
		{"material id", func(m *schemas.VoxelModelT) {
			m.Materials = append(m.Materials, &schemas.VoxelModelMaterialT{Id: 1})
		}, "two materials"},
	}
	if err := Validate(sampleModel()); err != nil {
		t.Fatal(err)
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			m := sampleModel()
			tt.modify(m)
			err := Validate(m)
			if err == nil || !strings.Contains(err.Error(), tt.want) {
				t.Fatalf("got error %v, want one containing %q", err, tt.want)
			}
		})
	}
}
