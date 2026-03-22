// Copyright mogemimi. Distributed under the MIT license.

package spirvreflect

type Reflect struct {
	Types            map[string]*Type       `json:"types"`
	EntryPoints      []*EntryPoint          `json:"entryPoints"`
	Inputs           []*Input               `json:"inputs"`
	Outputs          []*Output              `json:"outputs"`
	UBOs             []*UniformBufferObject `json:"ubos"`
	Textures         []*Texture             `json:"textures"`
	SeparateImages   []*SeparateImage       `json:"separate_images"`
	SeparateSamplers []*SeparateSampler     `json:"separate_samplers"`
}

type Type struct {
	Name    string        `json:"name"`
	Members []*TypeMember `json:"members"`
}

type TypeMember struct {
	Name               string `json:"name"`
	Type               string `json:"type"`
	Offset             int    `json:"offset"`
	MatrixStride       int    `json:"matrix_stride,omitempty"`
	Array              []int  `json:"array,omitempty"`
	ArraySizeIsLiteral []bool `json:"array_size_is_literal,omitempty"`
}

type EntryPoint struct {
	Name string `json:"name"`
	Mode string `json:"mode"`
}

type Input struct {
	Type     string `json:"type"`
	Name     string `json:"name"`
	Location int    `json:"location"`
}

type Output struct {
	Type     string `json:"type"`
	Name     string `json:"name"`
	Location int    `json:"location"`
}

type UniformBufferObject struct {
	Type      string `json:"type"`
	Name      string `json:"name"`
	BlockSize int    `json:"block_size"`
	Set       int    `json:"set"`
	Binding   int    `json:"binding"`
}

type Texture struct {
	Type    string `json:"type"`
	Name    string `json:"name"`
	Set     int    `json:"set"`
	Binding int    `json:"binding"`
}

type SeparateImage struct {
	Type    string `json:"type"`
	Name    string `json:"name"`
	Set     int    `json:"set"`
	Binding int    `json:"binding"`
}

type SeparateSampler struct {
	Type    string `json:"type"`
	Name    string `json:"name"`
	Set     int    `json:"set"`
	Binding int    `json:"binding"`
}
