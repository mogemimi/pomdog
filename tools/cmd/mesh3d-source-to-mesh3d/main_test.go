// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"encoding/binary"
	"math"
	"os"
	"path/filepath"
	"strings"
	"testing"

	flatbuffers "github.com/google/flatbuffers/go"
	schemas "github.com/mogemimi/pomdog/build/schemas"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

func attribute(semantic string, componentCount uint8, values ...float32) *schemas.Mesh3DSourceAttributeT {
	return &schemas.Mesh3DSourceAttributeT{
		Semantic:       semantic,
		ComponentCount: componentCount,
		Values:         values,
	}
}

func primitive(vertexCount uint32, indices []uint32, attributes ...*schemas.Mesh3DSourceAttributeT) *schemas.Mesh3DSourcePrimitiveT {
	return &schemas.Mesh3DSourcePrimitiveT{
		Attributes:  attributes,
		Indices:     indices,
		VertexCount: vertexCount,
		Material:    -1,
		Topology:    schemas.Mesh3DSourceTopologyTriangles,
	}
}

// makeQuadSource returns a source with the mesh "Quad", two triangles that
// are stored as six vertices without indices. The triangles share two
// vertices, and a seventh vertex is not drawn.
func makeQuadSource() *schemas.Mesh3DSourceT {
	quad := primitive(6, nil,
		attribute("NORMAL", 3,
			0, 0, 1,
			0, 0, 1,
			0, 0, 1,
			0, 0, 1,
			0, 0, 1,
			0, 0, 1,
		),
		attribute("POSITION", 3,
			0, 0, 0,
			1, 0, 0,
			0, 1, 0,
			0, 1, 0,
			1, 0, 0,
			1, 1, 0,
		),
		attribute("TEXCOORD_0", 2,
			0, 0,
			1, 0,
			0, 1,
			0, 1,
			1, 0,
			1, 1,
		),
	)
	quad.Material = 0

	return &schemas.Mesh3DSourceT{
		Generator:    "test",
		SourceFormat: "glTF 2.0",
		Meshes:       []*schemas.Mesh3DSourceMeshT{{Name: "Quad", Primitives: []*schemas.Mesh3DSourcePrimitiveT{quad}}},
		Materials:    []*schemas.Mesh3DSourceMaterialT{{Name: "Paint"}},
		Scene:        -1,
		Notes:        []string{"1 textures are not supported and were dropped"},
	}
}

func mustConvert(t *testing.T, source *schemas.Mesh3DSourceT) *output {
	t.Helper()
	out, err := convert(source)
	if err != nil {
		t.Fatalf("convert() failed: %v", err)
	}
	return out
}

func readFloats(t *testing.T, data []byte) []float32 {
	t.Helper()
	if len(data)%4 != 0 {
		t.Fatalf("got %d bytes of vertices, want a multiple of 4", len(data))
	}
	values := make([]float32, 0, len(data)/4)
	for i := 0; i < len(data); i += 4 {
		values = append(values, math.Float32frombits(binary.LittleEndian.Uint32(data[i:])))
	}
	return values
}

func readUint16s(t *testing.T, data []byte) []uint32 {
	t.Helper()
	if len(data)%2 != 0 {
		t.Fatalf("got %d bytes of indices, want a multiple of 2", len(data))
	}
	values := make([]uint32, 0, len(data)/2)
	for i := 0; i < len(data); i += 2 {
		values = append(values, uint32(binary.LittleEndian.Uint16(data[i:])))
	}
	return values
}

func equalFloats(got []float32, want ...float32) bool {
	if len(got) != len(want) {
		return false
	}
	for i := range got {
		if got[i] != want[i] {
			return false
		}
	}
	return true
}

func equalIndices(got []uint32, want ...uint32) bool {
	if len(got) != len(want) {
		return false
	}
	for i := range got {
		if got[i] != want[i] {
			return false
		}
	}
	return true
}

