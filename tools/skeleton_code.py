# coding: UTF-8

# examples:
# $ python tools/skeleton_code.py src/Example
# Create implement file: src/Example.cpp
# Create header file: src/Example.hpp

import sys
import os

argvs = sys.argv
arg_count = len(argvs)

if (arg_count < 2):
    print 'Usage: $ python %s filename INCLUDE_GUARD_NAME' % argvs[0]
    quit()

filename, file_extension = os.path.splitext(argvs[1]);


include_guard_element = "POMDOG_"

if (arg_count <= 2) and (arg_count < 3):
	include_guard_element += os.path.split(filename)[1].upper();
elif (arg_count <= 3):
	include_guard_element += argvs[2]
	
include_guard_element += "_HPP"
	
include_guard_begin = "#ifndef " + include_guard_element + "\n" + "#define " + include_guard_element + "\n" +"""
#if (_MSC_VER > 1000)
#	pragma once
#endif

"""

include_guard_end = "\n#endif // !defined("+ include_guard_element +")\n"

header = """//
//  Copyright (C) 2013 the Pomdog Engine authors.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

"""

footer = """namespace Pomdog {
namespace Details {

}// namespace Details
}// namespace Pomdog
"""

implement_filename = filename + '.cpp';
header_filename = filename + '.hpp';

include_header = "#include \"" + os.path.split(header_filename)[1] + "\"\n\n";

bom = u'\ufeff'

directory_path = os.path.dirname(filename)

if not os.path.exists(directory_path):
	os.mkdir(directory_path)
	print "Create new directory: " + directory_path

if (not file_extension) or (file_extension == '.cpp'):
	f = open(implement_filename, 'wb')
	f.write(bom.encode('utf-8'))
	f.write(header)
	f.write(include_header)
	f.write(footer)
	f.close()
	print "Create implement file: " + implement_filename

if (not file_extension) or (file_extension == '.hpp'):
	f = open(header_filename, 'wb')
	f.write(bom.encode('utf-8'))
	f.write(header)
	f.write(include_guard_begin)
	f.write(footer)
	f.write(include_guard_end)
	f.close()
	print "Create header file: " + header_filename
