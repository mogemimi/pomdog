// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"strings"
	"testing"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	"github.com/mogemimi/pomdog/tools/pkg/magicavoxel"
	"github.com/mogemimi/pomdog/tools/pkg/voxelmodel"
)

func materialIDs(m *schemas.VoxelModelT) []int32 {
	var ids []int32
	for _, mat := range m.Materials {
		ids = append(ids, mat.Id)
	}
	return ids
}

func layerIDs(m *schemas.VoxelModelT) []int32 {
	var ids []int32
	for _, l := range m.Layers {
		ids = append(ids, l.Id)
	}
	return ids
}

// pruneFile returns sceneFile() with a material for each of the 256 colors and
// 16 layers, as recent releases of MagicaVoxel write. Its voxels use the
// colors 1 and 3, and its transforms the layers -1, 0, and 1.
func pruneFile() *magicavoxel.File {
	f := sceneFile()
	f.Materials = nil
	for i := range 256 {
		f.Materials = append(f.Materials, magicavoxel.Material{ID: int32(i), Properties: magicavoxel.Dict{{Key: "_rough", Value: "0.1"}}})
	}
	f.Layers = nil
	for i := range 16 {
		f.Layers = append(f.Layers, magicavoxel.Layer{ID: int32(i), ReservedID: -1})
	}
	return f
}

func TestPrune(t *testing.T) {
	tests := []struct {
		name      string
		opts      pruneOptions
		materials []int32
		layers    int
		notes     []string
	}{
		{"nothing", pruneOptions{}, nil, 16, nil},
		{"materials", pruneOptions{DropUnusedMaterials: true}, []int32{1, 3}, 16, []string{"dropped 254 materials"}},
		{"layers", pruneOptions{DropUnusedLayers: true}, nil, 2, []string{"dropped 14 layers"}},
		{"both", pruneOptions{DropUnusedMaterials: true, DropUnusedLayers: true}, []int32{1, 3}, 2, []string{"dropped 254 materials", "dropped 14 layers"}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			m, err := convert(pruneFile())
			if err != nil {
				t.Fatal(err)
			}
			prune(m, tt.opts)
			if err := voxelmodel.Validate(m); err != nil {
				t.Fatal(err)
			}

			ids := materialIDs(m)
			if tt.materials == nil {
				if len(ids) != 256 {
					t.Fatalf("kept %d materials, want 256", len(ids))
				}
			} else if len(ids) != len(tt.materials) || ids[0] != tt.materials[0] || ids[1] != tt.materials[1] {
				t.Fatalf("kept materials %v, want %v", ids, tt.materials)
			}

			layers := layerIDs(m)
			if len(layers) != tt.layers {
				t.Fatalf("kept layers %v, want %d of them", layers, tt.layers)
			}
			if tt.layers == 2 && (layers[0] != 0 || layers[1] != 1) {
				t.Fatalf("kept layers %v, want [0 1]", layers)
			}

			notes := strings.Join(m.Notes, "\n")
			for _, want := range tt.notes {
				if !strings.Contains(notes, want) {
					t.Fatalf("notes %q lack %q", notes, want)
				}
			}
			if len(tt.notes) == 0 && (strings.Contains(notes, "materials of colors") || strings.Contains(notes, "layers that")) {
				t.Fatalf("notes %q tell about pruning that did not happen", notes)
			}
		})
	}
}

func TestPruneWithoutSceneGraph(t *testing.T) {
	f := pruneFile()
	f.Nodes = nil
	m, err := convert(f)
	if err != nil {
		t.Fatal(err)
	}
	prune(m, pruneOptions{DropUnusedLayers: true})
	if len(m.Layers) != 0 {
		t.Fatalf("kept %d layers of a file without transforms", len(m.Layers))
	}
}

func TestDescribeMetaChunk(t *testing.T) {
	notes := describeDroppedChunks([]magicavoxel.Chunk{{ID: "META"}})
	if len(notes) != 1 || !strings.Contains(notes[0], "settings of the document") {
		t.Fatalf("unexpected notes %v", notes)
	}
}
