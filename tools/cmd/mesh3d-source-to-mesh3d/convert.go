// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"encoding/binary"
	"errors"
	"fmt"
	"math"
	"sort"
	"strconv"
	"strings"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

const (
	// bytesPerFloat is the size of a component of a vertex element.
	bytesPerFloat = 4

	// maxUInt16VertexCount is the number of vertices that 16-bit indices
	// can address.
	maxUInt16VertexCount = 1 << 16

	// unassigned marks a source vertex that no index has referred to yet.
	unassigned = math.MaxUint32
)

// output holds the root tables and the raw bytes of the output files.
type output struct {
	Mesh     *schemas.Mesh3DT
	Vertices []byte
	Indices  []byte
	Debug    *schemas.Mesh3DDebugT
}

// element is an attribute of a primitive that goes into the vertices.
type element struct {
	name           string
	semantic       schemas.Mesh3DVertexSemantic
	semanticIndex  uint8
	componentCount int
	values         []float32
}

// vertexBuffer collects the vertices of the primitives that have the same
// elements, without duplicates.
type vertexBuffer struct {
	elements     []*schemas.Mesh3DVertexElementT
	elementNames []string
	strideBytes  int
	data         []byte

	// lookup maps the bytes of a vertex to its index in the buffer.
	lookup map[string]uint32

	vertexCount       uint32
	sourceVertexCount uint32
}

// converter holds the buffers that the meshes of one source share.
type converter struct {
	source *schemas.Mesh3DSourceT

	// vertexBuffers is in the order in which the meshes first use a layout,
	// and layouts maps a layout to its index in vertexBuffers.
	vertexBuffers []*vertexBuffer
	layouts       map[string]int

	indices []uint32
	notes   []string
}

