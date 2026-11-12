// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"reflect"
	"testing"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	"github.com/mogemimi/pomdog/tools/pkg/magicavoxel"
	"github.com/mogemimi/pomdog/tools/pkg/voxelmodel"
)

func sampleModel() *schemas.VoxelModelT {
	palette := make([]*schemas.VoxelModelColorT, voxelmodel.PaletteSize)
	for i := range palette {
		palette[i] = &schemas.VoxelModelColorT{R: uint8(i), G: 1, B: 2, A: 255}
	}
	attrs := func(kv ...string) []*schemas.VoxelModelAttributeT {
		var out []*schemas.VoxelModelAttributeT
		for i := 0; i < len(kv); i += 2 {
			out = append(out, &schemas.VoxelModelAttributeT{Key: kv[i], Value: kv[i+1]})
		}
		return out
	}
	return &schemas.VoxelModelT{
		Palette: palette,
		Grids: []*schemas.VoxelModelGridT{
			{SizeX: 1, SizeY: 2, SizeZ: 3, Voxels: []*schemas.VoxelModelVoxelT{{Y: 1, Z: 2, ColorIndex: 9}}},
			{SizeX: 1, SizeY: 1, SizeZ: 1, Voxels: []*schemas.VoxelModelVoxelT{{ColorIndex: 1}}},
		},
		Nodes: []*schemas.VoxelModelNodeT{
			{Type: schemas.VoxelModelNodeTypeTransform, Children: []uint32{1}, Layer: -1, Frames: []*schemas.VoxelModelTransformFrameT{{Rotation: 4}}},
			{Type: schemas.VoxelModelNodeTypeGroup, Children: []uint32{2}, Layer: -1, Attributes: attrs("_g", "1")},
			{Type: schemas.VoxelModelNodeTypeTransform, Name: "arm", Hidden: true, Children: []uint32{3}, Layer: 2, Attributes: attrs("_k", "v"), Frames: []*schemas.VoxelModelTransformFrameT{
				{Rotation: 17, Translation: &schemas.VoxelModelPoint3T{X: 1, Y: -2, Z: 3}},
				{Frame: 4, Rotation: 4, Translation: &schemas.VoxelModelPoint3T{}, Attributes: attrs("_x", "y")},
			}},
			{Type: schemas.VoxelModelNodeTypeShape, Layer: -1, Models: []*schemas.VoxelModelShapeModelT{{Grid: 1}, {Grid: 0, Frame: 4, Attributes: attrs("_m", "n")}}},
		},
		Layers:    []*schemas.VoxelModelLayerT{{Id: 2, Name: "top", Attributes: attrs("_color", "1 2 3")}},
		Materials: []*schemas.VoxelModelMaterialT{{Id: 9, Properties: attrs("_type", "_glass")}},
		Notes:     []string{"not written"},
	}
}

func dict(kv ...string) magicavoxel.Dict {
	var d magicavoxel.Dict
	for i := 0; i < len(kv); i += 2 {
		d = append(d, magicavoxel.DictEntry{Key: kv[i], Value: kv[i+1]})
	}
	return d
}

func TestConvert(t *testing.T) {
	m := sampleModel()
	if err := voxelmodel.Validate(m); err != nil {
		t.Fatal(err)
	}
	f := convert(m)

	if f.Version != magicavoxel.Version150 || len(f.Models) != 2 || f.Models[0].SizeZ != 3 || f.Models[0].Voxels[0] != (magicavoxel.Voxel{Y: 1, Z: 2, ColorIndex: 9}) {
		t.Fatalf("unexpected models %+v", f.Models)
	}
	if f.Palette == nil || f.Palette[7] != (magicavoxel.Color{R: 7, G: 1, B: 2, A: 255}) {
		t.Fatal("unexpected palette")
	}

	want := []magicavoxel.Node{
		{Kind: magicavoxel.TransformNode, ID: 0, ChildID: 1, ReservedID: -1, LayerID: -1, Frames: []magicavoxel.Dict{nil}},
		{Kind: magicavoxel.GroupNode, ID: 1, Attributes: dict("_g", "1"), ChildIDs: []int32{2}},
		{Kind: magicavoxel.TransformNode, ID: 2, Attributes: dict("_name", "arm", "_hidden", "1", "_k", "v"), ChildID: 3, ReservedID: -1, LayerID: 2, Frames: []magicavoxel.Dict{
			dict("_r", "17", "_t", "1 -2 3", "_f", "0"),
			dict("_f", "4", "_x", "y"),
		}},
		{Kind: magicavoxel.ShapeNode, ID: 3, Models: []magicavoxel.ShapeModel{
			{ModelID: 1, Attributes: dict("_f", "0")},
			{ModelID: 0, Attributes: dict("_f", "4", "_m", "n")},
		}},
	}
	if !reflect.DeepEqual(f.Nodes, want) {
		t.Fatalf("unexpected nodes:\n got %+v\nwant %+v", f.Nodes, want)
	}

	wantLayers := []magicavoxel.Layer{{ID: 2, Attributes: dict("_name", "top", "_color", "1 2 3"), ReservedID: -1}}
	if !reflect.DeepEqual(f.Layers, wantLayers) {
		t.Fatalf("unexpected layers %+v", f.Layers)
	}
	wantMaterials := []magicavoxel.Material{{ID: 9, Properties: dict("_type", "_glass")}}
	if !reflect.DeepEqual(f.Materials, wantMaterials) {
		t.Fatalf("unexpected materials %+v", f.Materials)
	}

	data, err := magicavoxel.Encode(f)
	if err != nil {
		t.Fatal(err)
	}
	decoded, err := magicavoxel.Decode(data)
	if err != nil {
		t.Fatal(err)
	}
	if !reflect.DeepEqual(decoded, f) {
		t.Fatal("the written file does not read back")
	}
}

func TestConvertWithoutScene(t *testing.T) {
	m := sampleModel()
	m.Nodes = nil
	data, err := magicavoxel.Encode(convert(m))
	if err != nil {
		t.Fatal(err)
	}
	// NOTE: Two models without a scene graph are the frames of an animation
	// for old releases, which need a PACK chunk.
	if string(data[20:24]) != "PACK" {
		t.Fatalf("the first chunk is %q, not PACK", data[20:24])
	}
}