func TestConvertMergesVertices(t *testing.T) {
	out := mustConvert(t, makeQuadSource())
	mesh := out.Mesh

	if len(mesh.Keys) != 1 || mesh.Keys[0] != stringhash.StringToHash32("Quad") || len(mesh.Meshes) != 1 {
		t.Fatalf("unexpected keys %v for %d meshes", mesh.Keys, len(mesh.Meshes))
	}

	// NOTE: The elements are in the order of their semantic, not of the source.
	if len(mesh.VertexBuffers) != 1 {
		t.Fatalf("got %d vertex buffers, want 1", len(mesh.VertexBuffers))
	}
	buffer := mesh.VertexBuffers[0]
	if buffer.ByteOffset != 0 || buffer.VertexCount != 4 || buffer.StrideBytes != 32 {
		t.Errorf("unexpected vertex buffer: offset %d, %d vertices, stride %d", buffer.ByteOffset, buffer.VertexCount, buffer.StrideBytes)
	}
	wantElements := []schemas.Mesh3DVertexElementT{
		{ByteOffset: 0, Semantic: schemas.Mesh3DVertexSemanticPosition, Format: schemas.Mesh3DVertexElementFormatFloat32x3},
		{ByteOffset: 12, Semantic: schemas.Mesh3DVertexSemanticNormal, Format: schemas.Mesh3DVertexElementFormatFloat32x3},
		{ByteOffset: 24, Semantic: schemas.Mesh3DVertexSemanticTexCoord, Format: schemas.Mesh3DVertexElementFormatFloat32x2},
	}
	if len(buffer.Elements) != len(wantElements) {
		t.Fatalf("got %d elements, want %d", len(buffer.Elements), len(wantElements))
	}
	for i, want := range wantElements {
		if *buffer.Elements[i] != want {
			t.Errorf("element %d: got %+v, want %+v", i, *buffer.Elements[i], want)
		}
	}

	// NOTE: Six vertices become four, in the order in which the triangles
	// refer to them.
	if !equalFloats(readFloats(t, out.Vertices),
		0, 0, 0, 0, 0, 1, 0, 0,
		1, 0, 0, 0, 0, 1, 1, 0,
		0, 1, 0, 0, 0, 1, 0, 1,
		1, 1, 0, 0, 0, 1, 1, 1,
	) {
		t.Errorf("unexpected vertices: %v", readFloats(t, out.Vertices))
	}

	if mesh.IndexFormat != schemas.Mesh3DIndexFormatUInt16 || mesh.IndexCount != 6 {
		t.Errorf("got index format %s and %d indices, want UInt16 and 6", mesh.IndexFormat, mesh.IndexCount)
	}
	if !equalIndices(readUint16s(t, out.Indices), 0, 1, 2, 2, 1, 3) {
		t.Errorf("unexpected indices: %v", readUint16s(t, out.Indices))
	}

	parts := mesh.Meshes[0].Parts
	if len(parts) != 1 || *parts[0] != (schemas.Mesh3DPartT{StartIndex: 0, IndexCount: 6, VertexBuffer: 0}) {
		t.Errorf("unexpected parts: %+v", parts)
	}
}

func TestConvertDebug(t *testing.T) {
	out := mustConvert(t, makeQuadSource())
	debug := out.Debug

	if debug.Generator != "test" || debug.SourceFormat != "glTF 2.0" {
		t.Errorf("got generator %q and format %q", debug.Generator, debug.SourceFormat)
	}
	if len(debug.Meshes) != 1 || debug.Meshes[0].Name != "Quad" || debug.Meshes[0].Key != stringhash.StringToHash32("Quad") {
		t.Fatalf("unexpected meshes: %+v", debug.Meshes)
	}
	part := debug.Meshes[0].Parts[0]
	if part.Material != "Paint" || part.SourceVertexCount != 6 || part.SourceIndexCount != 0 {
		t.Errorf("unexpected part: %+v", part)
	}

	buffer := debug.VertexBuffers[0]
	if strings.Join(buffer.Elements, ",") != "POSITION,NORMAL,TEXCOORD_0" || buffer.SourceVertexCount != 6 || buffer.VertexCount != 4 {
		t.Errorf("unexpected vertex buffer: %+v", buffer)
	}

	// NOTE: The notes of the importer are kept.
	if len(debug.Notes) != 1 || debug.Notes[0] != "1 textures are not supported and were dropped" {
		t.Errorf("unexpected notes: %v", debug.Notes)
	}
}