// convert lays the vertices of source out for the GPU: it interleaves the
// attributes as floats, merges identical vertices, drops the vertices that no
// index refers to, and picks the smallest index type.
func convert(source *schemas.Mesh3DSourceT) (*output, error) {
	if len(source.Meshes) == 0 {
		return nil, errors.New("the source has no meshes")
	}

	c := &converter{
		source:  source,
		layouts: map[string]int{},
	}

	// NOTE: The debug file also carries the notes of the importer.
	c.notes = append(c.notes, source.Notes...)

	// NOTE: The runtime finds a mesh by the hash of its name, so every mesh
	// needs a name of its own.
	type entry struct {
		key  uint32
		mesh *schemas.Mesh3DSourceMeshT
	}
	entries := make([]entry, 0, len(source.Meshes))
	names := map[uint32]string{}
	for i, mesh := range source.Meshes {
		if mesh.Name == "" {
			return nil, fmt.Errorf("mesh #%d has no name", i)
		}
		key := stringhash.StringToHash32(mesh.Name)
		if other, ok := names[key]; ok {
			if other == mesh.Name {
				return nil, fmt.Errorf("mesh %q is defined more than once", mesh.Name)
			}
			return nil, fmt.Errorf("hash collision: XXH32 key %#x for mesh %q and %q", key, other, mesh.Name)
		}
		names[key] = mesh.Name
		entries = append(entries, entry{key: key, mesh: mesh})
	}

	// NOTE: Sort by key for binary search.
	sort.Slice(entries, func(a, b int) bool {
		return entries[a].key < entries[b].key
	})

	out := &output{
		Mesh: &schemas.Mesh3DT{},
		Debug: &schemas.Mesh3DDebugT{
			Generator:    source.Generator,
			SourceFormat: source.SourceFormat,
		},
	}

	for _, e := range entries {
		mesh, meshDebug, err := c.convertMesh(e.mesh)
		if err != nil {
			return nil, fmt.Errorf("mesh %q: %w", e.mesh.Name, err)
		}
		meshDebug.Key = e.key
		out.Mesh.Keys = append(out.Mesh.Keys, e.key)
		out.Mesh.Meshes = append(out.Mesh.Meshes, mesh)
		out.Debug.Meshes = append(out.Debug.Meshes, meshDebug)
	}

	if len(c.vertexBuffers) > math.MaxUint16 {
		return nil, fmt.Errorf("the source needs %d vertex buffers, but the limit is %d", len(c.vertexBuffers), math.MaxUint16)
	}
	if uint64(len(c.indices)) > math.MaxUint32 {
		return nil, fmt.Errorf("the source has %d indices, but the limit is %d", len(c.indices), uint32(math.MaxUint32))
	}

	// NOTE: The vertices file holds the vertex buffers one after another.
	maxVertexCount := uint32(0)
	for _, buffer := range c.vertexBuffers {
		if uint64(len(out.Vertices))+uint64(len(buffer.data)) > math.MaxUint32 {
			return nil, fmt.Errorf("the vertices take more than %d bytes", uint32(math.MaxUint32))
		}
		out.Mesh.VertexBuffers = append(out.Mesh.VertexBuffers, &schemas.Mesh3DVertexBufferT{
			ByteOffset:  uint32(len(out.Vertices)),
			VertexCount: buffer.vertexCount,
			StrideBytes: uint16(buffer.strideBytes),
			Elements:    buffer.elements,
		})
		out.Debug.VertexBuffers = append(out.Debug.VertexBuffers, &schemas.Mesh3DVertexBufferDebugT{
			Elements:          buffer.elementNames,
			SourceVertexCount: buffer.sourceVertexCount,
			VertexCount:       buffer.vertexCount,
		})
		out.Vertices = append(out.Vertices, buffer.data...)
		maxVertexCount = max(maxVertexCount, buffer.vertexCount)
	}

	// NOTE: An index counts from the start of its vertex buffer, so the
	// largest buffer decides the index type.
	out.Mesh.IndexCount = uint32(len(c.indices))
	if maxVertexCount <= maxUInt16VertexCount {
		out.Mesh.IndexFormat = schemas.Mesh3DIndexFormatUInt16
		out.Indices = make([]byte, 0, len(c.indices)*2)
		for _, index := range c.indices {
			out.Indices = binary.LittleEndian.AppendUint16(out.Indices, uint16(index))
		}
	} else {
		out.Mesh.IndexFormat = schemas.Mesh3DIndexFormatUInt32
		out.Indices = make([]byte, 0, len(c.indices)*4)
		for _, index := range c.indices {
			out.Indices = binary.LittleEndian.AppendUint32(out.Indices, index)
		}
	}

	out.Debug.Notes = c.notes
	return out, nil
}

func (c *converter) notef(format string, args ...any) {
	c.notes = append(c.notes, fmt.Sprintf(format, args...))
}

func (c *converter) convertMesh(mesh *schemas.Mesh3DSourceMeshT) (*schemas.Mesh3DMeshT, *schemas.Mesh3DMeshDebugT, error) {
	if len(mesh.Primitives) == 0 {
		return nil, nil, errors.New("the mesh has no primitives")
	}

	converted := &schemas.Mesh3DMeshT{}
	debug := &schemas.Mesh3DMeshDebugT{Name: mesh.Name}

	for i, primitive := range mesh.Primitives {
		part, partDebug, err := c.convertPrimitive(mesh.Name, i, primitive)
		if err != nil {
			return nil, nil, fmt.Errorf("primitive #%d: %w", i, err)
		}
		converted.Parts = append(converted.Parts, part)
		debug.Parts = append(debug.Parts, partDebug)
	}
	return converted, debug, nil
}

