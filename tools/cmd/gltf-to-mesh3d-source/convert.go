// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"encoding/binary"
	"errors"
	"fmt"
	"math"
	"sort"
	"strings"

	schemas "github.com/mogemimi/pomdog/build/schemas"
)

// NOTE: The component types of glTF accessors.
const (
	componentTypeByte          = 5120
	componentTypeUnsignedByte  = 5121
	componentTypeShort         = 5122
	componentTypeUnsignedShort = 5123
	componentTypeUnsignedInt   = 5125
	componentTypeFloat         = 5126
)

// maxByteStride is the largest stride that glTF allows for a buffer view.
const maxByteStride = 252

// supportedRequiredExtensions lists the extensions that a file can require
// without changing how the converter reads its meshes.
var supportedRequiredExtensions = map[string]bool{
	// NOTE: Lets attributes use integer types, which the converter decodes anyway.
	"KHR_mesh_quantization": true,
}

// converter holds a glTF document and the binary chunk that its buffer views
// refer to.
type converter struct {
	doc   *gltfDocument
	bin   []byte
	notes []string
}

// convert decodes the meshes of doc into arrays of floats and indices, and
// copies what doc says about materials, nodes, and scenes.
func convert(doc *gltfDocument, bin []byte) (*schemas.Mesh3DSourceT, error) {
	if !strings.HasPrefix(doc.Asset.Version, "2.") {
		return nil, fmt.Errorf("glTF version %q is not supported", doc.Asset.Version)
	}
	for _, extension := range doc.ExtensionsRequired {
		if !supportedRequiredExtensions[extension] {
			return nil, fmt.Errorf("the file requires the extension %q, which is not supported", extension)
		}
	}

	c := &converter{doc: doc, bin: bin}
	source := &schemas.Mesh3DSourceT{
		Generator:    doc.Asset.Generator,
		SourceFormat: "glTF " + doc.Asset.Version,
		Scene:        -1,
	}

	for i, material := range doc.Materials {
		converted, err := convertMaterial(material)
		if err != nil {
			return nil, fmt.Errorf("material #%d: %w", i, err)
		}
		source.Materials = append(source.Materials, converted)
	}

	for i, mesh := range doc.Meshes {
		converted, err := c.convertMesh(mesh)
		if err != nil {
			return nil, fmt.Errorf("mesh #%d: %w", i, err)
		}
		source.Meshes = append(source.Meshes, converted)
	}

	for i, node := range doc.Nodes {
		converted, err := c.convertNode(i, node)
		if err != nil {
			return nil, fmt.Errorf("node #%d: %w", i, err)
		}
		source.Nodes = append(source.Nodes, converted)
	}

	for i, scene := range doc.Scenes {
		if scene == nil {
			return nil, fmt.Errorf("scene #%d is null", i)
		}
		nodes, err := toIndices(scene.Nodes, len(doc.Nodes), "node")
		if err != nil {
			return nil, fmt.Errorf("scene #%d: %w", i, err)
		}
		source.Scenes = append(source.Scenes, &schemas.Mesh3DSourceSceneT{Name: scene.Name, Nodes: nodes})
	}

	if doc.Scene != nil {
		if *doc.Scene < 0 || *doc.Scene >= len(doc.Scenes) {
			return nil, fmt.Errorf("the default scene #%d does not exist", *doc.Scene)
		}
		source.Scene = int32(*doc.Scene)
	}

	// NOTE: Report what the mesh source cannot hold yet.
	dropped := []struct {
		name  string
		count int
	}{
		{"animations", len(doc.Animations)},
		{"cameras", len(doc.Cameras)},
		{"images", len(doc.Images)},
		{"samplers", len(doc.Samplers)},
		{"skins", len(doc.Skins)},
		{"textures", len(doc.Textures)},
	}
	for _, d := range dropped {
		if d.count > 0 {
			c.notef("%d %s are not supported and were dropped", d.count, d.name)
		}
	}

	source.Notes = c.notes
	return source, nil
}

func (c *converter) notef(format string, args ...any) {
	c.notes = append(c.notes, fmt.Sprintf(format, args...))
}

