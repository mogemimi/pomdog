# Copyright (c) 2013-2015 mogemimi.
# Distributed under the MIT license. See LICENSE.md file for details.

# Compile HLSL shader from source file:
# >>> python tools/hlsl2embedded.py \
# >>>   experimental/Pomdog.Experimental/Graphics/Shaders/HLSL/SpriteBatch_VS.hlsl \
# >>>   -e SpriteBatchVS -p vs_4_0
# compilation object save succeeded; see experimental\Pomdog.Experimental\Gra
#   phics\Shaders\HLSL.Embedded\SpriteBatch_VS.fxo
# Create new file: experimental/Pomdog.Experimental/Graphics/Shaders/HLSL.Emb
#   edded/SpriteBatch_VS.inc.hpp


import sys
import os
import argparse
import string


def ReadCompiledShader(path):
  f = open(path, 'rb')
  content = f.read()
  f.close()
  return content

def SaveEmbeddedCode(path, content):
  f = open(path, 'w')
  bom = u'\ufeff'
  f.write(bom.encode('utf-8'))
  f.write(content)
  f.close()
  print "Create new file: " + path

def BinaryToAsciiCodeArray(binary):
  content = u''
  column = 0
  elementsPerLine = 6

  for index in range(len(binary)):
    content += str('{0:3d}').format(ord(binary[index]))
    content += ','
    column += 1
    if column >= elementsPerLine:
      content += '\n'
      column = 0
    else:
      content += ' '

  return content.rstrip('\n').rstrip(' ').rstrip(',')

def Compile(sourcePath, entrypoint, shaderProfile):
  splittedPath, ext = os.path.splitext(sourcePath)
  identifier = os.path.basename(splittedPath)
  directory = os.path.dirname(sourcePath)

  destDirectory = (directory + '.Embedded/')
  objectPath = os.path.join(destDirectory, (identifier + '.fxo'))
  destPath = os.path.join(destDirectory, (identifier + '.inc.hpp'))

  is64bits = sys.maxsize > 2**32
  compilerExeFile = u'C:/Program Files (x86)/Windows Kits/8.1/bin/{0}/fxc.exe'.format('x86' if is64bits else 'x64')

  if not os.path.exists(compilerExeFile):
    print(u'Error: Cannot find HLSL compiler "{0}"').format(compilerExeFile)
    return False

  cmd = u'"{0}" /nologo /T {1} /E {2} /Fo {3}  {4}'.format(
    compilerExeFile, shaderProfile, entrypoint, objectPath, sourcePath)

  errorCode = os.system(cmd)

  if errorCode != 0:
    return False

  if not os.path.exists(objectPath):
    print(u'Error: Cannot find object file "{0}"'.format(objectPath))
    return False

  compiledShader = ReadCompiledShader(objectPath)

  content = u'const uint8_t BuiltinHLSL_{0}[] = '.format(identifier)
  content += u'{\n'
  content += BinaryToAsciiCodeArray(compiledShader)
  content += u'\n};\n'

  SaveEmbeddedCode(destPath, content)

  os.remove(objectPath)

  return True

def ParsingCommandLineAraguments():
    parser = argparse.ArgumentParser(description='Compile HLSL shader file to embedded C++ code')
    parser.add_argument('sourcePath', default='def')
    parser.add_argument('-e', '--entrypoint', dest='entrypoint', required=True)
    parser.add_argument('-p', '--profile', dest='profile', required=True)
    parser.add_argument('-v', '--version', action='version', version='%(prog)s version 0.1.0')
    args = parser.parse_args()
    return args

def Run():
  args = ParsingCommandLineAraguments()

  succeeded = Compile(args.sourcePath, args.entrypoint, args.profile)
  if succeeded:
    print '\nCompile is successful (beer)'

Run()
