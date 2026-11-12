// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"math"
	"strings"
	"testing"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	"github.com/mogemimi/pomdog/tools/pkg/magicavoxel"
	"github.com/mogemimi/pomdog/tools/pkg/voxelmodel"
)

func testPalette() []*schemas.VoxelModelColorT {
	p := make([]*schemas.VoxelModelColorT, voxelmodel.PaletteSize)
	for i := range p {
		p[i] = &schemas.VoxelModelColorT{R: uint8(i), G: 51, B: 255, A: 255}
	}
	return p
}

func gridOf(size [3]uint32, voxels ...[4]uint8) *schemas.VoxelModelGridT {
	g := &schemas.VoxelModelGridT{SizeX: size[0], SizeY: size[1], SizeZ: size[2]}
	for _, v := range voxels {
		g.Voxels = append(g.Voxels, &schemas.VoxelModelVoxelT{X: v[0], Y: v[1], Z: v[2], ColorIndex: v[3]})
	}
	return g
}

func quadCount(g *schemas.VoxelModelGridT, m mesher) int {
	return len(makeQuads(newVoxelGrid(g), m))
}

func TestQuadCounts(t *testing.T) {
	tests := []struct {
		name                  string
		grid                  *schemas.VoxelModelGridT
		naive, culled, greedy int
	}{
		{"one voxel", gridOf([3]uint32{1, 1, 1}, [4]uint8{0, 0, 0, 1}), 6, 6, 6},
		{"two voxels of one color", gridOf([3]uint32{2, 1, 1}, [4]uint8{0, 0, 0, 1}, [4]uint8{1, 0, 0, 1}), 12, 10, 6},
		{"two voxels of two colors", gridOf([3]uint32{2, 1, 1}, [4]uint8{0, 0, 0, 1}, [4]uint8{1, 0, 0, 2}), 12, 10, 10},
		{"plate of one color", gridOf([3]uint32{2, 2, 1}, [4]uint8{0, 0, 0, 1}, [4]uint8{1, 0, 0, 1}, [4]uint8{0, 1, 0, 1}, [4]uint8{1, 1, 0, 1}), 24, 16, 6},
		// NOTE: The top and bottom of the L shape need two rectangles each.
		{"L shape", gridOf([3]uint32{2, 2, 1}, [4]uint8{0, 0, 0, 1}, [4]uint8{1, 0, 0, 1}, [4]uint8{0, 1, 0, 1}), 18, 14, 10},
		{"empty", gridOf([3]uint32{3, 3, 3}), 0, 0, 0},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			for _, c := range []struct {
				m    mesher
				want int
			}{{naiveMesher, tt.naive}, {culledMesher, tt.culled}, {greedyMesher, tt.greedy}} {
				if got := quadCount(tt.grid, c.m); got != c.want {
					t.Errorf("mesher %d makes %d quads, want %d", c.m, got, c.want)
				}
			}
		})
	}
}

func sub(a, b [3]float32) [3]float32 {
	return [3]float32{a[0] - b[0], a[1] - b[1], a[2] - b[2]}
}

func cross(a, b [3]float32) [3]float32 {
	return [3]float32{a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]}
}

func dot(a, b [3]float32) float32 {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2]
}

func TestMeshWindingAndPlacement(t *testing.T) {
	// NOTE: A grid of 3x3x3 with voxels at the center and on a corner.
	g := gridOf([3]uint32{3, 2, 3}, [4]uint8{1, 1, 1, 7}, [4]uint8{0, 0, 0, 7}, [4]uint8{1, 0, 1, 8})
	for _, ms := range []mesher{naiveMesher, culledMesher, greedyMesher} {
		mesh := convertGrid("model0", g, testPalette(), ms)
		prim := mesh.Primitives[0]
		colors, normals, positions := prim.Attributes[0], prim.Attributes[1], prim.Attributes[2]
		if colors.Semantic != "COLOR_0" || normals.Semantic != "NORMAL" || positions.Semantic != "POSITION" {
			t.Fatal("attributes are not sorted by semantic")
		}
		vertex := func(values []float32, i uint32) [3]float32 {
			return [3]float32{values[3*i], values[3*i+1], values[3*i+2]}
		}
		for i := 0; i < len(prim.Indices); i += 3 {
			a := vertex(positions.Values, prim.Indices[i])
			b := vertex(positions.Values, prim.Indices[i+1])
			c := vertex(positions.Values, prim.Indices[i+2])
			n := vertex(normals.Values, prim.Indices[i])
			if dot(cross(sub(b, a), sub(c, a)), n) <= 0 {
				t.Fatalf("mesher %d: triangle %d is not counterclockwise around its normal", ms, i/3)
			}
		}

		// NOTE: The pivot is (1, 1, 1), so the voxels span x in [-1, 1], y
		// (from z) in [-1, 1], and z (from -y) in [-1, 1].
		for i := uint32(0); i < prim.VertexCount; i++ {
			p := vertex(positions.Values, i)
			for _, c := range p {
				if c < -1 || c > 1 {
					t.Fatalf("mesher %d: vertex %v is outside the voxels", ms, p)
				}
			}
		}
		if colors.Values[0] != 7.0/255 && colors.Values[0] != 8.0/255 {
			t.Fatalf("unexpected red %v", colors.Values[0])
		}
		if colors.Values[1] != 51.0/255 || colors.Values[2] != 1 || colors.Values[3] != 1 {
			t.Fatalf("unexpected color %v", colors.Values[:4])
		}
	}
}

