// Copyright mogemimi. Distributed under the MIT license.

package archives

import (
	"errors"
	"fmt"
	"path"
	"strings"
)

// ValidatePathPattern returns an error unless pattern is a valid pattern for MatchPath.
func ValidatePathPattern(pattern string) error {
	if pattern == "" {
		return errors.New("empty path pattern")
	}
	if strings.HasPrefix(pattern, "/") {
		return fmt.Errorf("path pattern %q must be relative", pattern)
	}
	for _, segment := range strings.Split(pattern, "/") {
		if segment == "" {
			return fmt.Errorf("path pattern %q has an empty segment", pattern)
		}
		if segment != "**" && strings.Contains(segment, "**") {
			return fmt.Errorf("path pattern %q uses '**' with other characters in a segment", pattern)
		}
		if _, err := path.Match(segment, ""); err != nil {
			return fmt.Errorf("path pattern %q is malformed: %w", pattern, err)
		}
	}
	return nil
}

// MatchPath reports whether name, a slash-separated path as listed in archive
// recipes, matches pattern.
//
// The pattern is a slash-separated list of segments. A `**` segment matches
// zero or more segments, and any other segment matches one segment with the
// syntax of path.Match, such as `*.json`. The pattern must pass ValidatePathPattern.
func MatchPath(pattern, name string) bool {
	return matchSegments(strings.Split(pattern, "/"), strings.Split(name, "/"))
}

func matchSegments(patterns, names []string) bool {
	for len(patterns) > 0 {
		if patterns[0] == "**" {
			// NOTE: Try every number of segments for `**`, including none.
			for i := 0; i <= len(names); i++ {
				if matchSegments(patterns[1:], names[i:]) {
					return true
				}
			}
			return false
		}
		if len(names) == 0 {
			return false
		}
		// NOTE: Valid patterns have no malformed segments, so Match returns no error here.
		if ok, _ := path.Match(patterns[0], names[0]); !ok {
			return false
		}
		patterns, names = patterns[1:], names[1:]
	}
	return len(names) == 0
}
