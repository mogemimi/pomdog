// Copyright mogemimi. Distributed under the MIT license.

package stringhash

import (
	"io"
	"strings"

	"github.com/OneOfOne/xxhash"
)

func StringToHash32(s string) uint32 {
	const seed = uint32(20160723)
	h := xxhash.NewS32(seed)
	r := strings.NewReader(s)
	io.Copy(h, r)
	return h.Sum32()
}

func StringToHash64(s string) uint64 {
	const seed = uint64(20160723)
	h := xxhash.NewS64(seed)
	r := strings.NewReader(s)
	io.Copy(h, r)
	return h.Sum64()
}