func convertMaterial(material *gltfMaterial) (*schemas.Mesh3DSourceMaterialT, error) {
	if material == nil {
		return nil, errors.New("the material is null")
	}

	// NOTE: The values below are the defaults of glTF.
	converted := &schemas.Mesh3DSourceMaterialT{
		Name:            material.Name,
		BaseColorFactor: &schemas.Mesh3DSourceVector4T{X: 1, Y: 1, Z: 1, W: 1},
		MetallicFactor:  1,
		RoughnessFactor: 1,
		EmissiveFactor:  &schemas.Mesh3DSourceVector3T{},
		DoubleSided:     material.DoubleSided,
	}

	if pbr := material.PBR; pbr != nil {
		if len(pbr.BaseColorFactor) != 0 {
			if len(pbr.BaseColorFactor) != 4 {
				return nil, fmt.Errorf(`"baseColorFactor" must have 4 numbers, but has %d`, len(pbr.BaseColorFactor))
			}
			converted.BaseColorFactor = toVector4(pbr.BaseColorFactor)
		}
		if pbr.MetallicFactor != nil {
			converted.MetallicFactor = float32(*pbr.MetallicFactor)
		}
		if pbr.RoughnessFactor != nil {
			converted.RoughnessFactor = float32(*pbr.RoughnessFactor)
		}
	}
	if len(material.EmissiveFactor) != 0 {
		if len(material.EmissiveFactor) != 3 {
			return nil, fmt.Errorf(`"emissiveFactor" must have 3 numbers, but has %d`, len(material.EmissiveFactor))
		}
		converted.EmissiveFactor = toVector3(material.EmissiveFactor)
	}
	return converted, nil
}

func (c *converter) convertNode(index int, node *gltfNode) (*schemas.Mesh3DSourceNodeT, error) {
	if node == nil {
		return nil, errors.New("the node is null")
	}

	// NOTE: The values below are the defaults of glTF.
	converted := &schemas.Mesh3DSourceNodeT{
		Name:        node.Name,
		Mesh:        -1,
		Translation: &schemas.Mesh3DSourceVector3T{},
		Rotation:    &schemas.Mesh3DSourceVector4T{W: 1},
		Scale:       &schemas.Mesh3DSourceVector3T{X: 1, Y: 1, Z: 1},
	}

	children, err := toIndices(node.Children, len(c.doc.Nodes), "node")
	if err != nil {
		return nil, err
	}
	for _, child := range children {
		if int(child) == index {
			return nil, errors.New("the node is a child of itself")
		}
	}
	converted.Children = children

	if node.Mesh != nil {
		if *node.Mesh < 0 || *node.Mesh >= len(c.doc.Meshes) {
			return nil, fmt.Errorf("mesh #%d does not exist", *node.Mesh)
		}
		converted.Mesh = int32(*node.Mesh)
	}

	if len(node.Matrix) != 0 {
		if len(node.Matrix) != 16 {
			return nil, fmt.Errorf(`"matrix" must have 16 numbers, but has %d`, len(node.Matrix))
		}
		for _, value := range node.Matrix {
			converted.Matrix = append(converted.Matrix, float32(value))
		}
		return converted, nil
	}

	if len(node.Translation) != 0 {
		if len(node.Translation) != 3 {
			return nil, fmt.Errorf(`"translation" must have 3 numbers, but has %d`, len(node.Translation))
		}
		converted.Translation = toVector3(node.Translation)
	}
	if len(node.Rotation) != 0 {
		if len(node.Rotation) != 4 {
			return nil, fmt.Errorf(`"rotation" must have 4 numbers, but has %d`, len(node.Rotation))
		}
		converted.Rotation = toVector4(node.Rotation)
	}
	if len(node.Scale) != 0 {
		if len(node.Scale) != 3 {
			return nil, fmt.Errorf(`"scale" must have 3 numbers, but has %d`, len(node.Scale))
		}
		converted.Scale = toVector3(node.Scale)
	}
	return converted, nil
}