func TestConvertDropsUnusedVertices(t *testing.T) {
	source := &schemas.Mesh3DSourceT{
		Meshes: []*schemas.Mesh3DSourceMeshT{{Name: "Tri", Primitives: []*schemas.Mesh3DSourcePrimitiveT{
			// NOTE: No index refers to the vertices 1 and 4.
			primitive(5, []uint32{3, 0, 2},
				attribute("POSITION", 3,
					0, 0, 0,
					9, 9, 9,
					2, 0, 0,
					3, 0, 0,
					8, 8, 8,
				),
			),
		}}},
	}
	out := mustConvert(t, source)

	if !equalFloats(readFloats(t, out.Vertices), 3, 0, 0, 0, 0, 0, 2, 0, 0) {
		t.Errorf("unexpected vertices: %v", readFloats(t, out.Vertices))
	}
	if !equalIndices(readUint16s(t, out.Indices), 0, 1, 2) {
		t.Errorf("unexpected indices: %v", readUint16s(t, out.Indices))
	}
	if out.Debug.VertexBuffers[0].SourceVertexCount != 5 || out.Debug.VertexBuffers[0].VertexCount != 3 {
		t.Errorf("unexpected counts: %+v", out.Debug.VertexBuffers[0])
	}
	if out.Debug.Notes != nil {
		t.Errorf("got notes %v, want none", out.Debug.Notes)
	}
}