func TestFaceDirections(t *testing.T) {
	// NOTE: One voxel in a 1x1x1 grid spans [0, 1] on each axis of voxel
	// space, which becomes x in [0, 1], y in [0, 1], and z in [-1, 0].
	mesh := convertGrid("model0", gridOf([3]uint32{1, 1, 1}, [4]uint8{0, 0, 0, 1}), testPalette(), naiveMesher)
	prim := mesh.Primitives[0]
	normals, positions := prim.Attributes[1].Values, prim.Attributes[2].Values
	center := [3]float32{0.5, 0.5, -0.5}
	up := 0
	for q := 0; q < 6; q++ {
		var faceCenter [3]float32
		for k := range 4 {
			for j := range 3 {
				faceCenter[j] += positions[3*(4*q+k)+j] / 4
			}
		}
		n := [3]float32{normals[12*q], normals[12*q+1], normals[12*q+2]}
		if dot(sub(faceCenter, center), n) <= 0 {
			t.Fatalf("quad %d faces inward", q)
		}
		if n == [3]float32{0, 1, 0} {
			up++
			if faceCenter[1] != 1 {
				t.Fatalf("the top face is at y = %v", faceCenter[1])
			}
		}
	}
	if up != 1 {
		t.Fatal("no face points up along y")
	}
}

func rotateByQuaternion(q *schemas.Mesh3DSourceVector4T, v [3]float64) [3]float64 {
	// NOTE: v' = v + 2w (u x v) + 2 u x (u x v), where u = (x, y, z).
	u := [3]float64{float64(q.X), float64(q.Y), float64(q.Z)}
	w := float64(q.W)
	c := func(a, b [3]float64) [3]float64 {
		return [3]float64{a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]}
	}
	uv := c(u, v)
	uuv := c(u, uv)
	return [3]float64{v[0] + 2*(w*uv[0]+uuv[0]), v[1] + 2*(w*uv[1]+uuv[1]), v[2] + 2*(w*uv[2]+uuv[2])}
}

func TestConvertFrameRotations(t *testing.T) {
	count := 0
	for b := range 256 {
		r := magicavoxel.Rotation(b)
		if !r.IsValid() {
			continue
		}
		count++
		m, _ := r.Matrix()
		_, q, s := convertFrame(&schemas.VoxelModelTransformFrameT{Rotation: uint8(r)})
		if math.Abs(float64(q.X*q.X+q.Y*q.Y+q.Z*q.Z+q.W*q.W)-1) > 1e-6 {
			t.Fatalf("rotation %d: quaternion %+v is not a unit", b, q)
		}
		// NOTE: A vector of glTF space maps back to MagicaVoxel space, rotates
		// there, and comes back; the node must agree.
		for axis := range 3 {
			var v [3]float32
			v[axis] = 1
			back := [3]float32{v[0], -v[2], v[1]}
			var rotated [3]float32
			for i := range 3 {
				for j := range 3 {
					rotated[i] += float32(m[i][j]) * back[j]
				}
			}
			want := toYUp(rotated)
			scaled := [3]float64{float64(v[0] * s.X), float64(v[1] * s.Y), float64(v[2] * s.Z)}
			got := rotateByQuaternion(q, scaled)
			for i := range 3 {
				if math.Abs(got[i]-float64(want[i])) > 1e-5 {
					t.Fatalf("rotation %d moves axis %d to %v, want %v", b, axis, got, want)
				}
			}
		}
	}
	if count != 48 {
		t.Fatalf("checked %d rotations", count)
	}
}

