// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// ButtonKind mirrors pomdog::detail::ButtonKind (int8).
const (
	ButtonInvalid       int8 = -1
	ButtonA             int8 = 0
	ButtonB             int8 = 1
	ButtonX             int8 = 2
	ButtonY             int8 = 3
	ButtonLeftShoulder  int8 = 4
	ButtonRightShoulder int8 = 5
	ButtonLeftTrigger   int8 = 6
	ButtonRightTrigger  int8 = 7
	ButtonLeftMenu      int8 = 8
	ButtonRightMenu     int8 = 9
	ButtonLeftStick     int8 = 10
	ButtonRightStick    int8 = 11
	ButtonGuide         int8 = 12
	ButtonExtra1        int8 = 13
	ButtonExtra2        int8 = 14
	ButtonDPadUp        int8 = 15
	ButtonDPadDown      int8 = 16
	ButtonDPadLeft      int8 = 17
	ButtonDPadRight     int8 = 18
)

// ThumbStickKind mirrors pomdog::detail::ThumbStickKind (int8).
const (
	ThumbStickInvalid     int8 = -1
	ThumbStickLeftStickX  int8 = 0
	ThumbStickLeftStickY  int8 = 1
	ThumbStickRightStickX int8 = 2
	ThumbStickRightStickY int8 = 3
)

const numButtons = 16
const numAxes = 6
const numHats = 4

// sdlKeyToButton maps SDL_GameControllerDB field names to (ButtonKind, ThumbStickKind).
var sdlKeyToButton = map[string]struct {
	button     int8
	thumbStick int8
}{
	"a":             {ButtonA, ThumbStickInvalid},
	"b":             {ButtonB, ThumbStickInvalid},
	"x":             {ButtonX, ThumbStickInvalid},
	"y":             {ButtonY, ThumbStickInvalid},
	"guide":         {ButtonGuide, ThumbStickInvalid},
	"back":          {ButtonLeftMenu, ThumbStickInvalid},
	"start":         {ButtonRightMenu, ThumbStickInvalid},
	"leftshoulder":  {ButtonLeftShoulder, ThumbStickInvalid},
	"rightshoulder": {ButtonRightShoulder, ThumbStickInvalid},
	"leftstick":     {ButtonLeftStick, ThumbStickInvalid},
	"rightstick":    {ButtonRightStick, ThumbStickInvalid},
	"dpup":          {ButtonDPadUp, ThumbStickInvalid},
	"dpdown":        {ButtonDPadDown, ThumbStickInvalid},
	"dpleft":        {ButtonDPadLeft, ThumbStickInvalid},
	"dpright":       {ButtonDPadRight, ThumbStickInvalid},
	"lefttrigger":   {ButtonLeftTrigger, ThumbStickInvalid},
	"righttrigger":  {ButtonRightTrigger, ThumbStickInvalid},
	"leftx":         {ButtonInvalid, ThumbStickLeftStickX},
	"lefty":         {ButtonInvalid, ThumbStickLeftStickY},
	"rightx":        {ButtonInvalid, ThumbStickRightStickX},
	"righty":        {ButtonInvalid, ThumbStickRightStickY},
}

// normalizePlatform converts SDL_GameControllerDB platform names to lowercase keys.
func normalizePlatform(sdlPlatform string) string {
	switch sdlPlatform {
	case "Windows":
		return "windows"
	case "Linux":
		return "linux"
	case "Mac OS X":
		return "macos"
	case "Android":
		return "android"
	case "iOS":
		return "ios"
	default:
		return ""
	}
}

type axisMapping struct {
	thumbStick      int8
	positiveTrigger int8
	negativeTrigger int8
}

type controllerEntry struct {
	uuid     string
	name     string
	platform string // normalized: "windows", "linux", etc.
	buttons  [numButtons]int8
	axes     [numAxes]axisMapping
	hats     [numHats]int8
}

