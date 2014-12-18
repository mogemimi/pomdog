#
#  Copyright (C) 2013-2014 mogemimi.
#  Distributed under the MIT License. See LICENSE.md or
#  http://enginetrouble.net/pomdog/license for details.
#


import sys
import os
import argparse
import subprocess
import uuid
from string import Template


def ParsingCommandLineAraguments():
    parser = argparse.ArgumentParser(prog='source2utf8',
                                     description='Convert source files to UTF-8 with signature.')
    parser.add_argument('directory', default='def')
    parser.add_argument('-v', '--version', action='version', version='%(prog)s version 0.1.0')
    args = parser.parse_args()
    return args

def IsSourceFile(path):
    root, ext = os.path.splitext(path)
    return (ext == '.cpp'
      or ext == '.hpp'
      or ext == '.mm'
      or ext == '.hh'
      or ext == '.m'
      or ext == '.h'
      or ext == '.c'
      or ext == '.inc')

def ReadContent(path):
    f = open(path, 'rb')
    str = f.read()
    f.close()
    return str

def WriteContent(path, bom, content):
    f = open(path, 'wb')
    f.write(bom.encode('utf-8'))
    f.write(content.encode('utf-8'))
    f.close()

def ConvertSourceFile(path):
    content = unicode(ReadContent(path), 'utf-8')
    bom = u'\ufeff'
    if content.startswith(bom):
        return
    print("=> {0}".format(path))
    WriteContent(path, bom, content.replace('\r\n', '\n'))

def Run():
    args = ParsingCommandLineAraguments()

    #print("args = {0}".format(vars(args))) # for Debug

    directory = args.directory

    if not os.path.isdir(directory):
        print("error: Cannot find directory, {0}".format(directory))
        return

    for root, dirs, files in os.walk(directory):
        for file in files:
          path = os.path.join(root, file)
          if not os.path.isfile(path):
              continue
          if not IsSourceFile(path):
              continue
          #print(path)
          ConvertSourceFile(path)

Run()
