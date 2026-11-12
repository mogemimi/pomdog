// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"strings"
	"testing"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	"github.com/mogemimi/pomdog/tools/pkg/magicavoxel"
)

// sceneFile returns a file of the recent format, whose node IDs are not in
// the order of the file.
func sceneFile() *magicavoxel.File {
	return &magicavoxel.File{
		Version: magicavoxel.Version200,
		Models: []magicavoxel.Model{
			{SizeX: 2, SizeY: 2, SizeZ: 2, Voxels: []magicavoxel.Voxel{{X: 1, Y: 1, Z: 1, ColorIndex: 3}}},
			{SizeX: 1, SizeY: 1, SizeZ: 1, Voxels: []magicavoxel.Voxel{{ColorIndex: 1}}},
		},
		Nodes: []magicavoxel.Node{
			{Kind: magicavoxel.TransformNode, ID: 0, ChildID: 1, ReservedID: -1, LayerID: -1, Frames: []magicavoxel.Dict{nil}},
			{Kind: magicavoxel.GroupNode, ID: 1, ChildIDs: []int32{7, 2}},
			{Kind: magicavoxel.TransformNode, ID: 7, ChildID: 8, ReservedID: -1, LayerID: 0, Attributes: magicavoxel.Dict{
				{Key: "_name", Value: "head"}, {Key: "_hidden", Value: "1"}, {Key: "_custom", Value: "x"},
			}, Frames: []magicavoxel.Dict{
				{{Key: "_r", Value: "20"}, {Key: "_t", Value: "1 2 -3"}, {Key: "_f", Value: "0"}},
				{{Key: "_t", Value: "0 0 0"}, {Key: "_f", Value: "5"}, {Key: "_x", Value: "y"}},
			}},
			{Kind: magicavoxel.ShapeNode, ID: 8, Models: []magicavoxel.ShapeModel{
				{ModelID: 0, Attributes: magicavoxel.Dict{{Key: "_f", Value: "0"}}},
				{ModelID: 1, Attributes: magicavoxel.Dict{{Key: "_f", Value: "5"}}},
			}},
			{Kind: magicavoxel.TransformNode, ID: 2, ChildID: 3, ReservedID: -1, LayerID: 1, Frames: []magicavoxel.Dict{nil}},
			{Kind: magicavoxel.ShapeNode, ID: 3, Models: []magicavoxel.ShapeModel{{ModelID: 1}}},
		},
		Layers: []magicavoxel.Layer{
			{ID: 0, Attributes: magicavoxel.Dict{{Key: "_name", Value: "base"}, {Key: "_color", Value: "1 2 3"}}, ReservedID: -1},
			{ID: 1, Attributes: magicavoxel.Dict{{Key: "_hidden", Value: "0"}}, ReservedID: -1},
		},
		Materials: []magicavoxel.Material{
			{ID: 3, Properties: magicavoxel.Dict{{Key: "_type", Value: "_emit"}, {Key: "_flux", Value: "2"}}},
		},
		OtherChunks: []magicavoxel.Chunk{
			{ID: "rOBJ"}, {ID: "rOBJ"}, {ID: "rCAM"}, {ID: "XTRA"},
		},
	}
}

func TestConvertScene(t *testing.T) {
	m, err := convert(sceneFile())
	if err != nil {
		t.Fatal(err)
	}

	if len(m.Palette) != 256 || m.Palette[1].R != 255 || m.Palette[0].A != 0 {
		t.Fatal("a file without a palette does not get the default palette")
	}

	// NOTE: The nodes follow the order of their IDs: 0, 1, 2, 3, 7, 8.
	if len(m.Nodes) != 6 {
		t.Fatalf("got %d nodes", len(m.Nodes))
	}
	if got := m.Nodes[1].Children; len(got) != 2 || got[0] != 4 || got[1] != 2 {
		t.Fatalf("group children are %v, want [4 2]", got)
	}
	head := m.Nodes[4]
	if head.Type != schemas.VoxelModelNodeTypeTransform || head.Name != "head" || !head.Hidden || head.Layer != 0 || head.Children[0] != 5 {
		t.Fatalf("unexpected transform %+v", head)
	}
	if len(head.Attributes) != 1 || head.Attributes[0].Key != "_custom" {
		t.Fatalf("unexpected attributes %+v", head.Attributes)
	}
	f0, f1 := head.Frames[0], head.Frames[1]
	if f0.Frame != 0 || f0.Rotation != 20 || f0.Translation == nil || *f0.Translation != (schemas.VoxelModelPoint3T{X: 1, Y: 2, Z: -3}) {
		t.Fatalf("unexpected first frame %+v", f0)
	}
	if f1.Frame != 5 || f1.Rotation != 4 || f1.Translation != nil || len(f1.Attributes) != 1 {
		t.Fatalf("unexpected second frame %+v", f1)
	}
	shape := m.Nodes[5]
	if shape.Type != schemas.VoxelModelNodeTypeShape || shape.Layer != -1 || len(shape.Models) != 2 || shape.Models[1].Grid != 1 || shape.Models[1].Frame != 5 || len(shape.Models[1].Attributes) != 0 {
		t.Fatalf("unexpected shape %+v", shape)
	}
	if m.Nodes[1].Layer != -1 || m.Nodes[2].Layer != 1 {
		t.Fatal("unexpected layers of nodes")
	}

	if len(m.Layers) != 2 || m.Layers[0].Name != "base" || len(m.Layers[0].Attributes) != 1 || m.Layers[1].Hidden {
		t.Fatalf("unexpected layers %+v", m.Layers)
	}
	if len(m.Materials) != 1 || m.Materials[0].Id != 3 || len(m.Materials[0].Properties) != 2 {
		t.Fatalf("unexpected materials %+v", m.Materials)
	}

	notes := strings.Join(m.Notes, "\n")
	for _, want := range []string{`dropped 2 "rOBJ" chunks`, `dropped 1 "rCAM" chunks`, `dropped 1 "XTRA" chunks: an unknown kind`} {
		if !strings.Contains(notes, want) {
			t.Fatalf("notes %q lack %q", notes, want)
		}
	}
}