func TestConvertSharesVertexBuffersByLayout(t *testing.T) {
	triangle := func(z float32) *schemas.Mesh3DSourceAttributeT {
		return attribute("POSITION", 3, 0, 0, z, 1, 0, z, 0, 1, z)
	}
	source := &schemas.Mesh3DSourceT{
		Meshes: []*schemas.Mesh3DSourceMeshT{
			{Name: "b", Primitives: []*schemas.Mesh3DSourcePrimitiveT{
				primitive(3, nil, triangle(0)),
				// NOTE: The attributes come in another order than the elements,
				// and "_CUSTOM" is not a semantic of a vertex element.
				primitive(3, nil,
					attribute("COLOR_0", 4, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1),
					triangle(1),
					attribute("TEXCOORD_1", 2, 5, 5, 6, 6, 7, 7),
					attribute("TEXCOORD_0", 2, 0, 0, 1, 0, 0, 1),
					attribute("_CUSTOM", 1, 1, 2, 3),
				),
				// NOTE: This primitive shares a vertex with the first one.
				primitive(3, nil, attribute("POSITION", 3, 0, 0, 0, 2, 0, 0, 0, 2, 0)),
			}},
			{Name: "a", Primitives: []*schemas.Mesh3DSourcePrimitiveT{
				primitive(3, nil, triangle(2)),
			}},
		},
	}
	out := mustConvert(t, source)
	mesh := out.Mesh

	// NOTE: The meshes are sorted by key, and both arrays are in the same order.
	if len(mesh.Keys) != 2 || mesh.Keys[0] >= mesh.Keys[1] {
		t.Fatalf("keys are not in ascending order: %v", mesh.Keys)
	}
	indexOf := map[string]int{}
	for i, m := range out.Debug.Meshes {
		if m.Key != mesh.Keys[i] || m.Key != stringhash.StringToHash32(m.Name) {
			t.Errorf("mesh %d: key %#x does not match the name %q", i, m.Key, m.Name)
		}
		indexOf[m.Name] = i
	}

	if len(mesh.VertexBuffers) != 2 {
		t.Fatalf("got %d vertex buffers, want 2", len(mesh.VertexBuffers))
	}

	// NOTE: The buffers are in the order in which the meshes first use them.
	// Both meshes use the position-only layout first.
	positionOnly, colored := 0, 1
	if mesh.VertexBuffers[positionOnly].StrideBytes != 12 || mesh.VertexBuffers[colored].StrideBytes != 44 {
		t.Fatalf("unexpected strides: %d, %d", mesh.VertexBuffers[positionOnly].StrideBytes, mesh.VertexBuffers[colored].StrideBytes)
	}

	// NOTE: The position-only buffer holds the triangles of both meshes
	// without the shared vertex twice: 3 + 2 + 3 vertices.
	if mesh.VertexBuffers[positionOnly].VertexCount != 8 || mesh.VertexBuffers[positionOnly].ByteOffset != 0 {
		t.Errorf("unexpected position-only buffer: %+v", mesh.VertexBuffers[positionOnly])
	}
	if mesh.VertexBuffers[colored].VertexCount != 3 || mesh.VertexBuffers[colored].ByteOffset != 8*12 {
		t.Errorf("unexpected colored buffer: %+v", mesh.VertexBuffers[colored])
	}
	if len(out.Vertices) != 8*12+3*44 {
		t.Errorf("got %d bytes of vertices, want %d", len(out.Vertices), 8*12+3*44)
	}

	wantElements := []schemas.Mesh3DVertexElementT{
		{ByteOffset: 0, Semantic: schemas.Mesh3DVertexSemanticPosition, Format: schemas.Mesh3DVertexElementFormatFloat32x3},
		{ByteOffset: 12, Semantic: schemas.Mesh3DVertexSemanticTexCoord, SemanticIndex: 0, Format: schemas.Mesh3DVertexElementFormatFloat32x2},
		{ByteOffset: 20, Semantic: schemas.Mesh3DVertexSemanticTexCoord, SemanticIndex: 1, Format: schemas.Mesh3DVertexElementFormatFloat32x2},
		{ByteOffset: 28, Semantic: schemas.Mesh3DVertexSemanticColor, Format: schemas.Mesh3DVertexElementFormatFloat32x4},
	}
	elements := mesh.VertexBuffers[colored].Elements
	if len(elements) != len(wantElements) {
		t.Fatalf("got %d elements, want %d", len(elements), len(wantElements))
	}
	for i, want := range wantElements {
		if *elements[i] != want {
			t.Errorf("element %d: got %+v, want %+v", i, *elements[i], want)
		}
	}
	if strings.Join(out.Debug.VertexBuffers[colored].Elements, ",") != "POSITION,TEXCOORD_0,TEXCOORD_1,COLOR_0" {
		t.Errorf("unexpected element names: %v", out.Debug.VertexBuffers[colored].Elements)
	}

	// NOTE: The first vertex of the colored buffer interleaves its elements.
	colorVertices := readFloats(t, out.Vertices[8*12:])
	if !equalFloats(colorVertices[:11], 0, 0, 1, 0, 0, 5, 5, 1, 0, 0, 1) {
		t.Errorf("unexpected first colored vertex: %v", colorVertices[:11])
	}

	// NOTE: The parts of "b" draw from both buffers, and their indices count
	// from the start of their own buffer.
	parts := mesh.Meshes[indexOf["b"]].Parts
	if len(parts) != 3 {
		t.Fatalf("got %d parts, want 3", len(parts))
	}
	if parts[0].VertexBuffer != uint16(positionOnly) || parts[1].VertexBuffer != uint16(colored) || parts[2].VertexBuffer != uint16(positionOnly) {
		t.Errorf("unexpected vertex buffers of the parts: %d, %d, %d", parts[0].VertexBuffer, parts[1].VertexBuffer, parts[2].VertexBuffer)
	}
	indices := readUint16s(t, out.Indices)
	if mesh.IndexCount != 12 || len(indices) != 12 {
		t.Fatalf("got %d indices, want 12", len(indices))
	}
	for i, part := range parts {
		if part.IndexCount != 3 {
			t.Errorf("part %d has %d indices, want 3", i, part.IndexCount)
		}
	}
	coloredIndices := indices[parts[1].StartIndex : parts[1].StartIndex+3]
	if !equalIndices(coloredIndices, 0, 1, 2) {
		t.Errorf("unexpected indices of the colored part: %v", coloredIndices)
	}

	// NOTE: The third part reuses the vertex (0, 0, 0) of the first part.
	first := indices[parts[0].StartIndex : parts[0].StartIndex+3]
	third := indices[parts[2].StartIndex : parts[2].StartIndex+3]
	if third[0] != first[0] || third[1] == first[1] || third[2] == first[2] {
		t.Errorf("unexpected indices of the parts: %v, %v", first, third)
	}

	wantNote := `mesh "b": primitive #1: attribute "_CUSTOM" is not supported and was dropped`
	if len(out.Debug.Notes) != 1 || out.Debug.Notes[0] != wantNote {
		t.Errorf("got notes %v, want [%s]", out.Debug.Notes, wantNote)
	}
}

