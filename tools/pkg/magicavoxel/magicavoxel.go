// Package magicavoxel reads and writes the `.vox` files of MagicaVoxel.
//
// The package keeps the structure of the file: models, scene graph nodes with
// their IDs, layers, the palette, and materials, with the dictionaries of the
// file as ordered lists of text entries. It does not interpret the entries;
// the helpers in dict.go and rotation.go parse the common ones.
//
// It reads version 150, which the format documentation describes and which
// MagicaVoxel wrote for years, and version 200, which recent versions of
// MagicaVoxel write. A file of the older releases has only models and a
// palette, and a file of the recent releases adds the scene graph, layers,
// and materials.
package magicavoxel

// Version150 is the version that the format documentation describes.
const Version150 = 150

// Version200 is the version of the files that recent MagicaVoxel releases write.
const Version200 = 200

// Color is a color with 8-bit red, green, blue, and alpha.
type Color struct {
	R uint8
	G uint8
	B uint8
	A uint8
}

// Voxel is a voxel of a model: its position and its color index.
type Voxel struct {
	X          uint8
	Y          uint8
	Z          uint8
	ColorIndex uint8
}

// Model is a grid of voxels, which a SIZE chunk and an XYZI chunk describe.
// The z axis points up.
type Model struct {
	SizeX  int32
	SizeY  int32
	SizeZ  int32
	Voxels []Voxel
}

// DictEntry is an entry of a dictionary.
type DictEntry struct {
	Key   string
	Value string
}

// Dict is a dictionary of the file, which keeps its entries in order.
type Dict []DictEntry

// Get returns the value of the first entry with the given key.
func (d Dict) Get(key string) (string, bool) {
	for _, e := range d {
		if e.Key == key {
			return e.Value, true
		}
	}
	return "", false
}

// NodeKind tells which chunk a node of the scene graph comes from.
type NodeKind int

const (
	// TransformNode is a transform node (nTRN chunk).
	TransformNode NodeKind = iota

	// GroupNode is a group node (nGRP chunk).
	GroupNode

	// ShapeNode is a shape node (nSHP chunk).
	ShapeNode
)

// ShapeModel is a model that a shape node shows.
type ShapeModel struct {
	ModelID    int32
	Attributes Dict
}

// Node is a node of the scene graph. The fields after Attributes belong to
// one kind of node and are zero for the others.
type Node struct {
	Kind       NodeKind
	ID         int32
	Attributes Dict

	// ChildID is the ID of the only child of a transform node.
	ChildID int32

	// ReservedID is the reserved ID of a transform node, which is -1.
	ReservedID int32

	// LayerID is the ID of the layer of a transform node, or -1.
	LayerID int32

	// Frames lists the frame dictionaries of a transform node.
	Frames []Dict

	// ChildIDs lists the IDs of the children of a group node.
	ChildIDs []int32

	// Models lists the models of a shape node.
	Models []ShapeModel
}

// Layer is a layer of the scene (LAYR chunk).
type Layer struct {
	ID         int32
	Attributes Dict

	// ReservedID is -1.
	ReservedID int32
}

// Material is a material (MATL chunk). ID is the palette index that the
// material belongs to.
type Material struct {
	ID         int32
	Properties Dict
}

// Chunk is a chunk that the package does not decode, such as MATT, rOBJ,
// rCAM, NOTE, and IMAP.
type Chunk struct {
	ID      string
	Content []byte
}

// File is the content of a `.vox` file.
type File struct {
	Version int32

	// Models lists the models in the order of the file; their indices are
	// the model IDs that shape nodes refer to.
	Models []Model

	// Nodes lists the nodes of the scene graph in the order of the file. The
	// node whose ID is 0 is the root.
	Nodes []Node

	Layers []Layer

	// Palette holds the colors by color index: Palette[i] is the color of the
	// voxels whose color index is i. Palette[0] holds the last color of the
	// RGBA chunk, which no voxel uses. It is nil when the file has no RGBA
	// chunk, which means DefaultPalette.
	Palette *[256]Color

	Materials []Material

	// OtherChunks lists the chunks that the package does not decode, in the
	// order of the file.
	OtherChunks []Chunk
}
