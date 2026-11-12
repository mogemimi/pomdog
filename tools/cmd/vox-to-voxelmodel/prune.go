// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"

	schemas "github.com/mogemimi/pomdog/build/schemas"
)

// pruneOptions chooses the data that a voxel model leaves out to get smaller.
type pruneOptions struct {
	// DropUnusedMaterials drops the materials of the colors that no voxel
	// uses. MagicaVoxel writes a material for each of the 256 colors, and
	// opens a file without them with its default material.
	DropUnusedMaterials bool

	// DropUnusedLayers drops the layers that no transform uses. MagicaVoxel
	// writes all of its layers, and opens a file without them with its default
	// layers.
	DropUnusedLayers bool
}

// prune drops the data that the options choose from a voxel model, and adds a
// note for each kind of data that it drops.
func prune(m *schemas.VoxelModelT, opts pruneOptions) {
	if opts.DropUnusedMaterials {
		var used [256]bool
		for _, g := range m.Grids {
			for _, v := range g.Voxels {
				used[v.ColorIndex] = true
			}
		}
		var kept []*schemas.VoxelModelMaterialT
		for _, mat := range m.Materials {
			if mat.Id >= 0 && mat.Id < int32(len(used)) && used[mat.Id] {
				kept = append(kept, mat)
			}
		}
		if dropped := len(m.Materials) - len(kept); dropped > 0 {
			m.Notes = append(m.Notes, fmt.Sprintf("dropped %d materials of colors that no voxel uses", dropped))
		}
		m.Materials = kept
	}

	if opts.DropUnusedLayers {
		used := map[int32]bool{}
		for _, n := range m.Nodes {
			if n.Type == schemas.VoxelModelNodeTypeTransform {
				used[n.Layer] = true
			}
		}
		var kept []*schemas.VoxelModelLayerT
		for _, l := range m.Layers {
			if used[l.Id] {
				kept = append(kept, l)
			}
		}
		if dropped := len(m.Layers) - len(kept); dropped > 0 {
			m.Notes = append(m.Notes, fmt.Sprintf("dropped %d layers that no transform uses", dropped))
		}
		m.Layers = kept
	}
}