func TestConvertPicksIndexFormat(t *testing.T) {
	// NOTE: makeSource returns a mesh of triangles that do not share vertices.
	makeSource := func(vertexCount int) *schemas.Mesh3DSourceT {
		values := make([]float32, 0, vertexCount*3)
		for i := 0; i < vertexCount; i++ {
			values = append(values, float32(i), 0, 0)
		}
		return &schemas.Mesh3DSourceT{
			Meshes: []*schemas.Mesh3DSourceMeshT{{Name: "Big", Primitives: []*schemas.Mesh3DSourcePrimitiveT{
				primitive(uint32(vertexCount), nil, attribute("POSITION", 3, values...)),
			}}},
		}
	}

	// NOTE: 16-bit indices address 65536 vertices. The mesh needs a multiple
	// of three vertices, so 65535 is the largest count that still fits.
	small := mustConvert(t, makeSource(65535))
	if small.Mesh.IndexFormat != schemas.Mesh3DIndexFormatUInt16 || len(small.Indices) != 65535*2 {
		t.Errorf("got index format %s and %d bytes, want UInt16 and %d", small.Mesh.IndexFormat, len(small.Indices), 65535*2)
	}

	large := mustConvert(t, makeSource(65538))
	if large.Mesh.IndexFormat != schemas.Mesh3DIndexFormatUInt32 || len(large.Indices) != 65538*4 {
		t.Errorf("got index format %s and %d bytes, want UInt32 and %d", large.Mesh.IndexFormat, len(large.Indices), 65538*4)
	}
	if last := binary.LittleEndian.Uint32(large.Indices[len(large.Indices)-4:]); last != 65537 {
		t.Errorf("got the last index %d, want 65537", last)
	}
}

