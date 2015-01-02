#
#  Copyright (C) 2013-2015 mogemimi.
#  Distributed under the MIT License. See LICENSE.md or
#  http://enginetrouble.net/pomdog/license for details.
#

import sys
import os
import argparse
import string
import re

# How to use:
# >>> python tools/glsl2embedded.py VertexShader.glsl
# Create new file: VertexShader.embedded.glsl

def ReadGLSLSource(path):
    if not os.path.exists(path):
        print("Error: Cannot find file {0}".format(path))
        return ""
    f = open(path, 'r')
    str = f.read()
    f.close()
    return str

def SaveEmbeddedCode(path, content):
    f = open(path, 'w')
    bom = u'\ufeff'
    f.write(bom.encode('utf-8'))
    f.write(content)
    f.close()
    print "Create new file: " + path

def RemoveWhiteSpace(source):
    result = ""
    for line in source.split('\n'):
        line = line.strip()
        if line:
          result += line
          result += '\n'
    return result

def RemoveCommentOut(source):
    source = re.sub(re.compile("/\*.*?\*/", re.DOTALL), "", source)
    source = re.sub(re.compile("//.*?\n"), "", source)
    return source

def Replace(source, oldString, newString):
    result = ""
    for line in source.split('\n'):
        if not line or line == '\n':
            continue
        if not line.startswith('#'):
            line = line.replace(oldString, newString)
        result += line
        result += '\n';
    return result

def CompressGLSLCode(source):
    preformatted = RemoveCommentOut(RemoveWhiteSpace(source))
    preformatted = preformatted.replace(' + ', '+')
    preformatted = preformatted.replace(' - ', '-')
    preformatted = preformatted.replace(' * ', '*')
    preformatted = preformatted.replace(' / ', '/')
    preformatted = preformatted.replace(' = ', '=')
    preformatted = preformatted.replace(' < ', '<')
    preformatted = preformatted.replace(' > ', '>')
    preformatted = preformatted.replace(' <= ', '<=')
    preformatted = preformatted.replace(' >= ', '>=')
    preformatted = preformatted.replace(' == ', '==')
    preformatted = preformatted.replace(', ', ',')
    preformatted = Replace(preformatted, ' (', '(')
    preformatted = Replace(preformatted, ') ', ')')
    preformatted = Replace(preformatted, ' {', '{')
    preformatted = Replace(preformatted, '} ', '}')
    preformatted = preformatted.replace('\n{', '{')
    preformatted = preformatted.replace('\n}', '}')
    preformatted = preformatted.replace('\n(', '(')
    preformatted = preformatted.replace('\n)', ')')
    preformatted = preformatted.replace('\n;', ';')
    preformatted = preformatted.replace('void main()\n{', 'void main(){')
    return preformatted

def ConvertGLSL2EmbeddedCode(source):
    formatted = CompressGLSLCode(source)
    result = ""
    for line in formatted.split('\n'):
        if not line or line == '\n':
            continue
        line = line.replace('"', '\\"')
        result += '"'
        result += line
        result += '\\n"\n'
    return result

def GetSourceHeader():
    return """//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

"""

def CreateEmbeddedCode(identifier, content):
    name = "Builtin_GLSL_"
    name += identifier.replace('.', '_').replace('\\', '_').replace('/', '_')
    result = GetSourceHeader()
    result += "char const* {0} =\n".format(name)
    result += content.rstrip()
    result += ';\n'
    return result

def ParsingCommandLineAraguments():
    parser = argparse.ArgumentParser(prog='skeleton_generator',
                                     description='Convert GLSL to embedded C++ code.')
    parser.add_argument('identifier', default='def')
    parser.add_argument('-v', '--version', action='version', version='%(prog)s version 0.1.0')
    args = parser.parse_args()
    return args

def Run():
    args = ParsingCommandLineAraguments()

    path = args.identifier
    splittedPath, ext = os.path.splitext(args.identifier)
    identifier = os.path.basename(splittedPath)
    directory = os.path.dirname(args.identifier)

    if not os.path.exists(path):
        print "File does not exist: " + path
        return

    source = ReadGLSLSource(path)
    embedded = CreateEmbeddedCode(identifier, ConvertGLSL2EmbeddedCode(source))
    dest = directory + ".Embedded/" + identifier + '.inc.h'
    SaveEmbeddedCode(dest, embedded)

Run()
