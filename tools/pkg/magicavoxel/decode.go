package magicavoxel

import (
	"encoding/binary"
	"fmt"
	"math"
)

const chunkHeaderSize = 12

// reader reads little-endian values from the content of a chunk.
type reader struct {
	data []byte
	pos  int
}

func (r *reader) remaining() int {
	return len(r.data) - r.pos
}

func (r *reader) int32() (int32, error) {
	if r.remaining() < 4 {
		return 0, fmt.Errorf("unexpected end of chunk at byte %d", r.pos)
	}
	v := int32(binary.LittleEndian.Uint32(r.data[r.pos:]))
	r.pos += 4
	return v, nil
}

func (r *reader) count(what string) (int, error) {
	n, err := r.int32()
	if err != nil {
		return 0, err
	}
	if n < 0 {
		return 0, fmt.Errorf("negative number of %s: %d", what, n)
	}
	return int(n), nil
}

func (r *reader) string() (string, error) {
	n, err := r.count("bytes in a string")
	if err != nil {
		return "", err
	}
	if r.remaining() < n {
		return "", fmt.Errorf("string of %d bytes overruns the chunk at byte %d", n, r.pos)
	}
	s := string(r.data[r.pos : r.pos+n])
	r.pos += n
	return s, nil
}

func (r *reader) dict() (Dict, error) {
	n, err := r.count("dictionary entries")
	if err != nil {
		return nil, err
	}
	var d Dict
	for range n {
		key, err := r.string()
		if err != nil {
			return nil, err
		}
		value, err := r.string()
		if err != nil {
			return nil, err
		}
		d = append(d, DictEntry{Key: key, Value: value})
	}
	return d, nil
}

func (r *reader) end() error {
	if r.remaining() != 0 {
		return fmt.Errorf("%d unread bytes at the end of the chunk", r.remaining())
	}
	return nil
}

// Decode parses the content of a `.vox` file.
func Decode(data []byte) (*File, error) {
	if len(data) < 8 || string(data[:4]) != "VOX " {
		return nil, fmt.Errorf("not a MagicaVoxel file: missing the 'VOX ' signature")
	}
	version := int32(binary.LittleEndian.Uint32(data[4:]))
	if version != Version150 && version != Version200 {
		return nil, fmt.Errorf("unsupported version %d; supported versions are %d and %d", version, Version150, Version200)
	}

	mainID, mainContent, mainChildren, next, err := readChunk(data, 8)
	if err != nil {
		return nil, err
	}
	if mainID != "MAIN" {
		return nil, fmt.Errorf("the first chunk is %q, not MAIN", mainID)
	}
	if next != len(data) {
		return nil, fmt.Errorf("%d bytes after the MAIN chunk", len(data)-next)
	}
	if len(mainContent) != 0 {
		return nil, fmt.Errorf("MAIN chunk has %d bytes of content, but it must have none", len(mainContent))
	}

	f := &File{Version: version}
	packModelCount := -1
	var pendingSize *Model

	for pos := 0; pos < len(mainChildren); {
		id, content, children, nextPos, err := readChunk(mainChildren, pos)
		if err != nil {
			return nil, err
		}
		if len(children) != 0 {
			return nil, fmt.Errorf("chunk %q has child chunks, which only MAIN may have", id)
		}
		if pendingSize != nil && id != "XYZI" {
			return nil, fmt.Errorf("SIZE chunk of model %d is followed by %q, not XYZI", len(f.Models), id)
		}

		r := &reader{data: content}
		switch id {
		case "PACK":
			if packModelCount >= 0 {
				return nil, fmt.Errorf("more than one PACK chunk")
			}
			n, err := r.count("models")
			if err != nil {
				return nil, fmt.Errorf("PACK: %w", err)
			}
			if err := r.end(); err != nil {
				return nil, fmt.Errorf("PACK: %w", err)
			}
			packModelCount = n
		case "SIZE":
			m := Model{}
			for _, p := range []*int32{&m.SizeX, &m.SizeY, &m.SizeZ} {
				if *p, err = r.int32(); err != nil {
					return nil, fmt.Errorf("SIZE: %w", err)
				}
				if *p < 0 {
					return nil, fmt.Errorf("SIZE: negative size %d", *p)
				}
			}
			if err := r.end(); err != nil {
				return nil, fmt.Errorf("SIZE: %w", err)
			}
			pendingSize = &m
		case "XYZI":
			if pendingSize == nil {
				return nil, fmt.Errorf("XYZI chunk of model %d without a SIZE chunk before it", len(f.Models))
			}
			n, err := r.count("voxels")
			if err != nil {
				return nil, fmt.Errorf("XYZI: %w", err)
			}
			if r.remaining() != 4*n {
				return nil, fmt.Errorf("XYZI: %d voxels need %d bytes, but the chunk has %d", n, 4*n, r.remaining())
			}
			m := *pendingSize
			m.Voxels = make([]Voxel, n)
			for i := range m.Voxels {
				b := content[4+4*i:]
				m.Voxels[i] = Voxel{X: b[0], Y: b[1], Z: b[2], ColorIndex: b[3]}
			}
			f.Models = append(f.Models, m)
			pendingSize = nil
		case "RGBA":
			if f.Palette != nil {
				return nil, fmt.Errorf("more than one RGBA chunk")
			}
			if len(content) != 256*4 {
				return nil, fmt.Errorf("RGBA: the chunk has %d bytes, not 1024", len(content))
			}
			var palette [256]Color
			for i := range 256 {
				b := content[4*i:]
				palette[(i+1)%256] = Color{R: b[0], G: b[1], B: b[2], A: b[3]}
			}
			f.Palette = &palette
		case "nTRN":
			n, err := decodeTransformNode(r)
			if err != nil {
				return nil, fmt.Errorf("nTRN: %w", err)
			}
			f.Nodes = append(f.Nodes, n)
		case "nGRP":
			n, err := decodeGroupNode(r)
			if err != nil {
				return nil, fmt.Errorf("nGRP: %w", err)
			}
			f.Nodes = append(f.Nodes, n)
		case "nSHP":
			n, err := decodeShapeNode(r)
			if err != nil {
				return nil, fmt.Errorf("nSHP: %w", err)
			}
			f.Nodes = append(f.Nodes, n)
		case "LAYR":
			l, err := decodeLayer(r)
			if err != nil {
				return nil, fmt.Errorf("LAYR: %w", err)
			}
			f.Layers = append(f.Layers, l)
		case "MATL":
			m, err := decodeMaterial(r)
			if err != nil {
				return nil, fmt.Errorf("MATL: %w", err)
			}
			f.Materials = append(f.Materials, m)
		default:
			f.OtherChunks = append(f.OtherChunks, Chunk{ID: id, Content: append([]byte(nil), content...)})
		}
		pos = nextPos
	}

	if pendingSize != nil {
		return nil, fmt.Errorf("SIZE chunk of model %d without an XYZI chunk after it", len(f.Models))
	}
	if packModelCount >= 0 && packModelCount != len(f.Models) {
		return nil, fmt.Errorf("PACK chunk says %d models, but the file has %d", packModelCount, len(f.Models))
	}
	return f, nil
}