func TestConvertErrors(t *testing.T) {
	triangle := func() *schemas.Mesh3DSourceAttributeT {
		return attribute("POSITION", 3, 0, 0, 0, 1, 0, 0, 0, 1, 0)
	}
	mesh := func(name string, primitives ...*schemas.Mesh3DSourcePrimitiveT) *schemas.Mesh3DSourceMeshT {
		return &schemas.Mesh3DSourceMeshT{Name: name, Primitives: primitives}
	}
	strip := primitive(3, nil, triangle())
	strip.Topology = schemas.Mesh3DSourceTopologyTriangleStrip
	withMaterial := primitive(3, nil, triangle())
	withMaterial.Material = 2

	cases := []struct {
		name    string
		meshes  []*schemas.Mesh3DSourceMeshT
		wantErr string
	}{
		{"no meshes", nil, "the source has no meshes"},
		{"mesh without a name", []*schemas.Mesh3DSourceMeshT{mesh("", primitive(3, nil, triangle()))}, "mesh #0 has no name"},
		{"duplicate mesh", []*schemas.Mesh3DSourceMeshT{mesh("a", primitive(3, nil, triangle())), mesh("a", primitive(3, nil, triangle()))}, `mesh "a" is defined more than once`},
		{"mesh without primitives", []*schemas.Mesh3DSourceMeshT{mesh("a")}, `mesh "a": the mesh has no primitives`},
		{"triangle strip", []*schemas.Mesh3DSourceMeshT{mesh("a", strip)}, "topology TriangleStrip is not supported"},
		{"unknown material", []*schemas.Mesh3DSourceMeshT{mesh("a", withMaterial)}, "material #2 does not exist"},
		{"no position", []*schemas.Mesh3DSourceMeshT{mesh("a", primitive(3, nil, attribute("NORMAL", 3, 0, 0, 1, 0, 0, 1, 0, 0, 1)))}, `no "POSITION" attribute`},
		{"too few values", []*schemas.Mesh3DSourceMeshT{mesh("a", primitive(3, nil, attribute("POSITION", 3, 0, 0, 0, 1, 0, 0)))}, `attribute "POSITION" has 6 values, but 3 vertices need 9`},
		{"too many components", []*schemas.Mesh3DSourceMeshT{mesh("a", primitive(1, nil, attribute("POSITION", 5, 0, 0, 0, 0, 0)))}, "has 5 components"},
		{"duplicate attribute", []*schemas.Mesh3DSourceMeshT{mesh("a", primitive(3, nil, triangle(), triangle()))}, `attribute "POSITION" is defined more than once`},
		{"index out of range", []*schemas.Mesh3DSourceMeshT{mesh("a", primitive(3, []uint32{0, 1, 3}, triangle()))}, "index 3 is out of the 3 vertices"},
		{"indices that are not triangles", []*schemas.Mesh3DSourceMeshT{mesh("a", primitive(3, []uint32{0, 1}, triangle()))}, "triangles need three indices each, but the primitive has 2"},
		{"no vertices", []*schemas.Mesh3DSourceMeshT{mesh("a", primitive(0, nil, attribute("POSITION", 3)))}, "triangles need three indices each, but the primitive has 0"},
	}

	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			_, err := convert(&schemas.Mesh3DSourceT{Meshes: tc.meshes})
			if err == nil {
				t.Fatalf("convert() succeeded, want an error with %q", tc.wantErr)
			}
			if !strings.Contains(err.Error(), tc.wantErr) {
				t.Errorf("got error %q, want it to have %q", err.Error(), tc.wantErr)
			}
		})
	}
}

func TestParseSemantic(t *testing.T) {
	cases := []struct {
		name     string
		semantic schemas.Mesh3DVertexSemantic
		index    uint8
		ok       bool
	}{
		{"POSITION", schemas.Mesh3DVertexSemanticPosition, 0, true},
		{"NORMAL", schemas.Mesh3DVertexSemanticNormal, 0, true},
		{"TANGENT", schemas.Mesh3DVertexSemanticTangent, 0, true},
		{"TEXCOORD_0", schemas.Mesh3DVertexSemanticTexCoord, 0, true},
		{"TEXCOORD_12", schemas.Mesh3DVertexSemanticTexCoord, 12, true},
		{"COLOR_1", schemas.Mesh3DVertexSemanticColor, 1, true},
		{"JOINTS_0", schemas.Mesh3DVertexSemanticJoints, 0, true},
		{"WEIGHTS_255", schemas.Mesh3DVertexSemanticWeights, 255, true},
		{"TEXCOORD", 0, 0, false},
		{"TEXCOORD_", 0, 0, false},
		{"TEXCOORD_01", 0, 0, false},
		{"TEXCOORD_256", 0, 0, false},
		{"TEXCOORD_-1", 0, 0, false},
		{"POSITION_0", 0, 0, false},
		{"_CUSTOM", 0, 0, false},
		{"", 0, 0, false},
	}
	for _, tc := range cases {
		semantic, index, ok := parseSemantic(tc.name)
		if semantic != tc.semantic || index != tc.index || ok != tc.ok {
			t.Errorf("parseSemantic(%q) = %s, %d, %v, want %s, %d, %v", tc.name, semantic, index, ok, tc.semantic, tc.index, tc.ok)
		}
	}
}

func packSource(source *schemas.Mesh3DSourceT) []byte {
	builder := flatbuffers.NewBuilder(0)
	builder.Finish(source.Pack(builder))
	return builder.FinishedBytes()
}