func (c *converter) convertPrimitive(meshName string, primitiveIndex int, primitive *schemas.Mesh3DSourcePrimitiveT) (*schemas.Mesh3DPartT, *schemas.Mesh3DPartDebugT, error) {
	// NOTE: The runtime draws lists of triangles only.
	if primitive.Topology != schemas.Mesh3DSourceTopologyTriangles {
		return nil, nil, fmt.Errorf("topology %s is not supported, only Triangles is", primitive.Topology)
	}

	partDebug := &schemas.Mesh3DPartDebugT{
		SourceVertexCount: primitive.VertexCount,
		SourceIndexCount:  uint32(len(primitive.Indices)),
	}
	if primitive.Material >= 0 {
		if int(primitive.Material) >= len(c.source.Materials) {
			return nil, nil, fmt.Errorf("material #%d does not exist", primitive.Material)
		}
		partDebug.Material = c.source.Materials[primitive.Material].Name
	}

	vertexCount := int(primitive.VertexCount)
	elements, err := c.collectElements(meshName, primitiveIndex, primitive.Attributes, vertexCount)
	if err != nil {
		return nil, nil, err
	}
	bufferIndex, err := c.findVertexBuffer(elements)
	if err != nil {
		return nil, nil, err
	}
	buffer := c.vertexBuffers[bufferIndex]

	// NOTE: A primitive without indices draws its vertices in order.
	indexCount := len(primitive.Indices)
	if indexCount == 0 {
		indexCount = vertexCount
	}
	if indexCount == 0 || indexCount%3 != 0 {
		return nil, nil, fmt.Errorf("triangles need three indices each, but the primitive has %d", indexCount)
	}

	part := &schemas.Mesh3DPartT{
		StartIndex:   uint32(len(c.indices)),
		IndexCount:   uint32(indexCount),
		VertexBuffer: uint16(bufferIndex),
	}

	// NOTE: remap maps a vertex of the primitive to its index in the vertex
	// buffer. Vertices get their index in the order in which the indices
	// refer to them, so a vertex that no index refers to is left out.
	remap := make([]uint32, vertexCount)
	for i := range remap {
		remap[i] = unassigned
	}
	vertex := make([]byte, buffer.strideBytes)

	for i := 0; i < indexCount; i++ {
		sourceIndex := i
		if len(primitive.Indices) != 0 {
			sourceIndex = int(primitive.Indices[i])
			if sourceIndex >= vertexCount {
				return nil, nil, fmt.Errorf("index %d is out of the %d vertices", sourceIndex, vertexCount)
			}
		}

		if remap[sourceIndex] == unassigned {
			offset := 0
			for _, e := range elements {
				for _, value := range e.values[sourceIndex*e.componentCount : (sourceIndex+1)*e.componentCount] {
					binary.LittleEndian.PutUint32(vertex[offset:], math.Float32bits(value))
					offset += bytesPerFloat
				}
			}

			// NOTE: Vertices with the same bytes share one vertex of the buffer.
			index, ok := buffer.lookup[string(vertex)]
			if !ok {
				if buffer.vertexCount == math.MaxUint32 {
					return nil, nil, errors.New("the vertex buffer has too many vertices")
				}
				index = buffer.vertexCount
				buffer.lookup[string(vertex)] = index
				buffer.data = append(buffer.data, vertex...)
				buffer.vertexCount++
			}
			remap[sourceIndex] = index
		}
		c.indices = append(c.indices, remap[sourceIndex])
	}

	buffer.sourceVertexCount += primitive.VertexCount
	return part, partDebug, nil
}

