// Copyright mogemimi. Distributed under the MIT license.

// Package zstdframe reads the headers of Zstandard frames and dictionaries,
// as specified in RFC 8878.
package zstdframe

import (
	"encoding/binary"
	"errors"
	"fmt"
)

const (
	// FrameMagic starts every Zstandard frame.
	FrameMagic = 0xFD2FB528

	// DictionaryMagic starts every dictionary in the Zstandard dictionary format.
	DictionaryMagic = 0xEC30A437

	// MaxHeaderSize is the size of the largest frame header.
	MaxHeaderSize = 18
)

// Header is the header of a Zstandard frame.
type Header struct {
	// HasContentSize reports whether the frame records ContentSize, the size of
	// the decoded content.
	HasContentSize bool
	ContentSize    uint64

	// SingleSegment reports whether the decoder needs the whole content as its
	// history. The window size is then the content size.
	SingleSegment bool

	// WindowSize is the history that a streaming decoder keeps for the frame.
	WindowSize uint64

	// DictionaryID is the ID of the dictionary that the frame needs, or 0 when
	// the frame does not record one.
	DictionaryID uint32

	// HasChecksum reports whether the frame ends with a content checksum.
	HasChecksum bool
}

// ParseHeader parses the header of the Zstandard frame at the start of buf.
func ParseHeader(buf []byte) (*Header, error) {
	if len(buf) < 5 {
		return nil, errors.New("frame header is truncated")
	}
	magic := binary.LittleEndian.Uint32(buf)
	if magic&0xFFFFFFF0 == 0x184D2A50 {
		return nil, errors.New("skippable frame")
	}
	if magic != FrameMagic {
		return nil, fmt.Errorf("unknown frame magic 0x%08x", magic)
	}

	descriptor := buf[4]
	if descriptor&0x08 != 0 {
		return nil, errors.New("reserved bit is set in the frame header descriptor")
	}

	header := &Header{
		SingleSegment: descriptor&0x20 != 0,
		HasChecksum:   descriptor&0x04 != 0,
	}

	contentSizeBytes := [4]int{0, 2, 4, 8}[descriptor>>6]
	if contentSizeBytes == 0 && header.SingleSegment {
		contentSizeBytes = 1
	}
	dictionaryIDBytes := [4]int{0, 1, 2, 4}[descriptor&0x03]

	windowDescriptorBytes := 1
	if header.SingleSegment {
		windowDescriptorBytes = 0
	}

	size := 5 + windowDescriptorBytes + dictionaryIDBytes + contentSizeBytes
	if len(buf) < size {
		return nil, errors.New("frame header is truncated")
	}
	pos := 5

	if !header.SingleSegment {
		exponent := uint64(buf[pos] >> 3)
		mantissa := uint64(buf[pos] & 0x07)
		windowBase := uint64(1) << (10 + exponent)
		header.WindowSize = windowBase + windowBase/8*mantissa
		pos++
	}

	switch dictionaryIDBytes {
	case 1:
		header.DictionaryID = uint32(buf[pos])
	case 2:
		header.DictionaryID = uint32(binary.LittleEndian.Uint16(buf[pos:]))
	case 4:
		header.DictionaryID = binary.LittleEndian.Uint32(buf[pos:])
	}
	pos += dictionaryIDBytes

	header.HasContentSize = contentSizeBytes > 0
	switch contentSizeBytes {
	case 1:
		header.ContentSize = uint64(buf[pos])
	case 2:
		// NOTE: The 2-byte field stores the content size minus 256.
		header.ContentSize = uint64(binary.LittleEndian.Uint16(buf[pos:])) + 256
	case 4:
		header.ContentSize = uint64(binary.LittleEndian.Uint32(buf[pos:]))
	case 8:
		header.ContentSize = binary.LittleEndian.Uint64(buf[pos:])
	}

	if header.SingleSegment {
		header.WindowSize = header.ContentSize
	}
	return header, nil
}

// DictionaryID returns the dictionary ID that a dictionary in the Zstandard
// dictionary format records.
func DictionaryID(dictionary []byte) (uint32, error) {
	if len(dictionary) < 8 {
		return 0, errors.New("dictionary header is truncated")
	}
	if magic := binary.LittleEndian.Uint32(dictionary); magic != DictionaryMagic {
		return 0, fmt.Errorf("unknown dictionary magic 0x%08x", magic)
	}
	return binary.LittleEndian.Uint32(dictionary[4:]), nil
}
