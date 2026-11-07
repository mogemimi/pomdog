// Copyright mogemimi. Distributed under the MIT license.

package main

import "testing"

func TestComputeWebSocketAccept(t *testing.T) {
	// NOTE: The example of RFC 6455, section 1.3.
	const key = "dGhlIHNhbXBsZSBub25jZQ=="
	const want = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo="
	if got := computeWebSocketAccept(key); got != want {
		t.Errorf("computeWebSocketAccept(%q) = %q, want %q", key, got, want)
	}
}

func TestHasToken(t *testing.T) {
	tests := []struct {
		value string
		token string
		want  bool
	}{
		{"Upgrade", "upgrade", true},
		{"keep-alive, Upgrade", "Upgrade", true},
		{"keep-alive", "Upgrade", false},
		{"", "Upgrade", false},
		{"Upgrades", "Upgrade", false},
	}
	for _, tt := range tests {
		if got := hasToken(tt.value, tt.token); got != tt.want {
			t.Errorf("hasToken(%q, %q) = %v, want %v", tt.value, tt.token, got, tt.want)
		}
	}
}
