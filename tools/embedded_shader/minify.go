// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

package main

import (
	"regexp"
	"strings"
)

func minifyCode(content string) string {
	content = removeMultiLineComments(content)
	content = removeBCPLStyleComments(content)

	lines := strings.Split(content, "\n")
	for i, line := range lines {
		line = strings.Trim(line, " \t")
		if !strings.HasPrefix(line, "#") {
			line = strings.Replace(line, " (", "(", -1)
			line = strings.Replace(line, ") ", ")", -1)
			line = strings.Replace(line, " {", "{", -1)
			line = strings.Replace(line, "} ", "}", -1)
		}
		lines[i] = line
	}
	content = strings.Join(lines, "\n")

	content = regexp.MustCompile(`([\t ]*)([\+\-\*/=>,\]])([\t ]*)`).ReplaceAllString(content, `$2`)
	content = regexp.MustCompile(`([\t ]+)([\[<])([\t ]*)`).ReplaceAllString(content, ` $2`)
	content = regexp.MustCompile(`([\t ]+)`).ReplaceAllString(content, ` `)
	content = strings.Replace(content, "\n{", "{", -1)
	content = strings.Replace(content, "\n}", "}", -1)
	content = strings.Replace(content, "\n(", "(", -1)
	content = strings.Replace(content, "\n)", ")", -1)
	content = strings.Replace(content, "\n;", ";", -1)

	content = removeEmptyLine(content)

	return content
}

func removeMultiLineComments(content string) string {
	re := regexp.MustCompile(`(?m)/\*[\p{L}\s\r\n\w]+\*/`)
	return re.ReplaceAllString(content, "")
}

func removeBCPLStyleComments(content string) string {
	re := regexp.MustCompile(`//.*?\n`)
	return re.ReplaceAllString(content, "")
}

func removeEmptyLine(content string) string {
	re := regexp.MustCompile(`(?m)\n[\n\s\t]*\n`)
	s := re.ReplaceAllString(content, "\n\n")
	s = strings.Replace(s, "\n\n", "\n", -1)
	return strings.TrimRight(s, " \t\n") + "\n"
}