func parseSDLGameControllerDB(filePath string) ([]controllerEntry, error) {
	f, err := os.Open(filePath)
	if err != nil {
		return nil, fmt.Errorf("os.Open() failed: %w", err)
	}
	defer f.Close()

	var entries []controllerEntry

	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" || strings.HasPrefix(line, "#") {
			continue
		}

		fields := strings.Split(line, ",")
		if len(fields) < 3 {
			continue
		}

		entry := controllerEntry{}
		entry.uuid = fields[0]
		entry.name = fields[1]

		// NOTE: Initialize buttons to Invalid
		for i := range entry.buttons {
			entry.buttons[i] = ButtonInvalid
		}
		// NOTE: Initialize axes to Invalid
		for i := range entry.axes {
			entry.axes[i] = axisMapping{
				thumbStick:      ThumbStickInvalid,
				positiveTrigger: ButtonInvalid,
				negativeTrigger: ButtonInvalid,
			}
		}
		// NOTE: Initialize hats to default DPad mappings
		entry.hats = [numHats]int8{ButtonDPadUp, ButtonDPadRight, ButtonDPadDown, ButtonDPadLeft}

		for _, field := range fields[2:] {
			if strings.HasPrefix(field, "platform:") {
				entry.platform = normalizePlatform(strings.TrimPrefix(field, "platform:"))
				continue
			}
			if field == "" {
				continue
			}

			parts := strings.SplitN(field, ":", 2)
			if len(parts) != 2 {
				continue
			}
			key := parts[0]
			value := parts[1]

			kind, ok := sdlKeyToButton[key]
			if !ok {
				continue
			}

			if strings.HasPrefix(value, "b") {
				// NOTE: Button mapping: bN
				idxStr := strings.TrimPrefix(value, "b")
				idx, parseErr := strconv.Atoi(idxStr)
				if parseErr != nil || idx < 0 || idx >= numButtons {
					continue
				}
				entry.buttons[idx] = kind.button
			} else if strings.HasPrefix(value, "+a") {
				// NOTE: Positive half-axis: +aN or +aN~
				s := strings.TrimSuffix(value, "~")
				s = strings.TrimPrefix(s, "+a")
				idx, parseErr := strconv.Atoi(s)
				if parseErr != nil || idx < 0 || idx >= numAxes {
					continue
				}
				entry.axes[idx].positiveTrigger = kind.button
			} else if strings.HasPrefix(value, "-a") {
				// NOTE: Negative half-axis: -aN or -aN~
				s := strings.TrimSuffix(value, "~")
				s = strings.TrimPrefix(s, "-a")
				idx, parseErr := strconv.Atoi(s)
				if parseErr != nil || idx < 0 || idx >= numAxes {
					continue
				}
				entry.axes[idx].negativeTrigger = kind.button
			} else if strings.HasPrefix(value, "a") {
				// NOTE: Full axis: aN or aN~
				s := strings.TrimSuffix(value, "~")
				s = strings.TrimPrefix(s, "a")
				idx, parseErr := strconv.Atoi(s)
				if parseErr != nil || idx < 0 || idx >= numAxes {
					continue
				}
				entry.axes[idx].thumbStick = kind.thumbStick
				entry.axes[idx].positiveTrigger = kind.button
			} else if strings.HasPrefix(value, "h") {
				// NOTE: Hat mapping: h<hat_index>.<bitmask>
				// Only hat 0 is supported. Bitmask: 1=up, 2=right, 4=down, 8=left.
				rest := strings.TrimPrefix(value, "h")
				dotParts := strings.SplitN(rest, ".", 2)
				if len(dotParts) != 2 {
					continue
				}
				hatIdx, parseErr := strconv.Atoi(dotParts[0])
				if parseErr != nil || hatIdx != 0 {
					continue
				}
				mask, parseErr := strconv.Atoi(dotParts[1])
				if parseErr != nil {
					continue
				}
				// Convert bitmask to direction index: 1->0(up), 2->1(right), 4->2(down), 8->3(left)
				var dirIdx int
				switch mask {
				case 1:
					dirIdx = 0
				case 2:
					dirIdx = 1
				case 4:
					dirIdx = 2
				case 8:
					dirIdx = 3
				default:
					continue
				}
				if kind.button != ButtonInvalid {
					entry.hats[dirIdx] = kind.button
				}
			}
		}

		if entry.platform == "" {
			// NOTE: Skip entries without a recognized platform
			continue
		}

		entries = append(entries, entry)
	}

	if err := scanner.Err(); err != nil {
		return nil, fmt.Errorf("scanner error: %w", err)
	}

	return entries, nil
}
