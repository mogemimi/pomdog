package magicavoxel

import (
	"encoding/binary"
	"fmt"
	"math"
)

// writer appends little-endian values to the content of a chunk.
type writer struct {
	buf []byte
	err error
}

func (w *writer) int32(v int32) {
	w.buf = binary.LittleEndian.AppendUint32(w.buf, uint32(v))
}

func (w *writer) length(n int, what string) {
	if n > math.MaxInt32 {
		if w.err == nil {
			w.err = fmt.Errorf("too many %s: %d", what, n)
		}
		return
	}
	w.int32(int32(n))
}

func (w *writer) string(s string) {
	w.length(len(s), "bytes in a string")
	w.buf = append(w.buf, s...)
}

func (w *writer) dict(d Dict) {
	w.length(len(d), "dictionary entries")
	for _, e := range d {
		w.string(e.Key)
		w.string(e.Value)
	}
}

func appendChunk(dst []byte, id string, content []byte) ([]byte, error) {
	if len(id) != 4 {
		return nil, fmt.Errorf("chunk ID %q is not 4 bytes", id)
	}
	if len(content) > math.MaxInt32 {
		return nil, fmt.Errorf("chunk %q is too large: %d bytes", id, len(content))
	}
	dst = append(dst, id...)
	dst = binary.LittleEndian.AppendUint32(dst, uint32(len(content)))
	dst = binary.LittleEndian.AppendUint32(dst, 0)
	return append(dst, content...), nil
}

// Encode writes the content of a `.vox` file.
//
// The MAIN chunk holds, in this order, a PACK chunk when the file has more
// than one model and no scene graph (which old releases read as the frames of
// an animation), the SIZE and XYZI chunks of the models, the nodes, the
// layers, the RGBA chunk when Palette is not nil, the materials, and
// OtherChunks.
func Encode(f *File) ([]byte, error) {
	if f.Version != Version150 && f.Version != Version200 {
		return nil, fmt.Errorf("unsupported version %d; supported versions are %d and %d", f.Version, Version150, Version200)
	}

	var children []byte
	var err error
	add := func(id string, w *writer) {
		if err != nil {
			return
		}
		if w.err != nil {
			err = fmt.Errorf("%s: %w", id, w.err)
			return
		}
		children, err = appendChunk(children, id, w.buf)
	}

	if len(f.Models) > 1 && len(f.Nodes) == 0 {
		w := &writer{}
		w.length(len(f.Models), "models")
		add("PACK", w)
	}
	for i, m := range f.Models {
		if m.SizeX < 0 || m.SizeY < 0 || m.SizeZ < 0 {
			return nil, fmt.Errorf("model %d has a negative size", i)
		}
		size := &writer{}
		size.int32(m.SizeX)
		size.int32(m.SizeY)
		size.int32(m.SizeZ)
		add("SIZE", size)

		xyzi := &writer{}
		xyzi.length(len(m.Voxels), "voxels")
		for _, v := range m.Voxels {
			xyzi.buf = append(xyzi.buf, v.X, v.Y, v.Z, v.ColorIndex)
		}
		add("XYZI", xyzi)
	}
	for _, n := range f.Nodes {
		w := &writer{}
		w.int32(n.ID)
		w.dict(n.Attributes)
		switch n.Kind {
		case TransformNode:
			w.int32(n.ChildID)
			w.int32(n.ReservedID)
			w.int32(n.LayerID)
			w.length(len(n.Frames), "frames")
			for _, d := range n.Frames {
				w.dict(d)
			}
			add("nTRN", w)
		case GroupNode:
			w.length(len(n.ChildIDs), "children")
			for _, id := range n.ChildIDs {
				w.int32(id)
			}
			add("nGRP", w)
		case ShapeNode:
			w.length(len(n.Models), "models")
			for _, m := range n.Models {
				w.int32(m.ModelID)
				w.dict(m.Attributes)
			}
			add("nSHP", w)
		default:
			return nil, fmt.Errorf("node %d has an unknown kind %d", n.ID, n.Kind)
		}
	}
	for _, l := range f.Layers {
		w := &writer{}
		w.int32(l.ID)
		w.dict(l.Attributes)
		w.int32(l.ReservedID)
		add("LAYR", w)
	}
	if f.Palette != nil {
		w := &writer{}
		for i := range 256 {
			c := f.Palette[(i+1)%256]
			w.buf = append(w.buf, c.R, c.G, c.B, c.A)
		}
		add("RGBA", w)
	}
	for _, m := range f.Materials {
		w := &writer{}
		w.int32(m.ID)
		w.dict(m.Properties)
		add("MATL", w)
	}
	for _, c := range f.OtherChunks {
		add(c.ID, &writer{buf: c.Content})
	}
	if err != nil {
		return nil, err
	}
	if len(children) > math.MaxInt32 {
		return nil, fmt.Errorf("the file is too large: %d bytes", len(children))
	}

	out := make([]byte, 0, 8+chunkHeaderSize+len(children))
	out = append(out, "VOX "...)
	out = binary.LittleEndian.AppendUint32(out, uint32(f.Version))
	out = append(out, "MAIN"...)
	out = binary.LittleEndian.AppendUint32(out, 0)
	out = binary.LittleEndian.AppendUint32(out, uint32(len(children)))
	return append(out, children...), nil
}
