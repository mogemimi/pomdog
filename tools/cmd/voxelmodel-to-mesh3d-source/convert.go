// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"math"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	"github.com/mogemimi/pomdog/tools/pkg/magicavoxel"
)

// toYUp converts a vector of MagicaVoxel, whose z axis points up, to the
// coordinate system of glTF, whose y axis points up. Both are right-handed,
// and one voxel is one unit.
func toYUp(v [3]float32) [3]float32 {
	z := -v[1]
	if z == 0 {
		// NOTE: Negating zero gives -0, whose bytes differ from 0 and would keep
		// equal vertices from merging.
		z = 0
	}
	return [3]float32{v[0], v[2], z}
}

// meshName names the mesh of a grid.
func meshName(gridIndex int) string {
	return fmt.Sprintf("model%d", gridIndex)
}

// convert turns a validated voxel model into a mesh source with a mesh for
// each grid. The vertices of a mesh are relative to the center of its grid,
// which MagicaVoxel places at half the size of the grid, rounded down.
func convert(m *schemas.VoxelModelT, ms mesher) *schemas.Mesh3DSourceT {
	source := &schemas.Mesh3DSourceT{
		Generator:    "voxelmodel-to-mesh3d-source",
		SourceFormat: "Pomdog voxel model",
		Scene:        -1,
	}
	source.Notes = append(source.Notes, m.Notes...)

	for i, g := range m.Grids {
		source.Meshes = append(source.Meshes, convertGrid(meshName(i), g, m.Palette, ms))
	}

	if len(m.Nodes) > 0 {
		var notes []string
		source.Nodes, notes = convertNodes(m)
		source.Scenes = []*schemas.Mesh3DSourceSceneT{{Nodes: []uint32{0}}}
		source.Scene = 0
		source.Notes = append(source.Notes, notes...)
	}

	if len(m.Materials) > 0 {
		source.Notes = append(source.Notes, fmt.Sprintf("dropped %d MagicaVoxel materials", len(m.Materials)))
	}
	return source
}

func convertGrid(name string, g *schemas.VoxelModelGridT, palette []*schemas.VoxelModelColorT, ms mesher) *schemas.Mesh3DSourceMeshT {
	mesh := &schemas.Mesh3DSourceMeshT{Name: name}
	quads := makeQuads(newVoxelGrid(g), ms)
	if len(quads) == 0 {
		return mesh
	}

	pivot := [3]int{int(g.SizeX) / 2, int(g.SizeY) / 2, int(g.SizeZ) / 2}
	var positions, normals, colors []float32
	var indices []uint32
	for _, q := range quads {
		var normal [3]float32
		normal[q.axis] = -1
		if q.positive {
			normal[q.axis] = 1
		}
		normal = toYUp(normal)
		c := palette[q.colorIndex]
		color := [4]float32{float32(c.R) / 255, float32(c.G) / 255, float32(c.B) / 255, float32(c.A) / 255}

		base := uint32(len(positions) / 3)
		for _, corner := range q.corners() {
			p := toYUp([3]float32{
				float32(corner[0] - pivot[0]),
				float32(corner[1] - pivot[1]),
				float32(corner[2] - pivot[2]),
			})
			positions = append(positions, p[:]...)
			normals = append(normals, normal[:]...)
			colors = append(colors, color[:]...)
		}
		indices = append(indices, base, base+1, base+2, base, base+2, base+3)
	}

	mesh.Primitives = []*schemas.Mesh3DSourcePrimitiveT{{
		// NOTE: The attributes are sorted by semantic.
		Attributes: []*schemas.Mesh3DSourceAttributeT{
			{Semantic: "COLOR_0", ComponentCount: 4, Values: colors, SourceComponentType: schemas.Mesh3DSourceComponentTypeUInt8, SourceNormalized: true},
			{Semantic: "NORMAL", ComponentCount: 3, Values: normals, SourceComponentType: schemas.Mesh3DSourceComponentTypeFloat32},
			{Semantic: "POSITION", ComponentCount: 3, Values: positions, SourceComponentType: schemas.Mesh3DSourceComponentTypeFloat32},
		},
		Indices:     indices,
		VertexCount: uint32(len(positions) / 3),
		Material:    -1,
		Topology:    schemas.Mesh3DSourceTopologyTriangles,
	}}
	return mesh
}

