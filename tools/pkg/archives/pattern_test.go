// Copyright mogemimi. Distributed under the MIT license.

package archives

import "testing"

func TestMatchPath(t *testing.T) {
	for _, tc := range []struct {
		pattern string
		name    string
		want    bool
	}{
		{"shaders/webgl/*.glsl", "shaders/webgl/sprite_vs.glsl", true},
		{"shaders/webgl/*.glsl", "shaders/glsl/sprite_vs.glsl", false},
		{"shaders/webgl/*.glsl", "shaders/webgl/sub/sprite_vs.glsl", false},
		{"**/*.json", "particles/fire2d.json", true},
		{"**/*.json", "skeletal2d/Ghost/Ghost.json", true},
		{"**/*.json", "fire2d.json", true},
		{"**/*.json", "particles/fire2d.json.bak", false},
		{"skeletal2d/**", "skeletal2d/Ghost/Ghost.json", true},
		{"skeletal2d/**", "skeletal2d", true},
		{"skeletal2d/**/*.tileset", "skeletal2d/Ghost/Ghost.tileset", true},
		{"skeletal2d/**/*.tileset", "skeletal2d/Ghost.tileset", true},
		{"skeletal2d/**/*.tileset", "pixelart/misc.tileset", false},
		{"svg/md-arrow-?p.svg", "svg/md-arrow-up.svg", true},
		{"svg/[im]*-search.svg", "svg/ios-search.svg", true},
		{"svg/[im]*-search.svg", "svg/xs-search.svg", false},
		{"**", "fonts/Roboto-Medium.ttf", true},
	} {
		if err := ValidatePathPattern(tc.pattern); err != nil {
			t.Fatalf("ValidatePathPattern(%q) failed: %v", tc.pattern, err)
		}
		if got := MatchPath(tc.pattern, tc.name); got != tc.want {
			t.Errorf("MatchPath(%q, %q) = %v, want %v", tc.pattern, tc.name, got, tc.want)
		}
	}
}

func TestValidatePathPatternInvalid(t *testing.T) {
	for _, pattern := range []string{
		"",
		"/shaders/*.glsl",
		"shaders//*.glsl",
		"shaders/",
		"shaders/**.glsl",
		"shaders/[a.glsl",
	} {
		if err := ValidatePathPattern(pattern); err == nil {
			t.Errorf("ValidatePathPattern(%q) succeeded, want an error", pattern)
		}
	}
}
