// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"

	schemas "github.com/mogemimi/pomdog/build/schemas"
)

// mesher chooses which faces of the voxels become quads.
type mesher int

const (
	// naiveMesher makes a quad for every face of every voxel.
	naiveMesher mesher = iota

	// culledMesher makes quads only for the faces that no voxel covers.
	culledMesher

	// greedyMesher makes quads for the faces that no voxel covers, merging
	// faces of the same color on the same plane into rectangles.
	greedyMesher
)

func parseMesher(s string) (mesher, error) {
	switch s {
	case "naive":
		return naiveMesher, nil
	case "culled":
		return culledMesher, nil
	case "greedy":
		return greedyMesher, nil
	}
	return 0, fmt.Errorf("unknown mesher %q; it must be naive, culled, or greedy", s)
}

// quad is a rectangle on a face of the grid, in voxel coordinates.
// The quad lies on the plane where the coordinate on `axis` equals `depth`,
// faces the positive direction of the axis when `positive` is true, and
// covers [u0, u1) x [v0, v1) of the two other axes, which follow `axis` in
// the order x, y, z, x, y.
type quad struct {
	axis       int
	positive   bool
	depth      int
	u0, v0     int
	u1, v1     int
	colorIndex uint8
}

// voxelGrid is a dense copy of a grid, which tells the color index of each
// cell, or 0 for an empty cell.
type voxelGrid struct {
	size  [3]int
	cells []uint8
}

func newVoxelGrid(g *schemas.VoxelModelGridT) *voxelGrid {
	vg := &voxelGrid{size: [3]int{int(g.SizeX), int(g.SizeY), int(g.SizeZ)}}
	vg.cells = make([]uint8, vg.size[0]*vg.size[1]*vg.size[2])
	for _, v := range g.Voxels {
		vg.cells[vg.cellIndex([3]int{int(v.X), int(v.Y), int(v.Z)})] = v.ColorIndex
	}
	return vg
}

func (vg *voxelGrid) cellIndex(p [3]int) int {
	return (p[2]*vg.size[1]+p[1])*vg.size[0] + p[0]
}

// at returns the color index at p, or 0 outside the grid.
func (vg *voxelGrid) at(p [3]int) uint8 {
	for i := range 3 {
		if p[i] < 0 || p[i] >= vg.size[i] {
			return 0
		}
	}
	return vg.cells[vg.cellIndex(p)]
}

// makeQuads makes the quads of a grid, in the order of the axes x, y, z, the
// positive direction before the negative one, and then the depth, v, and u.
func makeQuads(vg *voxelGrid, m mesher) []quad {
	var quads []quad
	for axis := range 3 {
		u := (axis + 1) % 3
		v := (axis + 2) % 3
		for _, positive := range []bool{true, false} {
			step := -1
			if positive {
				step = 1
			}
			// NOTE: mask holds the color of each visible face of a slice, or 0.
			mask := make([]uint8, vg.size[u]*vg.size[v])
			for d := range vg.size[axis] {
				for j := range vg.size[v] {
					for i := range vg.size[u] {
						var p [3]int
						p[axis], p[u], p[v] = d, i, j
						c := vg.at(p)
						if c != 0 && m != naiveMesher {
							q := p
							q[axis] += step
							if vg.at(q) != 0 {
								c = 0
							}
						}
						mask[j*vg.size[u]+i] = c
					}
				}

				depth := d
				if positive {
					depth = d + 1
				}
				if m == greedyMesher {
					quads = appendGreedyQuads(quads, mask, vg.size[u], vg.size[v], axis, positive, depth)
					continue
				}
				for j := range vg.size[v] {
					for i := range vg.size[u] {
						if c := mask[j*vg.size[u]+i]; c != 0 {
							quads = append(quads, quad{axis: axis, positive: positive, depth: depth, u0: i, v0: j, u1: i + 1, v1: j + 1, colorIndex: c})
						}
					}
				}
			}
		}
	}
	return quads
}

// appendGreedyQuads covers the faces of a mask with rectangles of one color.
// Each rectangle starts at the first uncovered face in the order of v and u,
// grows along u while the color continues, and then grows along v while the
// whole next row matches.
func appendGreedyQuads(quads []quad, mask []uint8, width, height, axis int, positive bool, depth int) []quad {
	for j := range height {
		for i := 0; i < width; {
			c := mask[j*width+i]
			if c == 0 {
				i++
				continue
			}
			w := 1
			for i+w < width && mask[j*width+i+w] == c {
				w++
			}
			h := 1
		grow:
			for j+h < height {
				for k := range w {
					if mask[(j+h)*width+i+k] != c {
						break grow
					}
				}
				h++
			}
			for y := j; y < j+h; y++ {
				for x := i; x < i+w; x++ {
					mask[y*width+x] = 0
				}
			}
			quads = append(quads, quad{axis: axis, positive: positive, depth: depth, u0: i, v0: j, u1: i + w, v1: j + h, colorIndex: c})
			i += w
		}
	}
	return quads
}

// corners returns the corners of a quad in voxel coordinates, in
// counterclockwise order seen from the side that the quad faces.
func (q quad) corners() [4][3]int {
	u := (q.axis + 1) % 3
	v := (q.axis + 2) % 3
	corner := func(i, j int) [3]int {
		var p [3]int
		p[q.axis], p[u], p[v] = q.depth, i, j
		return p
	}
	// NOTE: The u and v axes follow the axis in cyclic order, so u x v points
	// along the positive direction of the axis.
	if q.positive {
		return [4][3]int{corner(q.u0, q.v0), corner(q.u1, q.v0), corner(q.u1, q.v1), corner(q.u0, q.v1)}
	}
	return [4][3]int{corner(q.u0, q.v0), corner(q.u0, q.v1), corner(q.u1, q.v1), corner(q.u1, q.v0)}
}
