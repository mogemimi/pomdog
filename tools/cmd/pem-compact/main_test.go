// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"encoding/base64"
	"encoding/pem"
	"os"
	"strings"
	"testing"
)

// block returns a CERTIFICATE block of data, with the Base64 in lines of width
// characters.
func block(data []byte, width int) string {
	encoded := base64.StdEncoding.EncodeToString(data)
	var lines []string
	for len(encoded) > width {
		lines = append(lines, encoded[:width])
		encoded = encoded[width:]
	}
	lines = append(lines, encoded)
	return beginLine + "\n" + strings.Join(lines, "\n") + "\n" + endLine + "\n"
}

func sequence(size int, offset byte) []byte {
	data := make([]byte, size)
	for i := range data {
		data[i] = byte(i) + offset
	}
	return data
}

func TestCompactDropsCommentsAndBlankLines(t *testing.T) {
	first := sequence(100, 0)
	second := sequence(10, 50)
	input := "# Root certificates\r\n" +
		"#\r\n" +
		"\r\n" +
		"# NOTE: The first root.\r\n" +
		strings.ReplaceAll(block(first, 64), "\n", "  \r\n") +
		"\n\n" +
		"# NOTE: The second root.\n" +
		block(second, 64)

	got, err := compact([]byte(input))
	if err != nil {
		t.Fatalf("compact() failed: %v", err)
	}
	if want := block(first, 64) + block(second, 64); string(got) != want {
		t.Errorf("compact() = %q, want %q", got, want)
	}
}

func TestCompactWrapsLinesAt64Characters(t *testing.T) {
	data := sequence(200, 7)
	got, err := compact([]byte(block(data, 76)))
	if err != nil {
		t.Fatalf("compact() failed: %v", err)
	}
	if want := block(data, 64); string(got) != want {
		t.Errorf("compact() = %q, want %q", got, want)
	}
}

func TestCompactRejectsInvalidSyntax(t *testing.T) {
	valid := block(sequence(30, 0), 64)
	for _, c := range []struct {
		name  string
		input string
	}{
		{"no block", "# only a comment\n\n"},
		{"empty input", ""},
		{"text outside a block", "not a comment\n" + valid},
		{"comment with leading spaces", "  # indented\n" + valid},
		{"another block type", strings.ReplaceAll(valid, "CERTIFICATE", "PRIVATE KEY")},
		{"mismatched end", strings.Replace(valid, endLine, "-----END PRIVATE KEY-----", 1)},
		{"nested begin", strings.Replace(valid, "\n"+endLine, "\n"+beginLine, 1)},
		{"missing end", strings.Replace(valid, endLine+"\n", "", 1)},
		{"header", strings.Replace(valid, beginLine+"\n", beginLine+"\nProc-Type: 4,ENCRYPTED\n", 1)},
		{"blank line inside", strings.Replace(valid, beginLine+"\n", beginLine+"\n\n", 1)},
		{"comment inside", strings.Replace(valid, beginLine+"\n", beginLine+"\n# comment\n", 1)},
		{"invalid character", strings.Replace(valid, beginLine+"\n", beginLine+"\n*", 1)},
		{"missing padding", beginLine + "\nAAECAwQ\n" + endLine + "\n"},
		{"non-zero padding bits", beginLine + "\nAB==\n" + endLine + "\n"},
		{"empty block", beginLine + "\n" + endLine + "\n"},
	} {
		t.Run(c.name, func(t *testing.T) {
			if got, err := compact([]byte(c.input)); err == nil {
				t.Errorf("compact() = %q, want an error", got)
			}
		})
	}
}

func TestCompactKeepsTheEngineRoots(t *testing.T) {
	input, err := os.ReadFile("../../../assets/certificates/root_certificates.pem")
	if err != nil {
		t.Fatalf("os.ReadFile() failed: %v", err)
	}
	got, err := compact(input)
	if err != nil {
		t.Fatalf("compact() failed: %v", err)
	}

	// NOTE: encoding/pem skips the comments, so it reads the same blocks.
	rest, compacted := input, got
	count := 0
	for {
		var want, have *pem.Block
		want, rest = pem.Decode(rest)
		have, compacted = pem.Decode(compacted)
		if (want == nil) || (have == nil) {
			if want != have {
				t.Fatalf("the output has a different number of blocks than the input after %d blocks", count)
			}
			break
		}
		if !bytes.Equal(want.Bytes, have.Bytes) {
			t.Fatalf("block %d differs", count)
		}
		count++
	}
	if count == 0 {
		t.Fatal("the bundle has no blocks")
	}
	if bytes.Contains(got, []byte("#")) || bytes.Contains(got, []byte("\n\n")) {
		t.Error("the output has comments or blank lines")
	}
}
