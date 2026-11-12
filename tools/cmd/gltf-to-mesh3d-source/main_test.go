// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"encoding/binary"
	"math"
	"os"
	"path/filepath"
	"strings"
	"testing"

	schemas "github.com/mogemimi/pomdog/build/schemas"
)

// makeGLB returns a glTF binary file with a JSON chunk and, when bin is not
// nil, a binary chunk. The chunks are padded to four bytes as the format
// requires.
func makeGLB(jsonText string, bin []byte) []byte {
	jsonChunk := []byte(jsonText)
	for len(jsonChunk)%4 != 0 {
		jsonChunk = append(jsonChunk, ' ')
	}

	chunks := []byte{}
	chunks = binary.LittleEndian.AppendUint32(chunks, uint32(len(jsonChunk)))
	chunks = binary.LittleEndian.AppendUint32(chunks, glbChunkJSON)
	chunks = append(chunks, jsonChunk...)

	if bin != nil {
		binChunk := append([]byte{}, bin...)
		for len(binChunk)%4 != 0 {
			binChunk = append(binChunk, 0)
		}
		chunks = binary.LittleEndian.AppendUint32(chunks, uint32(len(binChunk)))
		chunks = binary.LittleEndian.AppendUint32(chunks, glbChunkBIN)
		chunks = append(chunks, binChunk...)
	}

	data := []byte{}
	data = binary.LittleEndian.AppendUint32(data, glbMagic)
	data = binary.LittleEndian.AppendUint32(data, 2)
	data = binary.LittleEndian.AppendUint32(data, uint32(glbHeaderSize+len(chunks)))
	return append(data, chunks...)
}

func appendFloats(data []byte, values ...float32) []byte {
	for _, value := range values {
		data = binary.LittleEndian.AppendUint32(data, math.Float32bits(value))
	}
	return data
}

func appendUint16s(data []byte, values ...uint16) []byte {
	for _, value := range values {
		data = binary.LittleEndian.AppendUint16(data, value)
	}
	return data
}

// makeSampleBinary returns the binary chunk of sampleJSON:
//
//	offset   0: positions, four VEC3 of floats
//	offset  48: normals, four VEC3 of normalized bytes with a stride of 4
//	offset  64: texture coordinates, four VEC2 of normalized unsigned shorts
//	offset  80: colors, four VEC4 of normalized unsigned bytes
//	offset  96: indices, six unsigned bytes
//	offset 104: positions of the second mesh, three VEC3 of floats
func makeSampleBinary() []byte {
	data := []byte{}
	data = appendFloats(data, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0.5)
	data = append(data,
		0, 0, 127, 99,
		0, 0, 129, 99, // NOTE: 129 is -127 as a signed byte.
		127, 0, 0, 99,
		128, 0, 0, 99, // NOTE: 128 is -128, which clamps to -1.
	)
	data = appendUint16s(data, 0, 0, 65535, 0, 0, 65535, 65535, 13107)
	data = append(data,
		255, 0, 0, 255,
		0, 255, 0, 255,
		0, 0, 255, 255,
		51, 102, 153, 0,
	)
	data = append(data, 0, 1, 2, 2, 1, 3, 0, 0)
	data = appendFloats(data, 0, 0, 0, 2, 0, 0, 0, 2, 0)
	return data
}

