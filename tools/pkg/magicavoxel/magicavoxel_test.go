package magicavoxel

import (
	"encoding/binary"
	"reflect"
	"strings"
	"testing"
)

func sampleFile() *File {
	palette := DefaultPalette()
	palette[0] = Color{R: 1, G: 2, B: 3, A: 4}
	return &File{
		Version: Version200,
		Models: []Model{
			{SizeX: 2, SizeY: 3, SizeZ: 4, Voxels: []Voxel{{X: 0, Y: 1, Z: 2, ColorIndex: 5}, {X: 1, Y: 2, Z: 3, ColorIndex: 255}}},
			{SizeX: 1, SizeY: 1, SizeZ: 1, Voxels: []Voxel{{ColorIndex: 1}}},
		},
		Nodes: []Node{
			{Kind: TransformNode, ID: 0, ChildID: 1, ReservedID: -1, LayerID: -1, Frames: []Dict{nil}},
			{Kind: GroupNode, ID: 1, ChildIDs: []int32{2, 4}},
			{Kind: TransformNode, ID: 2, Attributes: Dict{{Key: KeyName, Value: "body"}}, ChildID: 3, ReservedID: -1, LayerID: 0, Frames: []Dict{
				{{Key: KeyRotation, Value: "17"}, {Key: KeyTranslation, Value: "1 -2 3"}},
			}},
			{Kind: ShapeNode, ID: 3, Models: []ShapeModel{{ModelID: 0}, {ModelID: 1, Attributes: Dict{{Key: KeyFrame, Value: "3"}}}}},
			{Kind: TransformNode, ID: 4, Attributes: Dict{{Key: KeyHidden, Value: "1"}}, ChildID: 5, ReservedID: -1, LayerID: 1, Frames: []Dict{nil}},
			{Kind: ShapeNode, ID: 5, Models: []ShapeModel{{ModelID: 1}}},
		},
		Layers: []Layer{
			{ID: 0, Attributes: Dict{{Key: KeyName, Value: "first"}}, ReservedID: -1},
			{ID: 1, Attributes: Dict{{Key: "_color", Value: "255 0 0"}}, ReservedID: -1},
		},
		Palette: &palette,
		Materials: []Material{
			{ID: 1, Properties: Dict{{Key: "_type", Value: "_diffuse"}, {Key: "_weight", Value: "1"}}},
		},
		OtherChunks: []Chunk{{ID: "rOBJ", Content: []byte{0, 0, 0, 0}}},
	}
}

func TestEncodeDecode(t *testing.T) {
	f := sampleFile()
	data, err := Encode(f)
	if err != nil {
		t.Fatal(err)
	}
	g, err := Decode(data)
	if err != nil {
		t.Fatal(err)
	}
	if !reflect.DeepEqual(f, g) {
		t.Fatalf("decoded file differs:\n got %+v\nwant %+v", g, f)
	}
	again, err := Encode(g)
	if err != nil {
		t.Fatal(err)
	}
	if string(again) != string(data) {
		t.Fatal("encoding the decoded file changed the bytes")
	}
}

func TestPaletteRotation(t *testing.T) {
	f := &File{Version: Version150, Models: []Model{{SizeX: 1, SizeY: 1, SizeZ: 1, Voxels: []Voxel{{ColorIndex: 1}}}}}
	var palette [256]Color
	for i := range palette {
		palette[i] = Color{R: uint8(i), A: 255}
	}
	f.Palette = &palette
	data, err := Encode(f)
	if err != nil {
		t.Fatal(err)
	}
	// NOTE: The RGBA chunk is the last chunk; its first color belongs to color
	// index 1 and its last color to color index 0.
	rgba := data[len(data)-1024:]
	if rgba[0] != 1 || rgba[4*254] != 255 || rgba[4*255] != 0 {
		t.Fatalf("unexpected palette order: %v %v %v", rgba[0], rgba[4*254], rgba[4*255])
	}
}

