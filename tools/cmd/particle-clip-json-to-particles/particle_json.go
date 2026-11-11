// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"encoding/json"
	"errors"
)

// clipJSON is a particle clip in its JSON source format. Every field is a
// pointer, so that a missing field differs from a zero value.
type clipJSON struct {
	Duration             *float64       `json:"duration"`
	EmissionRate         *float64       `json:"emission_rate"`
	EmissionRateOverTime *float64       `json:"emission_rate_over_time"`
	StartLifetime        *float64       `json:"start_lifetime"`
	StartDelay           *float64       `json:"start_delay"`
	GravityModifier      *float64       `json:"gravity_modifier"`
	MaxParticles         *float64       `json:"max_particles"`
	Looping              *bool          `json:"looping"`
	Shape                *shapeJSON     `json:"shape"`
	StartSpeed           *parameterJSON `json:"start_speed"`
	StartColor           *parameterJSON `json:"start_color"`
	ColorOverLifetime    *parameterJSON `json:"color_over_lifetime"`
	StartRotation        *parameterJSON `json:"start_rotation"`
	RotationOverLifetime *parameterJSON `json:"rotation_over_lifetime"`
	StartSize            *parameterJSON `json:"start_size"`
	SizeOverLifetime     *parameterJSON `json:"size_over_lifetime"`
}

// shapeJSON is an emitter shape. "type" chooses which of the other fields it
// uses. "angle" is in degrees.
type shapeJSON struct {
	Type   string   `json:"type"`
	Width  *float64 `json:"width"`
	Height *float64 `json:"height"`
	Depth  *float64 `json:"depth"`
	Radius *float64 `json:"radius"`
	Angle  *float64 `json:"angle"`
}

// parameterJSON is a parameter. A "constant" has "value", a "random" has
// "min" and "max", and a "curve" has "points". The values are numbers, or
// arrays of four channels for colors.
type parameterJSON struct {
	Type   string            `json:"type"`
	Value  json.RawMessage   `json:"value"`
	Min    json.RawMessage   `json:"min"`
	Max    json.RawMessage   `json:"max"`
	Points *[]curvePointJSON `json:"points"`
}

// curvePointJSON is a key of a curve at the normalized lifetime "t".
type curvePointJSON struct {
	T *float64        `json:"t"`
	V json.RawMessage `json:"v"`
}

// parseClipJSON decodes a particle clip. It rejects unknown fields, so that a
// misspelled field does not go unnoticed.
func parseClipJSON(data []byte) (*clipJSON, error) {
	decoder := json.NewDecoder(bytes.NewReader(data))
	decoder.DisallowUnknownFields()

	clip := &clipJSON{}
	if err := decoder.Decode(clip); err != nil {
		return nil, err
	}
	if decoder.More() {
		return nil, errors.New("unexpected data after the JSON value")
	}
	return clip, nil
}
