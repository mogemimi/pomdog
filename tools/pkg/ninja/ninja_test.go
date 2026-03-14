// Copyright mogemimi. Distributed under the MIT license.

package ninja

import (
	"strings"
	"testing"
)

func TestNewVariableAsString(t *testing.T) {
	v := NewVariableAsString("key", "value")
	if v.Key != "key" || v.Value != "value" || v.AsPath {
		t.Errorf("unexpected variable: %+v", v)
	}
}

func TestNewVariableAsPath(t *testing.T) {
	v := NewVariableAsPath("exe", "/usr/bin/tool")
	if v.Key != "exe" || v.Value != "/usr/bin/tool" || !v.AsPath {
		t.Errorf("unexpected variable: %+v", v)
	}
}

func TestGenerateEmpty(t *testing.T) {
	gen := &Generator{}
	result := gen.Generate()
	if result != "" {
		t.Errorf("expected empty string, got %q", result)
	}
}

func TestGenerateVariables(t *testing.T) {
	gen := &Generator{}
	gen.AddVariable(NewVariableAsString("cflags", "-Wall -Werror"))
	gen.AddVariable(NewVariableAsPath("cc", "/usr/bin/gcc"))

	result := gen.Generate()
	if !strings.Contains(result, "cflags = -Wall -Werror\n") {
		t.Errorf("missing cflags variable in output:\n%s", result)
	}
	if !strings.Contains(result, "cc = /usr/bin/gcc\n") {
		t.Errorf("missing cc variable in output:\n%s", result)
	}
}

func TestGenerateRule(t *testing.T) {
	gen := &Generator{}
	gen.AddRule(&Rule{
		Name:    "compile",
		Command: "gcc -c $in -o $out",
	})

	result := gen.Generate()
	if !strings.Contains(result, "rule compile\n") {
		t.Errorf("missing rule name in output:\n%s", result)
	}
	if !strings.Contains(result, "  command = gcc -c $in -o $out\n") {
		t.Errorf("missing command in output:\n%s", result)
	}
}

func TestGenerateRuleWithDepfile(t *testing.T) {
	gen := &Generator{}
	gen.AddRule(&Rule{
		Name:    "compile",
		Command: "gcc -c $in -o $out -MMD -MF $out.d",
		DepFile: "$out.d",
		Deps:    "gcc",
	})

	result := gen.Generate()
	if !strings.Contains(result, "  depfile = $out.d\n") {
		t.Errorf("missing depfile in output:\n%s", result)
	}
	if !strings.Contains(result, "  deps = gcc\n") {
		t.Errorf("missing deps in output:\n%s", result)
	}
}

func TestGenerateBuildSimple(t *testing.T) {
	gen := &Generator{}
	gen.AddRule(&Rule{
		Name:    "copy",
		Command: "cp $in $out",
	})
	gen.AddBuild(&Build{
		Rule:    "copy",
		OutFile: "out/file.txt",
		InFiles: []string{"src/file.txt"},
	})

	result := gen.Generate()
	if !strings.Contains(result, "build out/file.txt: copy src/file.txt\n") {
		t.Errorf("unexpected build statement in output:\n%s", result)
	}
}

func TestGenerateBuildWithImplicitOutputs(t *testing.T) {
	gen := &Generator{}
	gen.AddRule(&Rule{
		Name:    "archive",
		Command: "archive-tool -o $out $in",
	})
	gen.AddBuild(&Build{
		Rule:             "archive",
		OutFile:          "out/content.idx",
		ImplicitOutFiles: []string{"out/content.pak"},
		InFiles:          []string{"recipe.toml"},
	})

	result := gen.Generate()
	if !strings.Contains(result, "build out/content.idx | out/content.pak: archive recipe.toml\n") {
		t.Errorf("unexpected build statement in output:\n%s", result)
	}
}

func TestGenerateBuildWithImplicitInputs(t *testing.T) {
	gen := &Generator{}
	gen.AddRule(&Rule{
		Name:    "compile",
		Command: "gcc -c $in -o $out",
	})
	gen.AddBuild(&Build{
		Rule:            "compile",
		OutFile:         "out/main.o",
		InFiles:         []string{"src/main.c"},
		ImplicitInFiles: []string{"src/header.h"},
	})

	result := gen.Generate()
	if !strings.Contains(result, "| $\n  src/header.h\n") {
		t.Errorf("unexpected implicit input in output:\n%s", result)
	}
}

