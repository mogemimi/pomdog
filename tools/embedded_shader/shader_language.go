// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

package main

type shaderLanguage int

const (
	shaderLanguageGLSL shaderLanguage = iota
	shaderLanguageHLSL
	shaderLanguageMetal
)

func findShaderLanguageFromExtension(ext string) shaderLanguage {
	switch ext {
	case ".glsl":
		fallthrough
	case ".vert":
		fallthrough
	case ".flag":
		return shaderLanguageGLSL
	case ".hlsl":
		fallthrough
	case ".fx":
		return shaderLanguageHLSL
	case ".metal":
		return shaderLanguageMetal
	}
	return shaderLanguageGLSL
}