// readChunk reads the chunk at data[pos:] and returns its ID, its content,
// its children, and the position after it.
func readChunk(data []byte, pos int) (id string, content []byte, children []byte, next int, err error) {
	if len(data)-pos < chunkHeaderSize {
		return "", nil, nil, 0, fmt.Errorf("truncated chunk header at byte %d", pos)
	}
	id = string(data[pos : pos+4])
	contentSize := int64(int32(binary.LittleEndian.Uint32(data[pos+4:])))
	childrenSize := int64(int32(binary.LittleEndian.Uint32(data[pos+8:])))
	if contentSize < 0 || childrenSize < 0 {
		return "", nil, nil, 0, fmt.Errorf("chunk %q has a negative size", id)
	}
	start := int64(pos + chunkHeaderSize)
	end := start + contentSize + childrenSize
	if end > int64(len(data)) || end > math.MaxInt32 {
		return "", nil, nil, 0, fmt.Errorf("chunk %q of %d bytes overruns its parent", id, contentSize+childrenSize)
	}
	content = data[start : start+contentSize]
	children = data[start+contentSize : end]
	return id, content, children, int(end), nil
}

func decodeTransformNode(r *reader) (Node, error) {
	n := Node{Kind: TransformNode}
	var err error
	if n.ID, err = r.int32(); err != nil {
		return n, err
	}
	if n.Attributes, err = r.dict(); err != nil {
		return n, err
	}
	if n.ChildID, err = r.int32(); err != nil {
		return n, err
	}
	if n.ReservedID, err = r.int32(); err != nil {
		return n, err
	}
	if n.LayerID, err = r.int32(); err != nil {
		return n, err
	}
	frameCount, err := r.count("frames")
	if err != nil {
		return n, err
	}
	for range frameCount {
		d, err := r.dict()
		if err != nil {
			return n, err
		}
		n.Frames = append(n.Frames, d)
	}
	return n, r.end()
}

func decodeGroupNode(r *reader) (Node, error) {
	n := Node{Kind: GroupNode}
	var err error
	if n.ID, err = r.int32(); err != nil {
		return n, err
	}
	if n.Attributes, err = r.dict(); err != nil {
		return n, err
	}
	childCount, err := r.count("children")
	if err != nil {
		return n, err
	}
	for range childCount {
		id, err := r.int32()
		if err != nil {
			return n, err
		}
		n.ChildIDs = append(n.ChildIDs, id)
	}
	return n, r.end()
}

func decodeShapeNode(r *reader) (Node, error) {
	n := Node{Kind: ShapeNode}
	var err error
	if n.ID, err = r.int32(); err != nil {
		return n, err
	}
	if n.Attributes, err = r.dict(); err != nil {
		return n, err
	}
	modelCount, err := r.count("models")
	if err != nil {
		return n, err
	}
	for range modelCount {
		var m ShapeModel
		if m.ModelID, err = r.int32(); err != nil {
			return n, err
		}
		if m.Attributes, err = r.dict(); err != nil {
			return n, err
		}
		n.Models = append(n.Models, m)
	}
	return n, r.end()
}

func decodeLayer(r *reader) (Layer, error) {
	var l Layer
	var err error
	if l.ID, err = r.int32(); err != nil {
		return l, err
	}
	if l.Attributes, err = r.dict(); err != nil {
		return l, err
	}
	if l.ReservedID, err = r.int32(); err != nil {
		return l, err
	}
	return l, r.end()
}

func decodeMaterial(r *reader) (Material, error) {
	var m Material
	var err error
	if m.ID, err = r.int32(); err != nil {
		return m, err
	}
	if m.Properties, err = r.dict(); err != nil {
		return m, err
	}
	return m, r.end()
}