func TestGenerateBuildWithOrderOnlyDeps(t *testing.T) {
	gen := &Generator{}
	gen.AddRule(&Rule{
		Name:    "archive",
		Command: "archive-tool -o $out $in",
	})
	gen.AddBuild(&Build{
		Rule:          "archive",
		OutFile:       "out/archive.pak",
		InFiles:       []string{"recipe.toml"},
		OrderOnlyDeps: []string{"out/shader.spv", "out/texture.png"},
	})

	result := gen.Generate()
	if !strings.Contains(result, "|| $\n  out/shader.spv $\n  out/texture.png\n") {
		t.Errorf("unexpected order-only deps in output:\n%s", result)
	}
}

func TestGenerateBuildWithVariables(t *testing.T) {
	gen := &Generator{}
	gen.AddRule(&Rule{
		Name:    "compile",
		Command: "gcc -c $in -o $out $cflags",
	})
	gen.AddBuild(&Build{
		Rule:    "compile",
		OutFile: "out/main.o",
		InFiles: []string{"src/main.c"},
		Variables: []*Variable{
			NewVariableAsString("cflags", "-DDEBUG"),
		},
	})

	result := gen.Generate()
	if !strings.Contains(result, "  cflags = -DDEBUG\n") {
		t.Errorf("missing build variable in output:\n%s", result)
	}
}

func TestGenerateBuildMultipleInputs(t *testing.T) {
	gen := &Generator{}
	gen.AddRule(&Rule{
		Name:    "link",
		Command: "gcc $in -o $out",
	})
	gen.AddBuild(&Build{
		Rule:    "link",
		OutFile: "out/app",
		InFiles: []string{"out/a.o", "out/b.o"},
	})

	result := gen.Generate()
	// NOTE: With 2+ inputs, the format uses `$\n` continuation
	if !strings.Contains(result, "link $\n  out/a.o $\n  out/b.o\n") {
		t.Errorf("unexpected multi-input build in output:\n%s", result)
	}
}

func TestGenerateSubninja(t *testing.T) {
	gen := &Generator{}
	gen.AddSubninja("build/shaders.ninja")
	gen.AddSubninja("build/copy.ninja")

	result := gen.Generate()
	if !strings.Contains(result, "subninja build/shaders.ninja\n") {
		t.Errorf("missing first subninja in output:\n%s", result)
	}
	if !strings.Contains(result, "subninja build/copy.ninja\n") {
		t.Errorf("missing second subninja in output:\n%s", result)
	}
}

func TestEscapeNinjaPathSpaces(t *testing.T) {
	gen := &Generator{}
	gen.AddRule(&Rule{
		Name:    "copy",
		Command: "cp $in $out",
	})
	gen.AddBuild(&Build{
		Rule:    "copy",
		OutFile: "out/my file.txt",
		InFiles: []string{"src/my file.txt"},
	})

	result := gen.Generate()
	if !strings.Contains(result, "out/my$ file.txt") {
		t.Errorf("space not escaped in output:\n%s", result)
	}
}

func TestGenerateFullPipeline(t *testing.T) {
	gen := &Generator{}
	gen.AddVariable(NewVariableAsPath("cc", "/usr/bin/gcc"))
	gen.AddRule(&Rule{
		Name:    "compile",
		Command: "$cc -c $in -o $out",
	})
	gen.AddBuild(&Build{
		Rule:    "compile",
		OutFile: "out/main.o",
		InFiles: []string{"src/main.c"},
	})
	gen.AddSubninja("other/build.ninja")

	result := gen.Generate()

	// NOTE: Check ordering: variables, then rules, then builds, then subninjas
	varIdx := strings.Index(result, "cc = ")
	ruleIdx := strings.Index(result, "rule compile")
	buildIdx := strings.Index(result, "build out/main.o")
	subninjaIdx := strings.Index(result, "subninja other/build.ninja")

	if varIdx < 0 || ruleIdx < 0 || buildIdx < 0 || subninjaIdx < 0 {
		t.Fatalf("missing sections in output:\n%s", result)
	}
	if !(varIdx < ruleIdx && ruleIdx < buildIdx && buildIdx < subninjaIdx) {
		t.Errorf("sections not in expected order (var=%d, rule=%d, build=%d, subninja=%d)", varIdx, ruleIdx, buildIdx, subninjaIdx)
	}
}