func (c *converter) convertMesh(mesh *gltfMesh) (*schemas.Mesh3DSourceMeshT, error) {
	if mesh == nil {
		return nil, errors.New("the mesh is null")
	}

	converted := &schemas.Mesh3DSourceMeshT{Name: mesh.Name}
	targetCount := 0
	for i, primitive := range mesh.Primitives {
		if primitive == nil {
			return nil, fmt.Errorf("primitive #%d is null", i)
		}
		convertedPrimitive, err := c.convertPrimitive(primitive)
		if err != nil {
			return nil, fmt.Errorf("primitive #%d: %w", i, err)
		}
		converted.Primitives = append(converted.Primitives, convertedPrimitive)
		targetCount += len(primitive.Targets)
	}

	if targetCount > 0 {
		c.notef("mesh %q: %d morph targets are not supported and were dropped", mesh.Name, targetCount)
	}
	return converted, nil
}

func (c *converter) convertPrimitive(primitive *gltfPrimitive) (*schemas.Mesh3DSourcePrimitiveT, error) {
	converted := &schemas.Mesh3DSourcePrimitiveT{
		Material: -1,
		Topology: schemas.Mesh3DSourceTopologyTriangles,
	}

	if primitive.Mode != nil {
		// NOTE: The topologies have the values of the glTF modes.
		if *primitive.Mode < int(schemas.Mesh3DSourceTopologyPoints) || *primitive.Mode > int(schemas.Mesh3DSourceTopologyTriangleFan) {
			return nil, fmt.Errorf("mode %d is not supported", *primitive.Mode)
		}
		converted.Topology = schemas.Mesh3DSourceTopology(*primitive.Mode)
	}

	if primitive.Material != nil {
		if *primitive.Material < 0 || *primitive.Material >= len(c.doc.Materials) {
			return nil, fmt.Errorf("material #%d does not exist", *primitive.Material)
		}
		converted.Material = int32(*primitive.Material)
	}

	positionAccessor, ok := primitive.Attributes["POSITION"]
	if !ok {
		return nil, errors.New(`the primitive has no "POSITION" attribute`)
	}
	position, err := c.accessorView(positionAccessor)
	if err != nil {
		return nil, fmt.Errorf(`attribute "POSITION": %w`, err)
	}
	vertexCount := position.accessor.Count
	converted.VertexCount = uint32(vertexCount)

	// NOTE: A Go map has no order, so emit the attributes sorted by name.
	names := make([]string, 0, len(primitive.Attributes))
	for name := range primitive.Attributes {
		names = append(names, name)
	}
	sort.Strings(names)

	for _, name := range names {
		view, err := c.accessorView(primitive.Attributes[name])
		if err != nil {
			return nil, fmt.Errorf("attribute %q: %w", name, err)
		}
		if view.accessor.Count != vertexCount {
			return nil, fmt.Errorf("attribute %q has %d values, but the primitive has %d vertices", name, view.accessor.Count, vertexCount)
		}
		values, err := view.readFloats()
		if err != nil {
			return nil, fmt.Errorf("attribute %q: %w", name, err)
		}
		converted.Attributes = append(converted.Attributes, &schemas.Mesh3DSourceAttributeT{
			Semantic:            name,
			ComponentCount:      uint8(view.componentCount),
			Values:              values,
			SourceComponentType: view.sourceComponentType(),
			SourceNormalized:    view.accessor.Normalized,
		})
	}

	if primitive.Indices != nil {
		view, err := c.accessorView(*primitive.Indices)
		if err != nil {
			return nil, fmt.Errorf("indices: %w", err)
		}
		indices, err := view.readIndices()
		if err != nil {
			return nil, fmt.Errorf("indices: %w", err)
		}
		for _, index := range indices {
			if uint64(index) >= uint64(vertexCount) {
				return nil, fmt.Errorf("indices: index %d is out of the %d vertices", index, vertexCount)
			}
		}
		converted.Indices = indices
	}

	return converted, nil
}

// accessorView tells where the elements of an accessor are in the binary chunk.
type accessorView struct {
	accessor       *gltfAccessor
	componentCount int
	componentSize  int

	// data starts at the first element, and is nil for an accessor without
	// a buffer view, whose elements are all zero.
	data []byte

	// stride is the distance in bytes between two elements.
	stride int
}