const sampleJSON = `{
  "asset": {"version": "2.0", "generator": "test"},
  "scene": 0,
  "scenes": [{"name": "Scene", "nodes": [0, 2]}],
  "nodes": [
    {"name": "QuadNode", "mesh": 0, "children": [1], "translation": [1, 2, 3], "rotation": [0, 1, 0, 0], "scale": [2, 2, 2]},
    {"name": "TriNode", "mesh": 1},
    {"name": "MatrixNode", "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 4, 5, 6, 1]}
  ],
  "materials": [
    {"name": "Paint", "doubleSided": true, "emissiveFactor": [0.5, 0.25, 0],
     "pbrMetallicRoughness": {"baseColorFactor": [0.5, 0.25, 0.125, 1], "metallicFactor": 0, "roughnessFactor": 0.5}},
    {}
  ],
  "meshes": [
    {"name": "Quad", "primitives": [
      {"attributes": {"POSITION": 0, "NORMAL": 1, "TEXCOORD_0": 2, "COLOR_0": 3}, "indices": 4, "material": 0,
       "targets": [{"POSITION": 0}]}
    ]},
    {"name": "Tri", "primitives": [
      {"attributes": {"POSITION": 5}, "mode": 5}
    ]}
  ],
  "accessors": [
    {"bufferView": 0, "componentType": 5126, "count": 4, "type": "VEC3"},
    {"bufferView": 1, "componentType": 5120, "normalized": true, "count": 4, "type": "VEC3"},
    {"bufferView": 2, "componentType": 5123, "normalized": true, "count": 4, "type": "VEC2"},
    {"bufferView": 2, "byteOffset": 16, "componentType": 5121, "normalized": true, "count": 4, "type": "VEC4"},
    {"bufferView": 3, "componentType": 5121, "count": 6, "type": "SCALAR"},
    {"bufferView": 4, "componentType": 5126, "count": 3, "type": "VEC3"}
  ],
  "bufferViews": [
    {"buffer": 0, "byteOffset": 0, "byteLength": 48},
    {"buffer": 0, "byteOffset": 48, "byteLength": 16, "byteStride": 4},
    {"buffer": 0, "byteOffset": 64, "byteLength": 32},
    {"buffer": 0, "byteOffset": 96, "byteLength": 6},
    {"buffer": 0, "byteOffset": 104, "byteLength": 36}
  ],
  "buffers": [{"byteLength": 140}],
  "animations": [{}, {}],
  "textures": [{}]
}`

func convertGLB(t *testing.T, jsonText string, bin []byte) (*schemas.Mesh3DSourceT, error) {
	t.Helper()
	jsonChunk, binChunk, err := parseGLB(makeGLB(jsonText, bin))
	if err != nil {
		return nil, err
	}
	doc, err := parseGLTFDocument(jsonChunk)
	if err != nil {
		return nil, err
	}
	return convert(doc, binChunk)
}

func mustConvertSample(t *testing.T) *schemas.Mesh3DSourceT {
	t.Helper()
	source, err := convertGLB(t, sampleJSON, makeSampleBinary())
	if err != nil {
		t.Fatalf("convert() failed: %v", err)
	}
	return source
}

func equalFloats(got []float32, want ...float32) bool {
	if len(got) != len(want) {
		return false
	}
	for i := range got {
		if math.Abs(float64(got[i]-want[i])) > 1e-6 {
			return false
		}
	}
	return true
}

