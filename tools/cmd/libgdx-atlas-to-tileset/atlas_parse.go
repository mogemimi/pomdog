// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"fmt"
	"strconv"
	"strings"
)

// atlasRegion holds the parsed data for one sprite in a libGDX/Spine .atlas file.
type atlasRegion struct {
	name      string
	pageIndex int
	rotate    bool
	xy        [2]int
	size      [2]int
	orig      [2]int
	offset    [2]int // Spine/libGDX: offset.y is from the bottom
}

// atlasPage holds page-level metadata.
type atlasPage struct {
	textureName string
	width       int
	height      int
}

type parsedAtlas struct {
	pages   []atlasPage
	regions []atlasRegion
}

// parseAtlas parses a libGDX/Spine `.atlas` text file.
//
// Format:
// ```
//
//	<blank line>
//	<page-texture>.png
//	size: W,H
//	format: RGBA8888
//	filter: Linear,Linear
//	repeat: none
//	<sprite-name>
//	  rotate: false
//	  xy: X, Y
//	  size: W, H
//	  orig: W, H
//	  offset: X, Y
//	  index: -1
//
// ```
func parseAtlas(scanner *bufio.Scanner) (*parsedAtlas, error) {
	result := &parsedAtlas{}
	pageIndex := -1

	type state int
	const (
		statePageStart state = iota
		statePageProps
		stateRegionProps
	)
	cur := statePageStart

	var curRegion *atlasRegion

	parsePair := func(s string) (string, string) {
		parts := strings.SplitN(s, ":", 2)
		if len(parts) != 2 {
			return strings.TrimSpace(s), ""
		}
		return strings.TrimSpace(parts[0]), strings.TrimSpace(parts[1])
	}

	parseInt2 := func(s string) ([2]int, error) {
		s = strings.ReplaceAll(s, ",", " ")
		fields := strings.Fields(s)
		if len(fields) < 2 {
			return [2]int{}, fmt.Errorf("expected 2 ints, got %q", s)
		}
		a, err1 := strconv.Atoi(fields[0])
		b, err2 := strconv.Atoi(fields[1])
		if err1 != nil || err2 != nil {
			return [2]int{}, fmt.Errorf("invalid integers in %q", s)
		}
		return [2]int{a, b}, nil
	}

	for scanner.Scan() {
		line := scanner.Text()
		line = strings.TrimRight(line, "\r")

		switch cur {
		case statePageStart:
			if strings.TrimSpace(line) == "" {
				continue
			}
			pageIndex++
			result.pages = append(result.pages, atlasPage{textureName: strings.TrimSpace(line)})
			cur = statePageProps

		case statePageProps:
			if strings.TrimSpace(line) == "" {
				cur = statePageStart
				continue
			}
			key, val := parsePair(line)
			switch key {
			case "size":
				wh, err := parseInt2(val)
				if err != nil {
					return nil, fmt.Errorf("page size: %w", err)
				}
				result.pages[pageIndex].width = wh[0]
				result.pages[pageIndex].height = wh[1]
			case "format", "filter", "repeat", "pma":
				// NOTE: ignore page-level properties that are not used by this tool
			default:
				if !strings.HasPrefix(line, " ") && !strings.HasPrefix(line, "\t") {
					curRegion = &atlasRegion{name: strings.TrimSpace(line), pageIndex: pageIndex}
					result.regions = append(result.regions, *curRegion)
					curRegion = &result.regions[len(result.regions)-1]
					cur = stateRegionProps
				}
			}

		case stateRegionProps:
			if strings.TrimSpace(line) == "" {
				cur = statePageStart
				continue
			}
			if !strings.HasPrefix(line, " ") && !strings.HasPrefix(line, "\t") {
				curRegion = &atlasRegion{name: strings.TrimSpace(line), pageIndex: pageIndex}
				result.regions = append(result.regions, *curRegion)
				curRegion = &result.regions[len(result.regions)-1]
				continue
			}
			key, val := parsePair(line)
			switch key {
			case "rotate":
				curRegion.rotate = strings.HasPrefix(val, "t")
			case "xy":
				xy, err := parseInt2(val)
				if err != nil {
					return nil, fmt.Errorf("region xy: %w", err)
				}
				curRegion.xy = xy
			case "size":
				wh, err := parseInt2(val)
				if err != nil {
					return nil, fmt.Errorf("region size: %w", err)
				}
				curRegion.size = wh
			case "orig":
				wh, err := parseInt2(val)
				if err != nil {
					return nil, fmt.Errorf("region orig: %w", err)
				}
				curRegion.orig = wh
			case "offset":
				xy, err := parseInt2(val)
				if err != nil {
					return nil, fmt.Errorf("region offset: %w", err)
				}
				curRegion.offset = xy
			case "index":
				// NOTE: ignore
			}
		}
	}

	if err := scanner.Err(); err != nil {
		return nil, fmt.Errorf("scanner: %w", err)
	}

	return result, nil
}