func TestPackChunk(t *testing.T) {
	f := &File{Version: Version150, Models: []Model{
		{SizeX: 1, SizeY: 1, SizeZ: 1, Voxels: []Voxel{{ColorIndex: 1}}},
		{SizeX: 1, SizeY: 1, SizeZ: 1, Voxels: []Voxel{{ColorIndex: 2}}},
	}}
	data, err := Encode(f)
	if err != nil {
		t.Fatal(err)
	}
	if string(data[20:24]) != "PACK" {
		t.Fatalf("the first chunk is %q, not PACK", data[20:24])
	}
	if _, err := Decode(data); err != nil {
		t.Fatal(err)
	}

	f.Nodes = sampleFile().Nodes
	data, err = Encode(f)
	if err != nil {
		t.Fatal(err)
	}
	if string(data[20:24]) != "SIZE" {
		t.Fatalf("a file with a scene graph has %q before its models", data[20:24])
	}
}

type chunkSpec struct {
	id       string
	content  []byte
	children []byte
}

func makeChunk(c chunkSpec) []byte {
	var b []byte
	b = append(b, c.id...)
	b = binary.LittleEndian.AppendUint32(b, uint32(len(c.content)))
	b = binary.LittleEndian.AppendUint32(b, uint32(len(c.children)))
	b = append(b, c.content...)
	return append(b, c.children...)
}

func makeFile(version uint32, chunks ...chunkSpec) []byte {
	var children []byte
	for _, c := range chunks {
		children = append(children, makeChunk(c)...)
	}
	b := []byte("VOX ")
	b = binary.LittleEndian.AppendUint32(b, version)
	return append(b, makeChunk(chunkSpec{id: "MAIN", children: children})...)
}

func ints(values ...int32) []byte {
	var b []byte
	for _, v := range values {
		b = binary.LittleEndian.AppendUint32(b, uint32(v))
	}
	return b
}

func TestDecodeErrors(t *testing.T) {
	size := chunkSpec{id: "SIZE", content: ints(1, 1, 1)}
	xyzi := chunkSpec{id: "XYZI", content: append(ints(1), 0, 0, 0, 1)}

	tests := []struct {
		name string
		data []byte
		want string
	}{
		{"signature", []byte("VOXX\x96\x00\x00\x00"), "signature"},
		{"version", makeFile(151, size, xyzi), "unsupported version 151"},
		{"main", append([]byte("VOX \x96\x00\x00\x00"), makeChunk(chunkSpec{id: "SIZE", content: ints(1, 1, 1)})...), "not MAIN"},
		{"main content", append([]byte("VOX \x96\x00\x00\x00"), makeChunk(chunkSpec{id: "MAIN", content: ints(1)})...), "MAIN chunk has 4 bytes"},
		{"trailing", append(makeFile(150, size, xyzi), 0), "after the MAIN chunk"},
		{"truncated", makeFile(150, size, xyzi)[:30], "overruns"},
		{"child chunks", makeFile(150, chunkSpec{id: "SIZE", content: ints(1, 1, 1), children: makeChunk(xyzi)}), "child chunks"},
		{"size without xyzi", makeFile(150, size), "without an XYZI"},
		{"size then rgba", makeFile(150, size, chunkSpec{id: "RGBA", content: make([]byte, 1024)}), "not XYZI"},
		{"xyzi without size", makeFile(150, xyzi), "without a SIZE"},
		{"negative size", makeFile(150, chunkSpec{id: "SIZE", content: ints(1, -1, 1)}, xyzi), "negative size"},
		{"voxel bytes", makeFile(150, size, chunkSpec{id: "XYZI", content: append(ints(2), 0, 0, 0, 1)}), "2 voxels need 8 bytes"},
		{"rgba size", makeFile(150, size, xyzi, chunkSpec{id: "RGBA", content: make([]byte, 1020)}), "not 1024"},
		{"two rgba", makeFile(150, size, xyzi, chunkSpec{id: "RGBA", content: make([]byte, 1024)}, chunkSpec{id: "RGBA", content: make([]byte, 1024)}), "more than one RGBA"},
		{"pack count", makeFile(150, chunkSpec{id: "PACK", content: ints(2)}, size, xyzi), "PACK chunk says 2 models"},
		{"node dict", makeFile(150, size, xyzi, chunkSpec{id: "nGRP", content: ints(0, 1, 100)}), "overruns the chunk"},
		{"node rest", makeFile(150, size, xyzi, chunkSpec{id: "nGRP", content: ints(0, 0, 0, 7)}), "unread bytes"},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			_, err := Decode(tt.data)
			if err == nil || !strings.Contains(err.Error(), tt.want) {
				t.Fatalf("got error %v, want one containing %q", err, tt.want)
			}
		})
	}
}

