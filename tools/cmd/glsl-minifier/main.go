// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"regexp"
	"strings"
)

type Env struct {
	InFile  string
	OutFile string
}

func main() {
	env := &Env{}
	flag.StringVar(&env.InFile, "i", "", "input file")
	flag.StringVar(&env.OutFile, "o", "", "output file")
	flag.Parse()

	if err := run(env); err != nil {
		log.Fatal(err)
	}
}

func run(env *Env) error {
	content := ""
	{
		buf, err := os.ReadFile(env.InFile)
		if err != nil {
			return fmt.Errorf("os.ReadFile() failed: %w", err)
		}
		content = string(buf)
	}

	content = minifyCode(content)

	{
		f, err := os.Create(env.OutFile)
		if err != nil {
			return fmt.Errorf("os.Create() failed: %w", err)
		}
		defer f.Close()

		if _, err := f.WriteString(content); err != nil {
			return fmt.Errorf("f.WriteString() failed: %w", err)
		}
	}

	return nil
}

// NOTE: Pre-compiled regexps for performance.
var (
	reMultiLineComment  = regexp.MustCompile(`/\*[\s\S]*?\*/`)
	reSingleLineComment = regexp.MustCompile(`//[^\n]*`)
	reOperatorSpaces    = regexp.MustCompile(`[ \t]*([\+\-\*/=>,\]\):;])[ \t]*`)
	rePreBracketSpaces  = regexp.MustCompile(`[ \t]+([\[<(])[ \t]*`)
	reCollapseSpaces    = regexp.MustCompile(`[ \t]+`)
	reTrailingZeroFloat = regexp.MustCompile(`\b(\d+)\.0\b`)
	reLeadingZeroFloat  = regexp.MustCompile(`\b0\.(\d+)\b`)
	reEmptyLines        = regexp.MustCompile(`\n[ \t]*\n`)
)

func minifyCode(content string) string {
	content = reMultiLineComment.ReplaceAllString(content, "")
	content = reSingleLineComment.ReplaceAllString(content, "")

	lines := strings.Split(content, "\n")
	for i, line := range lines {
		line = strings.Trim(line, " \t\r")
		if strings.HasPrefix(line, "#") {
			// NOTE: Preprocessor directives must stay on their own line.
			line = line + "$$"
		}
		lines[i] = line
	}
	content = strings.Join(lines, "\n")

	// NOTE: Remove spaces around operators and punctuation.
	content = reOperatorSpaces.ReplaceAllString(content, `$1`)
	content = rePreBracketSpaces.ReplaceAllString(content, ` $1`)
	content = reCollapseSpaces.ReplaceAllString(content, ` `)

	// NOTE: Join braces and brackets with preceding line.
	content = strings.Replace(content, "\n{", "{", -1)
	content = strings.Replace(content, "\n}", "}", -1)
	content = strings.Replace(content, "\n(", "(", -1)
	content = strings.Replace(content, "\n)", ")", -1)
	content = strings.Replace(content, "\n;", ";", -1)

	// NOTE: Compact common patterns.
	content = strings.Replace(content, "){ return ", "){return ", -1)
	content = strings.Replace(content, "if (", "if(", -1)
	content = strings.Replace(content, ";}\nelse{", ";}else{", -1)
	content = strings.Replace(content, ";}else{\n_", ";}else{_", -1)
	content = strings.Replace(content, ";} ", ";}", -1)
	content = strings.Replace(content, ";\n_", ";_", -1)

	// NOTE: Restore preprocessor line breaks.
	content = strings.Replace(content, "$$", "\n", -1)
	content = strings.Replace(content, "\n\n", "\n", -1)

	// NOTE: Shorten float literals: 0.0 -> 0., 1.0 -> 1., etc.
	content = reTrailingZeroFloat.ReplaceAllString(content, `$1.`)

	// NOTE: Shorten leading zero: 0.5 -> .5
	content = reLeadingZeroFloat.ReplaceAllString(content, `.$1`)

	content = removeEmptyLines(content)

	return content
}

func removeEmptyLines(content string) string {
	s := reEmptyLines.ReplaceAllString(content, "\n")
	return strings.TrimRight(s, " \t\n") + "\n"
}
