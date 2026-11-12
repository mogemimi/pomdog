// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"io/fs"
	"log"
	"os"
	"path/filepath"

	flatbuffers "github.com/google/flatbuffers/go"
	schemas "github.com/mogemimi/pomdog/build/schemas"
)

type options struct {
	InFile      string
	OutMesh     string
	OutVertices string
	OutIndices  string
	OutDebug    string
}

func main() {
	opts := &options{}
	flag.StringVar(&opts.InFile, "i", "", "input mesh source file (*.mesh3dsource, required)")
	flag.StringVar(&opts.OutMesh, "out-mesh", "", "output mesh file (*.mesh3d, required)")
	flag.StringVar(&opts.OutVertices, "out-vertices", "", "output vertices file (*.mesh3dvertices, required)")
	flag.StringVar(&opts.OutIndices, "out-indices", "", "output indices file (*.mesh3dindices, required)")
	flag.StringVar(&opts.OutDebug, "out-debug", "", "output debug file (*.mesh3d-debug, optional)")
	flag.Parse()

	if opts.InFile == "" {
		log.Fatal("-i is required")
	}
	if opts.OutMesh == "" {
		log.Fatal("--out-mesh is required")
	}
	if opts.OutVertices == "" {
		log.Fatal("--out-vertices is required")
	}
	if opts.OutIndices == "" {
		log.Fatal("--out-indices is required")
	}

	if err := run(opts); err != nil {
		log.Fatal(err)
	}
}

func run(opts *options) error {
	source, err := readSource(opts.InFile)
	if err != nil {
		return err
	}

	out, err := convert(source)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}

	if err := writeFile(opts.OutMesh, packMesh(out.Mesh)); err != nil {
		return err
	}
	if err := writeFile(opts.OutVertices, out.Vertices); err != nil {
		return err
	}
	if err := writeFile(opts.OutIndices, out.Indices); err != nil {
		return err
	}

	// NOTE: Write debug binary (optional)
	if opts.OutDebug != "" {
		if err := writeFile(opts.OutDebug, packDebug(out.Debug)); err != nil {
			return err
		}
	}

	return nil
}

// readSource reads a mesh source file. The Go runtime of FlatBuffers has no
// verifier and panics on a buffer that is not a mesh source, so the panic is
// turned into an error.
func readSource(path string) (source *schemas.Mesh3DSourceT, err error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	defer func() {
		if r := recover(); r != nil {
			source = nil
			err = fmt.Errorf("%s is not a mesh source file: %v", path, r)
		}
	}()
	return schemas.GetRootAsMesh3DSource(data, 0).UnPack(), nil
}

func packMesh(mesh *schemas.Mesh3DT) []byte {
	builder := flatbuffers.NewBuilder(0)
	builder.Finish(mesh.Pack(builder))
	return builder.FinishedBytes()
}

func packDebug(debug *schemas.Mesh3DDebugT) []byte {
	builder := flatbuffers.NewBuilder(0)
	builder.Finish(debug.Pack(builder))
	return builder.FinishedBytes()
}

func writeFile(path string, data []byte) error {
	if err := os.MkdirAll(filepath.Dir(path), fs.ModePerm); err != nil {
		return fmt.Errorf("os.MkdirAll() failed: %w", err)
	}
	if err := os.WriteFile(path, data, fs.ModePerm); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}
	return nil
}