func TestRotation(t *testing.T) {
	m, err := IdentityRotation.Matrix()
	if err != nil {
		t.Fatal(err)
	}
	if m != [3][3]int32{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}} {
		t.Fatalf("identity rotation gives %v", m)
	}

	// NOTE: The example of the MagicaVoxel file format extension.
	example := Rotation((1 << 0) | (2 << 2) | (0 << 4) | (1 << 5) | (1 << 6))
	m, err = example.Matrix()
	if err != nil {
		t.Fatal(err)
	}
	if m != [3][3]int32{{0, 1, 0}, {0, 0, -1}, {-1, 0, 0}} {
		t.Fatalf("example rotation gives %v", m)
	}

	valid := 0
	for v := range 256 {
		r := Rotation(v)
		m, err := r.Matrix()
		if !r.IsValid() {
			if err == nil {
				t.Fatalf("invalid rotation %d gives a matrix", v)
			}
			continue
		}
		valid++
		back, err := RotationFromMatrix(m)
		if err != nil || back != r {
			t.Fatalf("rotation %d round-trips to %d, %v", v, back, err)
		}
	}
	if valid != 48 {
		t.Fatalf("%d valid rotations, want 48", valid)
	}

	if _, err := RotationFromMatrix([3][3]int32{{1, 0, 0}, {1, 0, 0}, {0, 0, 1}}); err == nil {
		t.Fatal("a matrix with two rows in one column is accepted")
	}
}

func TestDictValues(t *testing.T) {
	tr, err := ParseTranslation("-1 2 300")
	if err != nil || tr != [3]int32{-1, 2, 300} {
		t.Fatalf("ParseTranslation gives %v, %v", tr, err)
	}
	if FormatTranslation(tr) != "-1 2 300" {
		t.Fatal("FormatTranslation does not invert ParseTranslation")
	}
	for _, bad := range []string{"", "1 2", "1 2 3 4", "1 x 3"} {
		if _, err := ParseTranslation(bad); err == nil {
			t.Fatalf("ParseTranslation accepts %q", bad)
		}
	}
	if r, err := ParseRotation("4"); err != nil || r != IdentityRotation {
		t.Fatalf("ParseRotation gives %v, %v", r, err)
	}
	for _, bad := range []string{"3", "256", "-1", "a"} {
		if _, err := ParseRotation(bad); err == nil {
			t.Fatalf("ParseRotation accepts %q", bad)
		}
	}
	if b, err := ParseBool("1"); err != nil || !b {
		t.Fatal("ParseBool does not accept 1")
	}
	if _, err := ParseBool("true"); err == nil {
		t.Fatal("ParseBool accepts true")
	}
}

func TestDefaultPalette(t *testing.T) {
	p := DefaultPalette()
	if p[0] != (Color{}) {
		t.Fatalf("color 0 is %v", p[0])
	}
	if p[1] != (Color{R: 255, G: 255, B: 255, A: 255}) {
		t.Fatalf("color 1 is %v", p[1])
	}
	if p[216] != (Color{R: 0xee, A: 255}) {
		t.Fatalf("color 216 is %v", p[216])
	}
}