func TestConvertMeshes(t *testing.T) {
	source := mustConvertSample(t)

	if source.Generator != "test" || source.SourceFormat != "glTF 2.0" {
		t.Errorf("got generator %q and format %q", source.Generator, source.SourceFormat)
	}
	if len(source.Meshes) != 2 {
		t.Fatalf("got %d meshes, want 2", len(source.Meshes))
	}

	quad := source.Meshes[0]
	if quad.Name != "Quad" || len(quad.Primitives) != 1 {
		t.Fatalf("unexpected first mesh: %q with %d primitives", quad.Name, len(quad.Primitives))
	}
	primitive := quad.Primitives[0]
	if primitive.VertexCount != 4 || primitive.Material != 0 || primitive.Topology != schemas.Mesh3DSourceTopologyTriangles {
		t.Errorf("unexpected primitive: %d vertices, material %d, topology %s", primitive.VertexCount, primitive.Material, primitive.Topology)
	}

	// NOTE: The attributes are sorted by name.
	if len(primitive.Attributes) != 4 {
		t.Fatalf("got %d attributes, want 4", len(primitive.Attributes))
	}
	color, normal, position, texcoord := primitive.Attributes[0], primitive.Attributes[1], primitive.Attributes[2], primitive.Attributes[3]

	if position.Semantic != "POSITION" || position.ComponentCount != 3 || position.SourceComponentType != schemas.Mesh3DSourceComponentTypeFloat32 || position.SourceNormalized {
		t.Errorf("unexpected position attribute: %+v", position)
	}
	if !equalFloats(position.Values, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0.5) {
		t.Errorf("unexpected positions: %v", position.Values)
	}

	// NOTE: A buffer view with a stride skips the padding byte of each normal,
	// and a signed byte maps to [-1, 1].
	if normal.Semantic != "NORMAL" || normal.ComponentCount != 3 || normal.SourceComponentType != schemas.Mesh3DSourceComponentTypeInt8 || !normal.SourceNormalized {
		t.Errorf("unexpected normal attribute: %+v", normal)
	}
	if !equalFloats(normal.Values, 0, 0, 1, 0, 0, -1, 1, 0, 0, -1, 0, 0) {
		t.Errorf("unexpected normals: %v", normal.Values)
	}

	if texcoord.Semantic != "TEXCOORD_0" || texcoord.ComponentCount != 2 || texcoord.SourceComponentType != schemas.Mesh3DSourceComponentTypeUInt16 || !texcoord.SourceNormalized {
		t.Errorf("unexpected texture coordinate attribute: %+v", texcoord)
	}
	if !equalFloats(texcoord.Values, 0, 0, 1, 0, 0, 1, 1, 0.2) {
		t.Errorf("unexpected texture coordinates: %v", texcoord.Values)
	}

	// NOTE: The accessor starts 16 bytes into its buffer view.
	if color.Semantic != "COLOR_0" || color.ComponentCount != 4 || color.SourceComponentType != schemas.Mesh3DSourceComponentTypeUInt8 || !color.SourceNormalized {
		t.Errorf("unexpected color attribute: %+v", color)
	}
	if !equalFloats(color.Values, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0.2, 0.4, 0.6, 0) {
		t.Errorf("unexpected colors: %v", color.Values)
	}

	wantIndices := []uint32{0, 1, 2, 2, 1, 3}
	if len(primitive.Indices) != len(wantIndices) {
		t.Fatalf("got %d indices, want %d", len(primitive.Indices), len(wantIndices))
	}
	for i, want := range wantIndices {
		if primitive.Indices[i] != want {
			t.Errorf("got indices %v, want %v", primitive.Indices, wantIndices)
			break
		}
	}

	// NOTE: The second primitive has no indices, no material, and a mode.
	tri := source.Meshes[1].Primitives[0]
	if tri.VertexCount != 3 || len(tri.Indices) != 0 || tri.Material != -1 || tri.Topology != schemas.Mesh3DSourceTopologyTriangleStrip {
		t.Errorf("unexpected second primitive: %d vertices, %d indices, material %d, topology %s", tri.VertexCount, len(tri.Indices), tri.Material, tri.Topology)
	}
	if !equalFloats(tri.Attributes[0].Values, 0, 0, 0, 2, 0, 0, 0, 2, 0) {
		t.Errorf("unexpected positions: %v", tri.Attributes[0].Values)
	}
}

func TestConvertMaterialsAndNodes(t *testing.T) {
	source := mustConvertSample(t)

	if len(source.Materials) != 2 {
		t.Fatalf("got %d materials, want 2", len(source.Materials))
	}
	paint := source.Materials[0]
	if paint.Name != "Paint" || !paint.DoubleSided || paint.MetallicFactor != 0 || paint.RoughnessFactor != 0.5 {
		t.Errorf("unexpected material: %+v", paint)
	}
	if *paint.BaseColorFactor != (schemas.Mesh3DSourceVector4T{X: 0.5, Y: 0.25, Z: 0.125, W: 1}) {
		t.Errorf("unexpected base color: %+v", paint.BaseColorFactor)
	}
	if *paint.EmissiveFactor != (schemas.Mesh3DSourceVector3T{X: 0.5, Y: 0.25, Z: 0}) {
		t.Errorf("unexpected emissive factor: %+v", paint.EmissiveFactor)
	}

	// NOTE: A material without values has the defaults of glTF.
	plain := source.Materials[1]
	if plain.Name != "" || plain.DoubleSided || plain.MetallicFactor != 1 || plain.RoughnessFactor != 1 {
		t.Errorf("unexpected default material: %+v", plain)
	}
	if *plain.BaseColorFactor != (schemas.Mesh3DSourceVector4T{X: 1, Y: 1, Z: 1, W: 1}) || *plain.EmissiveFactor != (schemas.Mesh3DSourceVector3T{}) {
		t.Errorf("unexpected default factors: %+v, %+v", plain.BaseColorFactor, plain.EmissiveFactor)
	}

	if len(source.Nodes) != 3 {
		t.Fatalf("got %d nodes, want 3", len(source.Nodes))
	}
	quadNode := source.Nodes[0]
	if quadNode.Name != "QuadNode" || quadNode.Mesh != 0 || len(quadNode.Children) != 1 || quadNode.Children[0] != 1 || quadNode.Matrix != nil {
		t.Errorf("unexpected node: %+v", quadNode)
	}
	if *quadNode.Translation != (schemas.Mesh3DSourceVector3T{X: 1, Y: 2, Z: 3}) ||
		*quadNode.Rotation != (schemas.Mesh3DSourceVector4T{X: 0, Y: 1, Z: 0, W: 0}) ||
		*quadNode.Scale != (schemas.Mesh3DSourceVector3T{X: 2, Y: 2, Z: 2}) {
		t.Errorf("unexpected transform: %+v, %+v, %+v", quadNode.Translation, quadNode.Rotation, quadNode.Scale)
	}

	// NOTE: A node without a transform has the identity.
	triNode := source.Nodes[1]
	if triNode.Mesh != 1 || *triNode.Translation != (schemas.Mesh3DSourceVector3T{}) ||
		*triNode.Rotation != (schemas.Mesh3DSourceVector4T{W: 1}) ||
		*triNode.Scale != (schemas.Mesh3DSourceVector3T{X: 1, Y: 1, Z: 1}) {
		t.Errorf("unexpected default transform: %+v", triNode)
	}

	matrixNode := source.Nodes[2]
	if matrixNode.Mesh != -1 || !equalFloats(matrixNode.Matrix, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 4, 5, 6, 1) {
		t.Errorf("unexpected matrix node: %+v", matrixNode)
	}

	if source.Scene != 0 || len(source.Scenes) != 1 || source.Scenes[0].Name != "Scene" {
		t.Fatalf("unexpected scenes: default %d, %+v", source.Scene, source.Scenes)
	}
	if nodes := source.Scenes[0].Nodes; len(nodes) != 2 || nodes[0] != 0 || nodes[1] != 2 {
		t.Errorf("unexpected scene nodes: %v", nodes)
	}
}

