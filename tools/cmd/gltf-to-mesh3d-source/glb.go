// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"encoding/binary"
	"encoding/json"
	"errors"
	"fmt"
)

const (
	glbMagic     = 0x46546C67 // "glTF"
	glbChunkJSON = 0x4E4F534A // "JSON"
	glbChunkBIN  = 0x004E4942 // "BIN\0"

	glbHeaderSize      = 12
	glbChunkHeaderSize = 8
)

// parseGLB splits a glTF binary file into its JSON chunk and its binary
// chunk, which is nil when the file has none.
func parseGLB(data []byte) (jsonChunk []byte, binChunk []byte, err error) {
	if len(data) < glbHeaderSize {
		return nil, nil, errors.New("the file is too small to be a glTF binary file")
	}
	if binary.LittleEndian.Uint32(data[0:4]) != glbMagic {
		return nil, nil, errors.New("not a glTF binary file (.glb)")
	}
	if version := binary.LittleEndian.Uint32(data[4:8]); version != 2 {
		return nil, nil, fmt.Errorf("version %d of the glTF binary format is not supported", version)
	}
	length := binary.LittleEndian.Uint32(data[8:12])
	if length < glbHeaderSize || uint64(length) > uint64(len(data)) {
		return nil, nil, fmt.Errorf("the header declares %d bytes, but the file has %d", length, len(data))
	}
	data = data[glbHeaderSize:length]

	for index := 0; len(data) > 0; index++ {
		if len(data) < glbChunkHeaderSize {
			return nil, nil, fmt.Errorf("chunk #%d has no header", index)
		}
		chunkLength := binary.LittleEndian.Uint32(data[0:4])
		chunkType := binary.LittleEndian.Uint32(data[4:8])
		data = data[glbChunkHeaderSize:]
		if uint64(chunkLength) > uint64(len(data)) {
			return nil, nil, fmt.Errorf("chunk #%d is longer than the rest of the file", index)
		}
		chunk := data[:chunkLength]
		data = data[chunkLength:]

		switch {
		case index == 0:
			if chunkType != glbChunkJSON {
				return nil, nil, errors.New("the first chunk must be the JSON chunk")
			}
			jsonChunk = chunk
		case index == 1 && chunkType == glbChunkBIN:
			binChunk = chunk
		default:
			// NOTE: The format lets a reader skip the chunks that it does not know.
		}
	}

	if jsonChunk == nil {
		return nil, nil, errors.New("the file has no JSON chunk")
	}
	return jsonChunk, binChunk, nil
}

// gltfDocument holds the parts of a glTF 2.0 document that the converter reads.
type gltfDocument struct {
	Asset struct {
		Version   string `json:"version"`
		Generator string `json:"generator"`
	} `json:"asset"`
	Scene              *int              `json:"scene"`
	Scenes             []*gltfScene      `json:"scenes"`
	Nodes              []*gltfNode       `json:"nodes"`
	Materials          []*gltfMaterial   `json:"materials"`
	Meshes             []*gltfMesh       `json:"meshes"`
	Accessors          []*gltfAccessor   `json:"accessors"`
	BufferViews        []*gltfBufferView `json:"bufferViews"`
	Buffers            []*gltfBuffer     `json:"buffers"`
	ExtensionsRequired []string          `json:"extensionsRequired"`

	// NOTE: The converter only reports that it dropped the data below.
	Animations []json.RawMessage `json:"animations"`
	Cameras    []json.RawMessage `json:"cameras"`
	Images     []json.RawMessage `json:"images"`
	Samplers   []json.RawMessage `json:"samplers"`
	Skins      []json.RawMessage `json:"skins"`
	Textures   []json.RawMessage `json:"textures"`
}

type gltfScene struct {
	Name  string `json:"name"`
	Nodes []int  `json:"nodes"`
}

type gltfNode struct {
	Name        string    `json:"name"`
	Children    []int     `json:"children"`
	Mesh        *int      `json:"mesh"`
	Matrix      []float64 `json:"matrix"`
	Translation []float64 `json:"translation"`
	Rotation    []float64 `json:"rotation"`
	Scale       []float64 `json:"scale"`
}

type gltfMaterial struct {
	Name string `json:"name"`
	PBR  *struct {
		BaseColorFactor []float64 `json:"baseColorFactor"`
		MetallicFactor  *float64  `json:"metallicFactor"`
		RoughnessFactor *float64  `json:"roughnessFactor"`
	} `json:"pbrMetallicRoughness"`
	EmissiveFactor []float64 `json:"emissiveFactor"`
	DoubleSided    bool      `json:"doubleSided"`
}

type gltfMesh struct {
	Name       string           `json:"name"`
	Primitives []*gltfPrimitive `json:"primitives"`
}

type gltfPrimitive struct {
	Attributes map[string]int    `json:"attributes"`
	Indices    *int              `json:"indices"`
	Material   *int              `json:"material"`
	Mode       *int              `json:"mode"`
	Targets    []json.RawMessage `json:"targets"`
}

type gltfAccessor struct {
	BufferView    *int            `json:"bufferView"`
	ByteOffset    int             `json:"byteOffset"`
	ComponentType int             `json:"componentType"`
	Normalized    bool            `json:"normalized"`
	Count         int             `json:"count"`
	Type          string          `json:"type"`
	Sparse        json.RawMessage `json:"sparse"`
}

type gltfBufferView struct {
	Buffer     int `json:"buffer"`
	ByteOffset int `json:"byteOffset"`
	ByteLength int `json:"byteLength"`
	ByteStride int `json:"byteStride"`
}

type gltfBuffer struct {
	URI        *string `json:"uri"`
	ByteLength int     `json:"byteLength"`
}

func parseGLTFDocument(jsonChunk []byte) (*gltfDocument, error) {
	doc := &gltfDocument{}
	if err := json.Unmarshal(jsonChunk, doc); err != nil {
		return nil, fmt.Errorf("json.Unmarshal() failed: %w", err)
	}
	return doc, nil
}
