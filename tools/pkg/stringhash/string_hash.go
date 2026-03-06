// Copyright mogemimi. Distributed under the MIT license.

package stringhash

import (
	"io"
	"strings"

	"github.com/OneOfOne/xxhash"
)

// Seed32 is the xxHash-32 seed used by StringToHash32.
const Seed32 = uint32(20160723)

// Seed64 is the xxHash-64 seed used by StringToHash64.
const Seed64 = uint64(20160723)

func StringToHash32(s string) uint32 {
	h := xxhash.NewS32(Seed32)
	r := strings.NewReader(s)
	io.Copy(h, r)
	return h.Sum32()
}

func StringToHash64(s string) uint64 {
	h := xxhash.NewS64(Seed64)
	r := strings.NewReader(s)
	io.Copy(h, r)
	return h.Sum64()
}