func (c *converter) accessorView(index int) (*accessorView, error) {
	if index < 0 || index >= len(c.doc.Accessors) || c.doc.Accessors[index] == nil {
		return nil, fmt.Errorf("accessor #%d does not exist", index)
	}
	accessor := c.doc.Accessors[index]

	if sparse := bytes.TrimSpace(accessor.Sparse); len(sparse) != 0 && !bytes.Equal(sparse, []byte("null")) {
		return nil, fmt.Errorf("accessor #%d: sparse accessors are not supported", index)
	}

	view := &accessorView{accessor: accessor}
	switch accessor.Type {
	case "SCALAR":
		view.componentCount = 1
	case "VEC2":
		view.componentCount = 2
	case "VEC3":
		view.componentCount = 3
	case "VEC4":
		view.componentCount = 4
	default:
		return nil, fmt.Errorf("accessor #%d: type %q is not supported", index, accessor.Type)
	}

	switch accessor.ComponentType {
	case componentTypeByte, componentTypeUnsignedByte:
		view.componentSize = 1
	case componentTypeShort, componentTypeUnsignedShort:
		view.componentSize = 2
	case componentTypeUnsignedInt, componentTypeFloat:
		view.componentSize = 4
	default:
		return nil, fmt.Errorf("accessor #%d: component type %d is not supported", index, accessor.ComponentType)
	}

	if accessor.Count < 0 || accessor.Count > math.MaxUint32 {
		return nil, fmt.Errorf("accessor #%d: count %d is out of range", index, accessor.Count)
	}
	if accessor.BufferView == nil {
		return view, nil
	}

	bufferViewIndex := *accessor.BufferView
	if bufferViewIndex < 0 || bufferViewIndex >= len(c.doc.BufferViews) || c.doc.BufferViews[bufferViewIndex] == nil {
		return nil, fmt.Errorf("accessor #%d: buffer view #%d does not exist", index, bufferViewIndex)
	}
	bufferView := c.doc.BufferViews[bufferViewIndex]

	// NOTE: Only the binary chunk of the file is read, which is buffer #0
	// without a URI.
	if bufferView.Buffer != 0 || len(c.doc.Buffers) == 0 || c.doc.Buffers[0] == nil || c.doc.Buffers[0].URI != nil {
		return nil, fmt.Errorf("buffer view #%d: buffer #%d is not the binary chunk of the file, and external buffers are not supported", bufferViewIndex, bufferView.Buffer)
	}
	if bufferView.ByteOffset < 0 || bufferView.ByteLength < 0 || bufferView.ByteOffset > len(c.bin) || bufferView.ByteLength > len(c.bin)-bufferView.ByteOffset {
		return nil, fmt.Errorf("buffer view #%d is out of the binary chunk", bufferViewIndex)
	}
	viewBytes := c.bin[bufferView.ByteOffset : bufferView.ByteOffset+bufferView.ByteLength]

	elementSize := view.componentCount * view.componentSize
	view.stride = bufferView.ByteStride
	if view.stride == 0 {
		view.stride = elementSize
	}
	if view.stride < elementSize || view.stride > maxByteStride {
		return nil, fmt.Errorf("buffer view #%d: byte stride %d is out of range", bufferViewIndex, bufferView.ByteStride)
	}

	if accessor.ByteOffset < 0 || accessor.ByteOffset > len(viewBytes) {
		return nil, fmt.Errorf("accessor #%d is out of buffer view #%d", index, bufferViewIndex)
	}
	view.data = viewBytes[accessor.ByteOffset:]
	if accessor.Count > 0 && (accessor.Count-1)*view.stride+elementSize > len(view.data) {
		return nil, fmt.Errorf("accessor #%d is out of buffer view #%d", index, bufferViewIndex)
	}
	return view, nil
}

func (view *accessorView) sourceComponentType() schemas.Mesh3DSourceComponentType {
	switch view.accessor.ComponentType {
	case componentTypeByte:
		return schemas.Mesh3DSourceComponentTypeInt8
	case componentTypeUnsignedByte:
		return schemas.Mesh3DSourceComponentTypeUInt8
	case componentTypeShort:
		return schemas.Mesh3DSourceComponentTypeInt16
	case componentTypeUnsignedShort:
		return schemas.Mesh3DSourceComponentTypeUInt16
	case componentTypeUnsignedInt:
		return schemas.Mesh3DSourceComponentTypeUInt32
	}
	return schemas.Mesh3DSourceComponentTypeFloat32
}

