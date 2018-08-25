// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"strings"
)

func createEmbeddedCode(identifier, prefix, content string) string {
	name := strings.Replace(identifier, ".", "_", -1)
	name = strings.Replace(identifier, "\\", "_", -1)
	name = strings.Replace(identifier, "/", "_", -1)
	name = prefix + name

	return fmt.Sprintf(`// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto %s = R"(
%s)";
`, name, content)
}

func createEmbeddedBinary(identifier, prefix, content string) string {
	name := strings.Replace(identifier, ".", "_", -1)
	name = strings.Replace(identifier, "\\", "_", -1)
	name = strings.Replace(identifier, "/", "_", -1)
	name = prefix + name

	return fmt.Sprintf(`// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr uint8_t %s[] = {
%s
};
`, name, content)
}

func fillSpaces(n int) (s string) {
	for i := 0; i < n; i++ {
		s += " "
	}
	return s
}

func binaryToByteArrayString(bin []byte) string {
	const elementsPerLine = 15
	const tabWidth = 4
	content := ""
	for i := range bin {
		if i > 0 {
			if (i % elementsPerLine) == 0 {
				content += "\n"
			} else {
				prev := fmt.Sprintf("%d", int(bin[i-1]))
				content += fillSpaces(4 - len(prev))
			}
		}
		if (i % elementsPerLine) == 0 {
			content += fillSpaces(tabWidth)
		}
		content += fmt.Sprintf("%d,", int(bin[i]))
	}
	return content
}