// convertNodes turns the scene graph into nodes of a mesh source. Each
// transform becomes a node: a transform of a group gets the transforms of
// the group as children, and a transform of a shape gets the mesh of its
// grid. The first keyframe of a transform and the first grid of a shape
// place the node.
func convertNodes(m *schemas.VoxelModelT) ([]*schemas.Mesh3DSourceNodeT, []string) {
	var nodes []*schemas.Mesh3DSourceNodeT
	hidden := 0
	animated := 0

	var visit func(transformIndex uint32) uint32
	visit = func(transformIndex uint32) uint32 {
		t := m.Nodes[transformIndex]
		child := m.Nodes[t.Children[0]]
		if t.Hidden || child.Hidden {
			hidden++
		}
		if len(t.Frames) > 1 || len(child.Models) > 1 {
			animated++
		}

		node := &schemas.Mesh3DSourceNodeT{Name: t.Name, Mesh: -1}
		node.Translation, node.Rotation, node.Scale = convertFrame(t.Frames[0])
		index := uint32(len(nodes))
		nodes = append(nodes, node)

		switch child.Type {
		case schemas.VoxelModelNodeTypeGroup:
			for _, c := range child.Children {
				node.Children = append(node.Children, visit(c))
			}
		case schemas.VoxelModelNodeTypeShape:
			node.Mesh = int32(child.Models[0].Grid)
		}
		return index
	}
	visit(0)

	var notes []string
	if hidden > 0 {
		notes = append(notes, fmt.Sprintf("%d nodes are hidden in MagicaVoxel, which a mesh source cannot tell", hidden))
	}
	if animated > 0 {
		notes = append(notes, fmt.Sprintf("%d nodes are animated; only their first frames were converted", animated))
	}
	return nodes, notes
}

// convertFrame turns a keyframe of a transform into a translation, a
// rotation, and a scale in the coordinate system of glTF. A rotation of
// MagicaVoxel can mirror, which a quaternion cannot; the scale then flips
// the z axis, and the rotation does the rest.
func convertFrame(frame *schemas.VoxelModelTransformFrameT) (*schemas.Mesh3DSourceVector3T, *schemas.Mesh3DSourceVector4T, *schemas.Mesh3DSourceVector3T) {
	var t [3]float32
	if frame.Translation != nil {
		t = [3]float32{float32(frame.Translation.X), float32(frame.Translation.Y), float32(frame.Translation.Z)}
	}
	t = toYUp(t)
	translation := &schemas.Mesh3DSourceVector3T{X: t[0], Y: t[1], Z: t[2]}

	// NOTE: The voxel model is validated, so the rotation is valid.
	r, _ := magicavoxel.Rotation(frame.Rotation).Matrix()

	// NOTE: toYUp is the matrix c, so the rotation in glTF is c * r * c^T.
	c := [3][3]int32{{1, 0, 0}, {0, 0, 1}, {0, -1, 0}}
	var cr, rot [3][3]int32
	for i := range 3 {
		for j := range 3 {
			for k := range 3 {
				cr[i][j] += c[i][k] * r[k][j]
			}
		}
	}
	for i := range 3 {
		for j := range 3 {
			for k := range 3 {
				rot[i][j] += cr[i][k] * c[j][k]
			}
		}
	}

	scale := &schemas.Mesh3DSourceVector3T{X: 1, Y: 1, Z: 1}
	if determinant(rot) < 0 {
		scale.Z = -1
		for i := range 3 {
			rot[i][2] = -rot[i][2]
		}
	}
	return translation, quaternionFromMatrix(rot), scale
}

func determinant(m [3][3]int32) int32 {
	return m[0][0]*(m[1][1]*m[2][2]-m[1][2]*m[2][1]) -
		m[0][1]*(m[1][0]*m[2][2]-m[1][2]*m[2][0]) +
		m[0][2]*(m[1][0]*m[2][1]-m[1][1]*m[2][0])
}

// quaternionFromMatrix returns the unit quaternion (x, y, z, w) of a rotation
// matrix that multiplies column vectors.
func quaternionFromMatrix(mi [3][3]int32) *schemas.Mesh3DSourceVector4T {
	var m [3][3]float64
	for i := range 3 {
		for j := range 3 {
			m[i][j] = float64(mi[i][j])
		}
	}
	var x, y, z, w float64
	trace := m[0][0] + m[1][1] + m[2][2]
	switch {
	case trace > 0:
		s := math.Sqrt(trace+1) * 2
		w = s / 4
		x = (m[2][1] - m[1][2]) / s
		y = (m[0][2] - m[2][0]) / s
		z = (m[1][0] - m[0][1]) / s
	case m[0][0] > m[1][1] && m[0][0] > m[2][2]:
		s := math.Sqrt(1+m[0][0]-m[1][1]-m[2][2]) * 2
		w = (m[2][1] - m[1][2]) / s
		x = s / 4
		y = (m[0][1] + m[1][0]) / s
		z = (m[0][2] + m[2][0]) / s
	case m[1][1] > m[2][2]:
		s := math.Sqrt(1+m[1][1]-m[0][0]-m[2][2]) * 2
		w = (m[0][2] - m[2][0]) / s
		x = (m[0][1] + m[1][0]) / s
		y = s / 4
		z = (m[1][2] + m[2][1]) / s
	default:
		s := math.Sqrt(1+m[2][2]-m[0][0]-m[1][1]) * 2
		w = (m[1][0] - m[0][1]) / s
		x = (m[0][2] + m[2][0]) / s
		y = (m[1][2] + m[2][1]) / s
		z = s / 4
	}
	return &schemas.Mesh3DSourceVector4T{X: float32(x), Y: float32(y), Z: float32(z), W: float32(w)}
}
