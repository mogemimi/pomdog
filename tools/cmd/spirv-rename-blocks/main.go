// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"os"
	"strings"

	spvparse "github.com/mogemimi/pomdog/tools/pkg/spvparse"
)

const (
	opName = 5
)

func main() {
	inPath := flag.String("i", "", "input SPIR-V file path")
	outPath := flag.String("o", "", "output SPIR-V file path")
	suffix := flag.String("suffix", "_std140", "suffix to strip from OpName instructions")
	verbose := flag.Bool("v", false, "verbose output")
	flag.Parse()

	if *inPath == "" || *outPath == "" {
		fmt.Fprintln(os.Stderr, "usage: spirv-rename-blocks -i <input.spv> -o <output.spv> [-suffix _std140]")
		os.Exit(1)
	}

	if err := run(*inPath, *outPath, *suffix, *verbose); err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		os.Exit(1)
	}
}

func run(inPath, outPath, suffix string, verbose bool) error {
	data, err := os.ReadFile(inPath)
	if err != nil {
		return fmt.Errorf("failed to read input: %w", err)
	}

	words := spvparse.ToWords(data)
	if len(words) < 5 {
		return fmt.Errorf("SPIR-V too short")
	}

	// NOTE: Rebuild the instruction stream, replacing OpName instructions that have
	// the target suffix with a shorter version.
	header := words[:5]
	var result []uint32
	result = append(result, header...)

	renamed := 0
	i := 5
	for i < len(words) {
		wc := int(words[i] >> 16)
		op := words[i] & 0xFFFF
		if wc == 0 {
			return fmt.Errorf("invalid instruction length 0 at word %d", i)
		}
		if i+wc > len(words) {
			return fmt.Errorf("instruction at word %d extends past end", i)
		}

		if op == opName && wc >= 3 {
			targetID := words[i+1]
			name := decodeString(words[i+2 : i+wc])

			if strings.HasSuffix(name, suffix) {
				newName := strings.TrimSuffix(name, suffix)
				nameWords := spvparse.EncodeString(newName)

				// NOTE: OpName header (1) + target ID (1) + name words
				newWC := 2 + len(nameWords)

				newInstr := make([]uint32, newWC)
				newInstr[0] = uint32(newWC<<16) | opName
				newInstr[1] = targetID
				copy(newInstr[2:], nameWords)
				result = append(result, newInstr...)
				if verbose {
					fmt.Fprintf(os.Stdout, "renamed: %q -> %q (ID %d)\n", name, newName, targetID)
				}
				renamed++
				i += wc
				continue
			}
		}

		result = append(result, words[i:i+wc]...)
		i += wc
	}

	if renamed == 0 {
		// NOTE: Nothing changed; just copy.
		return os.WriteFile(outPath, data, 0644)
	}

	outData := spvparse.FromWords(result)
	return os.WriteFile(outPath, outData, 0644)
}

// decodeString decodes a null-terminated UTF-8 string from SPIR-V word operands.
func decodeString(words []uint32) string {
	var buf []byte
	for _, w := range words {
		buf = append(buf, byte(w), byte(w>>8), byte(w>>16), byte(w>>24))
	}
	if idx := strings.IndexByte(string(buf), 0); idx >= 0 {
		buf = buf[:idx]
	}
	return string(buf)
}