func TestConvertNotes(t *testing.T) {
	source := mustConvertSample(t)

	want := []string{
		`mesh "Quad": 1 morph targets are not supported and were dropped`,
		"2 animations are not supported and were dropped",
		"1 textures are not supported and were dropped",
	}
	if strings.Join(source.Notes, "\n") != strings.Join(want, "\n") {
		t.Errorf("got notes:\n%s\nwant:\n%s", strings.Join(source.Notes, "\n"), strings.Join(want, "\n"))
	}
}

func TestConvertAccessorWithoutBufferView(t *testing.T) {
	// NOTE: An accessor without a buffer view holds zeros.
	source, err := convertGLB(t, `{
  "asset": {"version": "2.0"},
  "meshes": [{"name": "Zero", "primitives": [{"attributes": {"POSITION": 0}, "indices": 1}]}],
  "accessors": [
    {"componentType": 5126, "count": 3, "type": "VEC3"},
    {"componentType": 5123, "count": 3, "type": "SCALAR"}
  ]
}`, nil)
	if err != nil {
		t.Fatalf("convert() failed: %v", err)
	}

	primitive := source.Meshes[0].Primitives[0]
	if !equalFloats(primitive.Attributes[0].Values, 0, 0, 0, 0, 0, 0, 0, 0, 0) {
		t.Errorf("unexpected positions: %v", primitive.Attributes[0].Values)
	}
	if len(primitive.Indices) != 3 || primitive.Indices[0] != 0 || primitive.Indices[2] != 0 {
		t.Errorf("unexpected indices: %v", primitive.Indices)
	}
	if source.Scene != -1 || source.Notes != nil {
		t.Errorf("unexpected scene %d and notes %v", source.Scene, source.Notes)
	}
}

func TestParseGLBErrors(t *testing.T) {
	valid := makeGLB(`{"asset": {"version": "2.0"}}`, []byte{1, 2, 3, 4})
	if _, _, err := parseGLB(valid); err != nil {
		t.Fatalf("parseGLB() failed: %v", err)
	}

	cases := []struct {
		name    string
		data    []byte
		wantErr string
	}{
		{"empty file", []byte{}, "too small"},
		{"text file", []byte(`{"asset": {"version": "2.0"}}`), "not a glTF binary file"},
		{"other version", func() []byte {
			data := append([]byte{}, valid...)
			binary.LittleEndian.PutUint32(data[4:], 1)
			return data
		}(), "version 1"},
		{"truncated file", valid[:len(valid)-4], "the header declares"},
		{"chunk longer than the file", func() []byte {
			data := append([]byte{}, valid...)
			binary.LittleEndian.PutUint32(data[12:], uint32(len(data)))
			return data
		}(), "chunk #0 is longer"},
		{"binary chunk first", func() []byte {
			data := append([]byte{}, valid...)
			binary.LittleEndian.PutUint32(data[16:], glbChunkBIN)
			return data
		}(), "the first chunk must be the JSON chunk"},
		{"header only", valid[:12], "the header declares"},
	}

	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			_, _, err := parseGLB(tc.data)
			if err == nil {
				t.Fatalf("parseGLB() succeeded, want an error with %q", tc.wantErr)
			}
			if !strings.Contains(err.Error(), tc.wantErr) {
				t.Errorf("got error %q, want it to have %q", err.Error(), tc.wantErr)
			}
		})
	}
}