func TestConvertScene(t *testing.T) {
	m := &schemas.VoxelModelT{
		Palette: testPalette(),
		Grids: []*schemas.VoxelModelGridT{
			gridOf([3]uint32{1, 1, 1}, [4]uint8{0, 0, 0, 1}),
			gridOf([3]uint32{2, 2, 2}),
		},
		Nodes: []*schemas.VoxelModelNodeT{
			{Type: schemas.VoxelModelNodeTypeTransform, Children: []uint32{1}, Layer: -1, Frames: []*schemas.VoxelModelTransformFrameT{{Rotation: 4}}},
			{Type: schemas.VoxelModelNodeTypeGroup, Children: []uint32{2, 4}, Layer: -1},
			{Type: schemas.VoxelModelNodeTypeTransform, Name: "a", Children: []uint32{3}, Layer: 0, Frames: []*schemas.VoxelModelTransformFrameT{
				{Rotation: 4, Translation: &schemas.VoxelModelPoint3T{X: 1, Y: 2, Z: 3}},
				{Frame: 3, Rotation: 4},
			}},
			{Type: schemas.VoxelModelNodeTypeShape, Layer: -1, Models: []*schemas.VoxelModelShapeModelT{{Grid: 1}}},
			{Type: schemas.VoxelModelNodeTypeTransform, Name: "b", Hidden: true, Children: []uint32{5}, Layer: 0, Frames: []*schemas.VoxelModelTransformFrameT{{Rotation: 4}}},
			{Type: schemas.VoxelModelNodeTypeShape, Layer: -1, Models: []*schemas.VoxelModelShapeModelT{{Grid: 0}}},
		},
		Materials: []*schemas.VoxelModelMaterialT{{Id: 1}},
		Notes:     []string{"from the voxel model"},
	}
	if err := voxelmodel.Validate(m); err != nil {
		t.Fatal(err)
	}
	source := convert(m, greedyMesher)

	if len(source.Meshes) != 2 || source.Meshes[0].Name != "model0" || source.Meshes[1].Name != "model1" {
		t.Fatalf("unexpected meshes %+v", source.Meshes)
	}
	if len(source.Meshes[1].Primitives) != 0 {
		t.Fatal("an empty grid has primitives")
	}
	if len(source.Nodes) != 3 || source.Scene != 0 || len(source.Scenes) != 1 || source.Scenes[0].Nodes[0] != 0 {
		t.Fatalf("unexpected nodes %+v", source.Nodes)
	}
	root, a, b := source.Nodes[0], source.Nodes[1], source.Nodes[2]
	if root.Mesh != -1 || len(root.Children) != 2 || root.Children[0] != 1 || root.Children[1] != 2 {
		t.Fatalf("unexpected root %+v", root)
	}
	if a.Name != "a" || a.Mesh != 1 || *a.Translation != (schemas.Mesh3DSourceVector3T{X: 1, Y: 3, Z: -2}) {
		t.Fatalf("unexpected node a %+v", a)
	}
	if b.Name != "b" || b.Mesh != 0 || *b.Rotation != (schemas.Mesh3DSourceVector4T{W: 1}) || *b.Scale != (schemas.Mesh3DSourceVector3T{X: 1, Y: 1, Z: 1}) {
		t.Fatalf("unexpected node b %+v", b)
	}

	notes := strings.Join(source.Notes, "\n")
	for _, want := range []string{"from the voxel model", "1 nodes are hidden", "1 nodes are animated", "dropped 1 MagicaVoxel materials"} {
		if !strings.Contains(notes, want) {
			t.Fatalf("notes %q lack %q", notes, want)
		}
	}

	m.Nodes = nil
	source = convert(m, greedyMesher)
	if len(source.Nodes) != 0 || source.Scene != -1 {
		t.Fatal("a voxel model without a scene graph gets nodes")
	}
}

func TestParseMesher(t *testing.T) {
	for s, want := range map[string]mesher{"naive": naiveMesher, "culled": culledMesher, "greedy": greedyMesher} {
		if got, err := parseMesher(s); err != nil || got != want {
			t.Fatalf("parseMesher(%q) = %v, %v", s, got, err)
		}
	}
	if _, err := parseMesher("fast"); err == nil {
		t.Fatal("parseMesher accepts an unknown name")
	}
}
