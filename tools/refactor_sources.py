# Copyright (c) 2013-2015 mogemimi.
# Distributed under the MIT license. See LICENSE.md file for details.


import sys
import os
import argparse
import subprocess
import uuid

from string import Template


def ParsingCommandLineAraguments():
    parser = argparse.ArgumentParser(
        prog='refactor_sources',
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


def WriteContent(path, content):
    f = open(path, 'wb')
    f.write(content.encode('utf-8'))
    f.close()


def RemoveUnnecessaryWhitespace(content):
    result = unicode("", 'utf-8')
    for line in content.split('\n'):
        result += line.rstrip(' ').rstrip('\t')
        result += '\n'
    result = result.rstrip('\n') + '\n'
    return result


def ReplaceHardTabsWithWhiteSpaces(content):
    result = unicode("", 'utf-8')
    for line in content.split('\n'):
        result += line.replace('\t', '    ')
        result += '\n'
    result = result.rstrip('\n') + '\n'
    return result


def RemoveOldStyleCode(content):
    return content.replace("""

#if _MSC_VER > 1000
#pragma once
#endif""", '')


def ConvertSourceFile(path):
    isChanged = False
    statusRemoveBom = "---"
    content = unicode(ReadContent(path), 'utf-8')
    bom = u'\ufeff'
    if content.startswith(bom):
        content = content.replace(bom, '')
        statusRemoveBom = "bom"
        isChanged = True

    statusUnusedSpaces = "-----"
    removed = RemoveUnnecessaryWhitespace(content.replace('\r\n', '\n'))
    if content != removed:
        content = removed
        statusUnusedSpaces = "space"
        isChanged = True

    statusReplaceIndentWithSpaces = "------"
    replaced = ReplaceHardTabsWithWhiteSpaces(content)
    if content != replaced:
        content = replaced
        statusReplaceIndentWithSpaces = "indent"
        isChanged = True

    statusRemoveOldStyleCode = "---"
    replaced = RemoveOldStyleCode(content)
    if content != replaced:
        content = replaced
        statusRemoveOldStyleCode = "old"
        isChanged = True

    if isChanged:
        print("=> ({0} {1} {2} {3}) {4}".format(
            statusRemoveBom,
            statusUnusedSpaces,
            statusReplaceIndentWithSpaces,
            statusRemoveOldStyleCode,
            path))
        WriteContent(path, content)


def main():
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

if __name__ == '__main__':
    main()