func TestConvertErrors(t *testing.T) {
	// NOTE: Each case replaces one part of a valid document, whose binary
	// chunk holds three positions and three 16-bit indices.
	const template = `{
  "asset": {"version": VERSION},
  EXTRA
  "meshes": [{"name": "Tri", "primitives": [PRIMITIVE]}],
  "accessors": [ACCESSOR0, ACCESSOR1],
  "bufferViews": [VIEW0, {"buffer": 0, "byteOffset": 36, "byteLength": 6}],
  "buffers": [BINARY]
}`
	valid := map[string]string{
		"VERSION":   `"2.0"`,
		"EXTRA":     ``,
		"PRIMITIVE": `{"attributes": {"POSITION": 0}, "indices": 1}`,
		"ACCESSOR0": `{"bufferView": 0, "componentType": 5126, "count": 3, "type": "VEC3"}`,
		"ACCESSOR1": `{"bufferView": 1, "componentType": 5123, "count": 3, "type": "SCALAR"}`,
		"VIEW0":     `{"buffer": 0, "byteOffset": 0, "byteLength": 36}`,
		"BINARY":    `{"byteLength": 42}`,
	}
	bin := appendFloats(nil, 0, 0, 0, 1, 0, 0, 0, 1, 0)
	bin = appendUint16s(bin, 0, 1, 2)

	cases := []struct {
		name    string
		part    string
		value   string
		wantErr string
	}{
		{"valid document", "EXTRA", "", ""},
		{"glTF 1.0", "VERSION", `"1.0"`, `glTF version "1.0" is not supported`},
		{"required extension", "EXTRA", `"extensionsRequired": ["KHR_draco_mesh_compression"],`, `requires the extension "KHR_draco_mesh_compression"`},
		{"supported required extension", "EXTRA", `"extensionsRequired": ["KHR_mesh_quantization"],`, ""},
		{"no position", "PRIMITIVE", `{"attributes": {"NORMAL": 0}}`, `no "POSITION" attribute`},
		{"unknown accessor", "PRIMITIVE", `{"attributes": {"POSITION": 7}}`, "accessor #7 does not exist"},
		{"unknown material", "PRIMITIVE", `{"attributes": {"POSITION": 0}, "material": 0}`, "material #0 does not exist"},
		{"unknown mode", "PRIMITIVE", `{"attributes": {"POSITION": 0}, "mode": 7}`, "mode 7 is not supported"},
		{"indices of vectors", "PRIMITIVE", `{"attributes": {"POSITION": 0}, "indices": 0}`, "is not a scalar"},
		{"indices of floats", "ACCESSOR1", `{"bufferView": 1, "componentType": 5126, "count": 1, "type": "SCALAR"}`, "component type 5126 is not an unsigned integer"},
		{"matrix accessor", "ACCESSOR0", `{"bufferView": 0, "componentType": 5126, "count": 1, "type": "MAT4"}`, `type "MAT4" is not supported`},
		{"unknown component type", "ACCESSOR0", `{"bufferView": 0, "componentType": 5124, "count": 3, "type": "VEC3"}`, "component type 5124 is not supported"},
		{"sparse accessor", "ACCESSOR0", `{"bufferView": 0, "componentType": 5126, "count": 3, "type": "VEC3", "sparse": {"count": 1}}`, "sparse accessors are not supported"},
		{"accessor out of its buffer view", "ACCESSOR0", `{"bufferView": 0, "componentType": 5126, "count": 4, "type": "VEC3"}`, "accessor #0 is out of buffer view #0"},
		{"negative count", "ACCESSOR0", `{"bufferView": 0, "componentType": 5126, "count": -1, "type": "VEC3"}`, "count -1 is out of range"},
		{"unknown buffer view", "ACCESSOR0", `{"bufferView": 9, "componentType": 5126, "count": 3, "type": "VEC3"}`, "buffer view #9 does not exist"},
		{"index out of range", "ACCESSOR0", `{"bufferView": 0, "componentType": 5126, "count": 2, "type": "VEC3"}`, "index 2 is out of the 2 vertices"},
		{"normalized 32-bit integer", "ACCESSOR0", `{"bufferView": 0, "componentType": 5125, "normalized": true, "count": 3, "type": "VEC3"}`, "a 32-bit integer cannot be normalized"},
		{"buffer view out of the binary chunk", "VIEW0", `{"buffer": 0, "byteOffset": 8, "byteLength": 64}`, "buffer view #0 is out of the binary chunk"},
		{"stride smaller than an element", "VIEW0", `{"buffer": 0, "byteOffset": 0, "byteLength": 36, "byteStride": 8}`, "byte stride 8 is out of range"},
		{"external buffer", "BINARY", `{"byteLength": 42, "uri": "mesh.bin"}`, "external buffers are not supported"},
	}

	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			source := template
			for part, value := range valid {
				if part == tc.part {
					value = tc.value
				}
				source = strings.Replace(source, part, value, 1)
			}

			_, err := convertGLB(t, source, bin)
			if tc.wantErr == "" {
				if err != nil {
					t.Fatalf("convert() failed: %v", err)
				}
				return
			}
			if err == nil {
				t.Fatalf("convert() succeeded, want an error with %q", tc.wantErr)
			}
			if !strings.Contains(err.Error(), tc.wantErr) {
				t.Errorf("got error %q, want it to have %q", err.Error(), tc.wantErr)
			}
		})
	}
}

