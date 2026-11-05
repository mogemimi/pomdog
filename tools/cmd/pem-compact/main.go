// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"encoding/base64"
	"errors"
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strings"
)

const (
	beginLine = "-----BEGIN CERTIFICATE-----"
	endLine   = "-----END CERTIFICATE-----"

	// lineWidth is the number of Base64 characters per output line (RFC 7468).
	lineWidth = 64
)

func main() {
	inFile := ""
	outFile := ""
	flag.StringVar(&inFile, "i", "", "input PEM file")
	flag.StringVar(&outFile, "o", "", "output PEM file")
	flag.Parse()

	if inFile == "" {
		log.Fatal("-i is required")
	}
	if outFile == "" {
		log.Fatal("-o is required")
	}

	if err := run(inFile, outFile); err != nil {
		log.Fatal(err)
	}
}

func run(inFile, outFile string) error {
	input, err := os.ReadFile(inFile)
	if err != nil {
		return fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	output, err := compact(input)
	if err != nil {
		return fmt.Errorf("%s: %w", inFile, err)
	}

	if err := os.MkdirAll(filepath.Dir(outFile), os.ModePerm); err != nil {
		return fmt.Errorf("os.MkdirAll() failed: %w", err)
	}
	if err := os.WriteFile(outFile, output, 0o644); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}
	return nil
}

// compact returns the CERTIFICATE blocks of input, re-encoded in lines of 64
// characters, without the comments and blank lines between them.
//
// Outside the blocks, input may hold only blank lines and lines that start
// with "#". Inside a block, it may hold only Base64 that decodes to at least
// one byte. compact checks the syntax only, not whether a block holds a valid
// certificate.
func compact(input []byte) ([]byte, error) {
	var out bytes.Buffer
	var body strings.Builder
	inBlock := false
	blockStart := 0
	blockCount := 0

	for i, raw := range strings.Split(string(input), "\n") {
		number := i + 1

		// NOTE: Accept CRLF line ends and trailing spaces.
		line := strings.TrimRight(raw, " \t\r")

		if !inBlock {
			switch {
			case (line == "") || strings.HasPrefix(line, "#"):
			case line == beginLine:
				inBlock = true
				blockStart = number
				body.Reset()
			case strings.HasPrefix(line, "-----BEGIN "):
				return nil, fmt.Errorf("line %d: %q is not %q, the only block that a root bundle may hold", number, line, beginLine)
			default:
				return nil, fmt.Errorf("line %d: unexpected text outside a block: %q", number, line)
			}
			continue
		}

		switch {
		case line == endLine:
			data, err := base64.StdEncoding.Strict().DecodeString(body.String())
			if err != nil {
				return nil, fmt.Errorf("line %d: the block from line %d has invalid Base64: %w", number, blockStart, err)
			}
			if len(data) == 0 {
				return nil, fmt.Errorf("line %d: the block from line %d is empty", number, blockStart)
			}
			writeBlock(&out, data)
			inBlock = false
			blockCount++
		case strings.HasPrefix(line, "-----"):
			return nil, fmt.Errorf("line %d: %q inside the block from line %d, which must end with %q", number, line, blockStart, endLine)
		case line == "":
			return nil, fmt.Errorf("line %d: a blank line inside the block from line %d", number, blockStart)
		case strings.Contains(line, ":"):
			return nil, fmt.Errorf("line %d: the block from line %d has a header, which PEM certificates do not use", number, blockStart)
		default:
			body.WriteString(line)
		}
	}

	if inBlock {
		return nil, fmt.Errorf("line %d: the block has no %q line", blockStart, endLine)
	}
	if blockCount == 0 {
		return nil, errors.New("no " + beginLine + " block")
	}
	return out.Bytes(), nil
}

// writeBlock appends data to out as one CERTIFICATE block.
func writeBlock(out *bytes.Buffer, data []byte) {
	encoded := base64.StdEncoding.EncodeToString(data)
	out.WriteString(beginLine + "\n")
	for len(encoded) > lineWidth {
		out.WriteString(encoded[:lineWidth] + "\n")
		encoded = encoded[lineWidth:]
	}
	out.WriteString(encoded + "\n")
	out.WriteString(endLine + "\n")
}