// collectElements returns the attributes that the vertices hold, in the order
// of their semantic and then their semantic index.
func (c *converter) collectElements(meshName string, primitiveIndex int, attributes []*schemas.Mesh3DSourceAttributeT, vertexCount int) ([]*element, error) {
	elements := []*element{}
	hasPosition := false

	for _, attribute := range attributes {
		semantic, semanticIndex, ok := parseSemantic(attribute.Semantic)
		if !ok {
			c.notef("mesh %q: primitive #%d: attribute %q is not supported and was dropped", meshName, primitiveIndex, attribute.Semantic)
			continue
		}

		componentCount := int(attribute.ComponentCount)
		if componentCount < 1 || componentCount > 4 {
			return nil, fmt.Errorf("attribute %q has %d components, but a vertex element holds 1 to 4", attribute.Semantic, componentCount)
		}
		if len(attribute.Values) != componentCount*vertexCount {
			return nil, fmt.Errorf("attribute %q has %d values, but %d vertices need %d", attribute.Semantic, len(attribute.Values), vertexCount, componentCount*vertexCount)
		}
		for _, e := range elements {
			if e.semantic == semantic && e.semanticIndex == semanticIndex {
				return nil, fmt.Errorf("attribute %q is defined more than once", attribute.Semantic)
			}
		}

		if semantic == schemas.Mesh3DVertexSemanticPosition {
			hasPosition = true
		}
		elements = append(elements, &element{
			name:           attribute.Semantic,
			semantic:       semantic,
			semanticIndex:  semanticIndex,
			componentCount: componentCount,
			values:         attribute.Values,
		})
	}

	if !hasPosition {
		return nil, errors.New(`the primitive has no "POSITION" attribute`)
	}

	sort.SliceStable(elements, func(a, b int) bool {
		if elements[a].semantic != elements[b].semantic {
			return elements[a].semantic < elements[b].semantic
		}
		return elements[a].semanticIndex < elements[b].semanticIndex
	})
	return elements, nil
}

// findVertexBuffer returns the index of the vertex buffer whose vertices have
// the given elements, and adds the buffer when the layout is new.
func (c *converter) findVertexBuffer(elements []*element) (int, error) {
	parts := make([]string, 0, len(elements))
	for _, e := range elements {
		parts = append(parts, fmt.Sprintf("%d:%d:%d", e.semantic, e.semanticIndex, e.componentCount))
	}
	layout := strings.Join(parts, ",")

	if index, ok := c.layouts[layout]; ok {
		return index, nil
	}

	buffer := &vertexBuffer{lookup: map[string]uint32{}}
	for _, e := range elements {
		buffer.elements = append(buffer.elements, &schemas.Mesh3DVertexElementT{
			ByteOffset:    uint16(buffer.strideBytes),
			Semantic:      e.semantic,
			SemanticIndex: e.semanticIndex,
			// NOTE: The formats are numbered by their component count, from Float32x1.
			Format: schemas.Mesh3DVertexElementFormat(e.componentCount - 1),
		})
		buffer.elementNames = append(buffer.elementNames, e.name)
		buffer.strideBytes += e.componentCount * bytesPerFloat
	}
	if buffer.strideBytes > math.MaxUint16 {
		return 0, fmt.Errorf("a vertex takes %d bytes, but the limit is %d", buffer.strideBytes, math.MaxUint16)
	}

	index := len(c.vertexBuffers)
	c.vertexBuffers = append(c.vertexBuffers, buffer)
	c.layouts[layout] = index
	return index, nil
}

// parseSemantic maps the attribute name of a mesh source to the semantic of a
// vertex element. The names are those of glTF: "POSITION", "NORMAL",
// "TANGENT", and "TEXCOORD", "COLOR", "JOINTS", and "WEIGHTS" followed by an
// underscore and a set index. It reports false for any other name.
func parseSemantic(name string) (schemas.Mesh3DVertexSemantic, uint8, bool) {
	switch name {
	case "POSITION":
		return schemas.Mesh3DVertexSemanticPosition, 0, true
	case "NORMAL":
		return schemas.Mesh3DVertexSemanticNormal, 0, true
	case "TANGENT":
		return schemas.Mesh3DVertexSemanticTangent, 0, true
	}

	prefix, suffix, found := strings.Cut(name, "_")
	if !found {
		return 0, 0, false
	}
	index, err := strconv.ParseUint(suffix, 10, 8)
	if err != nil || strconv.FormatUint(index, 10) != suffix {
		return 0, 0, false
	}

	switch prefix {
	case "TEXCOORD":
		return schemas.Mesh3DVertexSemanticTexCoord, uint8(index), true
	case "COLOR":
		return schemas.Mesh3DVertexSemanticColor, uint8(index), true
	case "JOINTS":
		return schemas.Mesh3DVertexSemanticJoints, uint8(index), true
	case "WEIGHTS":
		return schemas.Mesh3DVertexSemanticWeights, uint8(index), true
	}
	return 0, 0, false
}