// readFloats returns the components of every element as floats.
func (view *accessorView) readFloats() ([]float32, error) {
	accessor := view.accessor
	if accessor.Normalized && accessor.ComponentType == componentTypeUnsignedInt {
		return nil, errors.New("a 32-bit integer cannot be normalized")
	}

	values := make([]float32, accessor.Count*view.componentCount)
	if view.data == nil {
		return values, nil
	}

	for i := 0; i < accessor.Count; i++ {
		element := view.data[i*view.stride:]
		for j := 0; j < view.componentCount; j++ {
			component := element[j*view.componentSize:]
			values[i*view.componentCount+j] = decodeComponent(component, accessor.ComponentType, accessor.Normalized)
		}
	}
	return values, nil
}

// decodeComponent converts a component to a float. A normalized integer maps
// to [0, 1] when it is unsigned and to [-1, 1] when it is signed, as glTF
// defines.
func decodeComponent(data []byte, componentType int, normalized bool) float32 {
	switch componentType {
	case componentTypeByte:
		value := float64(int8(data[0]))
		if normalized {
			value = math.Max(value/127, -1)
		}
		return float32(value)
	case componentTypeUnsignedByte:
		value := float64(data[0])
		if normalized {
			value /= 255
		}
		return float32(value)
	case componentTypeShort:
		value := float64(int16(binary.LittleEndian.Uint16(data)))
		if normalized {
			value = math.Max(value/32767, -1)
		}
		return float32(value)
	case componentTypeUnsignedShort:
		value := float64(binary.LittleEndian.Uint16(data))
		if normalized {
			value /= 65535
		}
		return float32(value)
	case componentTypeUnsignedInt:
		return float32(binary.LittleEndian.Uint32(data))
	}
	return math.Float32frombits(binary.LittleEndian.Uint32(data))
}

// readIndices returns the elements of an accessor of unsigned integers.
func (view *accessorView) readIndices() ([]uint32, error) {
	accessor := view.accessor
	if view.componentCount != 1 {
		return nil, fmt.Errorf("type %q is not a scalar", accessor.Type)
	}
	switch accessor.ComponentType {
	case componentTypeUnsignedByte, componentTypeUnsignedShort, componentTypeUnsignedInt:
	default:
		return nil, fmt.Errorf("component type %d is not an unsigned integer", accessor.ComponentType)
	}

	indices := make([]uint32, accessor.Count)
	if view.data == nil {
		return indices, nil
	}

	for i := range indices {
		element := view.data[i*view.stride:]
		switch accessor.ComponentType {
		case componentTypeUnsignedByte:
			indices[i] = uint32(element[0])
		case componentTypeUnsignedShort:
			indices[i] = uint32(binary.LittleEndian.Uint16(element))
		default:
			indices[i] = binary.LittleEndian.Uint32(element)
		}
	}
	return indices, nil
}

// toIndices converts indices of a JSON array, and fails when one is out of
// the count items that they refer to.
func toIndices(values []int, count int, what string) ([]uint32, error) {
	indices := make([]uint32, 0, len(values))
	for _, value := range values {
		if value < 0 || value >= count {
			return nil, fmt.Errorf("%s #%d does not exist", what, value)
		}
		indices = append(indices, uint32(value))
	}
	if len(indices) == 0 {
		return nil, nil
	}
	return indices, nil
}

func toVector3(values []float64) *schemas.Mesh3DSourceVector3T {
	return &schemas.Mesh3DSourceVector3T{X: float32(values[0]), Y: float32(values[1]), Z: float32(values[2])}
}

func toVector4(values []float64) *schemas.Mesh3DSourceVector4T {
	return &schemas.Mesh3DSourceVector4T{X: float32(values[0]), Y: float32(values[1]), Z: float32(values[2]), W: float32(values[3])}
}