func TestConvertRejectsAttributeOfAnotherCount(t *testing.T) {
	bin := appendFloats(nil, 0, 0, 0, 1, 0, 0, 0, 1, 0)
	_, err := convertGLB(t, `{
  "asset": {"version": "2.0"},
  "meshes": [{"name": "Tri", "primitives": [{"attributes": {"POSITION": 0, "NORMAL": 1}}]}],
  "accessors": [
    {"bufferView": 0, "componentType": 5126, "count": 3, "type": "VEC3"},
    {"bufferView": 0, "componentType": 5126, "count": 2, "type": "VEC3"}
  ],
  "bufferViews": [{"buffer": 0, "byteLength": 36}],
  "buffers": [{"byteLength": 36}]
}`, bin)
	if err == nil || !strings.Contains(err.Error(), `attribute "NORMAL" has 2 values, but the primitive has 3 vertices`) {
		t.Errorf("got error %v, want one about the attribute count", err)
	}
}

func TestRunWritesFile(t *testing.T) {
	dir := t.TempDir()
	opts := &options{
		InFile:  filepath.Join(dir, "in.glb"),
		OutFile: filepath.Join(dir, "out", "model.mesh3dsource"),
	}
	if err := os.WriteFile(opts.InFile, makeGLB(sampleJSON, makeSampleBinary()), 0o644); err != nil {
		t.Fatalf("os.WriteFile() failed: %v", err)
	}
	if err := run(opts); err != nil {
		t.Fatalf("run() failed: %v", err)
	}

	data, err := os.ReadFile(opts.OutFile)
	if err != nil {
		t.Fatalf("os.ReadFile() failed: %v", err)
	}
	source := schemas.GetRootAsMesh3DSource(data, 0).UnPack()
	if len(source.Meshes) != 2 || source.Meshes[0].Name != "Quad" {
		t.Errorf("unexpected meshes in the output: %d", len(source.Meshes))
	}

	// NOTE: The same input gives the same bytes.
	first := string(data)
	if err := run(opts); err != nil {
		t.Fatalf("run() failed: %v", err)
	}
	again, err := os.ReadFile(opts.OutFile)
	if err != nil {
		t.Fatalf("os.ReadFile() failed: %v", err)
	}
	if string(again) != first {
		t.Errorf("the second run produced different bytes")
	}

	t.Run("a missing input fails", func(t *testing.T) {
		opts := *opts
		opts.InFile = filepath.Join(dir, "missing.glb")
		if err := run(&opts); err == nil {
			t.Errorf("run() succeeded, want an error")
		}
	})
}
