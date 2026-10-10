// Copyright mogemimi. Distributed under the MIT license.

package zstdframe

import (
	"strings"
	"testing"
)

func TestParseHeader(t *testing.T) {
	// NOTE: The headers come from frames that the zstd command-line tool writes
	// with level 19.
	for _, tc := range []struct {
		name   string
		header []byte
		want   Header
	}{
		{"1-byte content size", []byte{0x28, 0xb5, 0x2f, 0xfd, 0x24, 0x18}, Header{
			HasContentSize: true, ContentSize: 24, SingleSegment: true, WindowSize: 24, HasChecksum: true,
		}},
		{"2-byte content size", []byte{0x28, 0xb5, 0x2f, 0xfd, 0x64, 0x2c, 0x00}, Header{
			HasContentSize: true, ContentSize: 300, SingleSegment: true, WindowSize: 300, HasChecksum: true,
		}},
		{"4-byte content size", []byte{0x28, 0xb5, 0x2f, 0xfd, 0xa4, 0x70, 0x11, 0x01, 0x00}, Header{
			HasContentSize: true, ContentSize: 70000, SingleSegment: true, WindowSize: 70000, HasChecksum: true,
		}},
		{"window descriptor", []byte{0x28, 0xb5, 0x2f, 0xfd, 0x84, 0x68, 0x40, 0x54, 0x89, 0x00}, Header{
			HasContentSize: true, ContentSize: 9000000, WindowSize: 8 << 20, HasChecksum: true,
		}},
		{"window mantissa", []byte{0x28, 0xb5, 0x2f, 0xfd, 0x00, 0x53}, Header{
			WindowSize: (1 << 20) + 3*(1<<17),
		}},
		{"1-byte dictionary ID", []byte{0x28, 0xb5, 0x2f, 0xfd, 0x25, 0x07, 0x18}, Header{
			HasContentSize: true, ContentSize: 24, SingleSegment: true, WindowSize: 24, DictionaryID: 7, HasChecksum: true,
		}},
		{"4-byte dictionary ID", []byte{0x28, 0xb5, 0x2f, 0xfd, 0x27, 0x78, 0x56, 0x34, 0x12, 0x18}, Header{
			HasContentSize: true, ContentSize: 24, SingleSegment: true, WindowSize: 24, DictionaryID: 0x12345678, HasChecksum: true,
		}},
	} {
		t.Run(tc.name, func(t *testing.T) {
			header, err := ParseHeader(tc.header)
			if err != nil {
				t.Fatalf("ParseHeader() failed: %v", err)
			}
			if *header != tc.want {
				t.Errorf("ParseHeader() = %+v, want %+v", *header, tc.want)
			}
			if _, err := ParseHeader(tc.header[:len(tc.header)-1]); err == nil {
				t.Error("ParseHeader() succeeded for a truncated header")
			}
		})
	}
}

func TestParseHeaderInvalid(t *testing.T) {
	for _, tc := range []struct {
		name   string
		header []byte
		want   string
	}{
		{"empty", nil, "truncated"},
		{"skippable frame", []byte{0x50, 0x2a, 0x4d, 0x18, 0x00, 0x00, 0x00, 0x00}, "skippable frame"},
		{"dictionary", []byte{0x37, 0xa4, 0x30, 0xec, 0x01, 0x00, 0x00, 0x00}, "unknown frame magic 0xec30a437"},
		{"reserved bit", []byte{0x28, 0xb5, 0x2f, 0xfd, 0x2c, 0x18}, "reserved bit"},
	} {
		t.Run(tc.name, func(t *testing.T) {
			_, err := ParseHeader(tc.header)
			if err == nil {
				t.Fatal("ParseHeader() succeeded, want an error")
			}
			if !strings.Contains(err.Error(), tc.want) {
				t.Errorf("error %q does not contain %q", err, tc.want)
			}
		})
	}
}

func TestDictionaryID(t *testing.T) {
	id, err := DictionaryID([]byte{0x37, 0xa4, 0x30, 0xec, 0x2a, 0x00, 0x00, 0x00, 0xff})
	if err != nil {
		t.Fatalf("DictionaryID() failed: %v", err)
	}
	if id != 42 {
		t.Errorf("DictionaryID() = %d, want 42", id)
	}

	if _, err := DictionaryID([]byte{0x37, 0xa4, 0x30, 0xec, 0x2a, 0x00, 0x00}); err == nil {
		t.Error("DictionaryID() succeeded for a truncated header")
	}
	if _, err := DictionaryID([]byte{0x28, 0xb5, 0x2f, 0xfd, 0x24, 0x18, 0x00, 0x00}); err == nil {
		t.Error("DictionaryID() succeeded for a frame")
	}
}