func TestRunWritesFiles(t *testing.T) {
	dir := t.TempDir()
	opts := &options{
		InFile:      filepath.Join(dir, "in.mesh3dsource"),
		OutMesh:     filepath.Join(dir, "out", "model.mesh3d"),
		OutVertices: filepath.Join(dir, "out", "model.mesh3dvertices"),
		OutIndices:  filepath.Join(dir, "out", "model.mesh3dindices"),
		OutDebug:    filepath.Join(dir, "debug", "model.mesh3d-debug"),
	}
	if err := os.WriteFile(opts.InFile, packSource(makeQuadSource()), 0o644); err != nil {
		t.Fatalf("os.WriteFile() failed: %v", err)
	}
	if err := run(opts); err != nil {
		t.Fatalf("run() failed: %v", err)
	}

	read := func(path string) []byte {
		t.Helper()
		data, err := os.ReadFile(path)
		if err != nil {
			t.Fatalf("os.ReadFile() failed: %v", err)
		}
		return data
	}

	mesh := schemas.GetRootAsMesh3D(read(opts.OutMesh), 0).UnPack()
	if len(mesh.Meshes) != 1 || mesh.IndexCount != 6 || mesh.VertexBuffers[0].VertexCount != 4 {
		t.Errorf("unexpected mesh file: %d meshes, %d indices", len(mesh.Meshes), mesh.IndexCount)
	}

	// NOTE: The raw files have no header, only the vertices and the indices.
	if got := len(read(opts.OutVertices)); got != 4*32 {
		t.Errorf("got %d bytes of vertices, want %d", got, 4*32)
	}
	if got := len(read(opts.OutIndices)); got != 6*2 {
		t.Errorf("got %d bytes of indices, want %d", got, 6*2)
	}

	debug := schemas.GetRootAsMesh3DDebug(read(opts.OutDebug), 0).UnPack()
	if len(debug.Meshes) != 1 || debug.Meshes[0].Name != "Quad" {
		t.Errorf("unexpected debug file: %+v", debug.Meshes)
	}

	t.Run("the debug file is optional", func(t *testing.T) {
		opts := *opts
		opts.OutMesh = filepath.Join(dir, "nodebug", "model.mesh3d")
		opts.OutVertices = filepath.Join(dir, "nodebug", "model.mesh3dvertices")
		opts.OutIndices = filepath.Join(dir, "nodebug", "model.mesh3dindices")
		opts.OutDebug = ""
		if err := run(&opts); err != nil {
			t.Fatalf("run() failed: %v", err)
		}
		entries, err := os.ReadDir(filepath.Join(dir, "nodebug"))
		if err != nil {
			t.Fatalf("os.ReadDir() failed: %v", err)
		}
		if len(entries) != 3 {
			t.Errorf("got %d files, want 3", len(entries))
		}
	})

	t.Run("the same input gives the same bytes", func(t *testing.T) {
		first := string(read(opts.OutMesh)) + string(read(opts.OutVertices)) + string(read(opts.OutIndices)) + string(read(opts.OutDebug))
		if err := run(opts); err != nil {
			t.Fatalf("run() failed: %v", err)
		}
		again := string(read(opts.OutMesh)) + string(read(opts.OutVertices)) + string(read(opts.OutIndices)) + string(read(opts.OutDebug))
		if again != first {
			t.Errorf("the second run produced different bytes")
		}
	})

	t.Run("a file of another kind fails", func(t *testing.T) {
		opts := *opts
		opts.InFile = filepath.Join(dir, "garbage.mesh3dsource")
		if err := os.WriteFile(opts.InFile, []byte{0xff, 0xff, 0xff, 0x7f, 1, 2, 3}, 0o644); err != nil {
			t.Fatalf("os.WriteFile() failed: %v", err)
		}
		err := run(&opts)
		if err == nil || !strings.Contains(err.Error(), "is not a mesh source file") {
			t.Errorf("got error %v, want one about the mesh source file", err)
		}
	})

	t.Run("a missing input fails", func(t *testing.T) {
		opts := *opts
		opts.InFile = filepath.Join(dir, "missing.mesh3dsource")
		if err := run(&opts); err == nil {
			t.Errorf("run() succeeded, want an error")
		}
	})
}