func TestConvertErrors(t *testing.T) {
	tests := []struct {
		name   string
		modify func(f *magicavoxel.File)
		want   string
	}{
		{"size", func(f *magicavoxel.File) { f.Models[1].SizeX = 257 }, "not between 1 and 256"},
		{"empty size", func(f *magicavoxel.File) { f.Models[1].SizeZ = 0 }, "not between 1 and 256"},
		{"color index", func(f *magicavoxel.File) { f.Models[1].Voxels[0].ColorIndex = 0 }, "color index 0"},
		{"duplicate id", func(f *magicavoxel.File) { f.Nodes[5].ID = 8 }, "two nodes have the ID 8"},
		{"root", func(f *magicavoxel.File) { f.Nodes = f.Nodes[1:] }, "no root node"},
		{"child", func(f *magicavoxel.File) { f.Nodes[4].ChildID = 9 }, "node 9 does not exist"},
		{"group child", func(f *magicavoxel.File) { f.Nodes[1].ChildIDs[0] = 9 }, "node 9 does not exist"},
		{"model", func(f *magicavoxel.File) { f.Nodes[5].Models[0].ModelID = 2 }, "model 2, which does not exist"},
		{"hidden", func(f *magicavoxel.File) { f.Nodes[2].Attributes[1].Value = "yes" }, "invalid boolean"},
		{"rotation", func(f *magicavoxel.File) { f.Nodes[2].Frames[0][0].Value = "3" }, "invalid rotation"},
		{"translation", func(f *magicavoxel.File) { f.Nodes[2].Frames[0][1].Value = "1 2" }, "invalid translation"},
		{"frame", func(f *magicavoxel.File) { f.Nodes[2].Frames[1][1].Value = "0" }, "ascending"},
		{"shape frame", func(f *magicavoxel.File) { f.Nodes[3].Models[1].Attributes[0].Value = "x" }, "invalid integer"},
		{"tree", func(f *magicavoxel.File) { f.Nodes[1].ChildIDs = []int32{7, 7} }, "two parents"},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			f := sceneFile()
			tt.modify(f)
			_, err := convert(f)
			if err == nil || !strings.Contains(err.Error(), tt.want) {
				t.Fatalf("got error %v, want one containing %q", err, tt.want)
			}
		})
	}
}

func TestConvertOldFormat(t *testing.T) {
	palette := magicavoxel.DefaultPalette()
	palette[5] = magicavoxel.Color{R: 1, G: 2, B: 3, A: 4}
	f := &magicavoxel.File{
		Version: magicavoxel.Version150,
		Models: []magicavoxel.Model{
			{SizeX: 3, SizeY: 2, SizeZ: 1, Voxels: []magicavoxel.Voxel{{X: 2, Y: 1, ColorIndex: 5}}},
		},
		Palette:     &palette,
		OtherChunks: []magicavoxel.Chunk{{ID: "MATT"}},
	}
	m, err := convert(f)
	if err != nil {
		t.Fatal(err)
	}
	if len(m.Nodes) != 0 || len(m.Grids) != 1 || m.Grids[0].SizeX != 3 || m.Grids[0].Voxels[0].X != 2 {
		t.Fatalf("unexpected model %+v", m)
	}
	if *m.Palette[5] != (schemas.VoxelModelColorT{R: 1, G: 2, B: 3, A: 4}) {
		t.Fatal("the palette was not kept")
	}
	if len(m.Notes) != 1 || !strings.Contains(m.Notes[0], "MATT") {
		t.Fatalf("unexpected notes %v", m.Notes)
	}
}
