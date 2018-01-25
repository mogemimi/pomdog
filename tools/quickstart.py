# Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

import sys
import os
import platform
import argparse
import subprocess
import uuid
import shutil


from string import Template
from download_dependencies import DownloadDependencies


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def ConsoleColorText(color, text):
    if platform.system() == 'Windows':
        return text
    return color + text + bcolors.ENDC


def CreateProjectDirectory(path):
    if os.path.exists(path):
        print("Error: {0} is exists".format(path))
        return False, ""
    if (not os.path.exists(path)) and path:
        os.makedirs(path)
        print("{0}: Create a new directory.".format(path))
    return True, path


def ParsingCommandLineAraguments():
    parser = argparse.ArgumentParser(prog='quickstart',
                                     description='Create new project.')
    parser.add_argument('name', default='def', help='project name')
    parser.add_argument('-d', nargs=1, required=True, help='directory path')
    parser.add_argument('-u', default='com.example.QuickStart',
                        nargs=1, help='project url')
    args = parser.parse_args()
    return args


def CopySourceFiles(sourceRoot, destRoot, filePaths):
    for path in filePaths:
        sourcePath = os.path.join(sourceRoot, path)
        destPath = os.path.join(destRoot, path)

        if os.path.isdir(sourcePath):
            shutil.copytree(sourcePath, destPath)
        elif os.path.exists(sourcePath):
            shutil.copy(sourcePath, destPath)
        else:
            print("Error: Cannot find file {0}".format(sourcePath))


def CopyTemplates(templateDirectory, projectRoot):
    templateFiles = [
        "Content",
        "Platform.Cocoa",
        "Platform.Win32",
        "Platform.X11",
        "Source",
        ".gitignore",
        "common.gypi",
        "QuickStart.gyp",
        "README.md",
    ]
    CopySourceFiles(templateDirectory, projectRoot, templateFiles)


def CopyFrameworkFiles(pomdogPath, projectRoot):
    sourceFiles = [
        "build",
        "include",
        "src",
        "LICENSE.md",
        "README.md",
        ".gitignore",
    ]
    thirdPartyFiles = [
        "glew",
        "zlib",
        "libpng",
    ]
    minimumGypFiles = [
        "pylib",
        "gyp",
        "gyp.bat",
        "gyp_main.py",
        "setup.py",
    ]
    CopySourceFiles(
        pomdogPath,
        os.path.join(projectRoot, "ThirdParty/pomdog"),
        sourceFiles)
    CopySourceFiles(
        os.path.join(pomdogPath, "third-party"),
        os.path.join(projectRoot, "ThirdParty/pomdog/third-party"),
        thirdPartyFiles)
    CopySourceFiles(
        os.path.join(pomdogPath, "tools/gyp"),
        os.path.join(projectRoot, "Tools/gyp"),
        minimumGypFiles)


def RenameSourceContent(project_root, identifier, source):
    path = os.path.join(project_root, source)

    if not os.path.exists(path):
        print("Error: Cannot find file {0}".format(path))
        return

    f = open(path, 'r')
    content = f.read()
    f.close()

    f = open(path, 'w')
    content = content.replace('QuickStart'.encode('utf_8'), identifier)
    content = content.replace('QUICKSTART'.encode('utf_8'), identifier.upper())
    f.write(content)
    f.close()


def RenameContentByUrl(project_root, url, source):
    path = os.path.join(project_root, source)

    if not os.path.exists(path):
        print('Error: Cannot find file {0}'.format(path))
        return

    f = open(path, 'r')
    content = f.read()
    f.close()

    f = open(path, 'w')
    content = content.replace('com.example.QuickStart'.encode('utf_8'), url)
    f.write(content)
    f.close()


def RenameFilename(project_root, identifier, source):
    path = os.path.join(project_root, source)

    if not os.path.exists(path):
        print("Error: Cannot find file {0}".format(path))
        return

    dest = os.path.join(project_root, source.replace('QuickStart', identifier))
    if source != dest:
        os.rename(path, dest)


def NormalizePath(path):
    return os.path.normpath(path)


def CreateNewProject(config):
    identifier = os.path.basename(config['name'])
    project_root = NormalizePath(os.path.join(config['path'], identifier))
    CreateProjectDirectory(project_root)
    project_url = config['url']

    pomdogPath = NormalizePath(
        os.path.join(os.path.dirname(__file__), ".."))
    templates_directory = NormalizePath(
        os.path.join(pomdogPath, 'examples/QuickStart'))

    if not DownloadDependencies(pomdogPath):
        print("Error: Can't download project dependencies.")
        return

    CopyTemplates(templates_directory, project_root)
    CopyFrameworkFiles(pomdogPath, project_root)
    RenameContentByUrl(project_root, project_url, 'Platform.Cocoa/Info.plist')
    RenameSourceContent(project_root, identifier, 'README.md')
    RenameSourceContent(project_root, identifier, 'QuickStart.gyp')
    RenameSourceContent(project_root, identifier, 'Source/QuickStartGame.cpp')
    RenameSourceContent(project_root, identifier, 'Source/QuickStartGame.hpp')
    RenameSourceContent(project_root, identifier, 'Platform.Cocoa/AppDelegate.mm')
    RenameSourceContent(project_root, identifier, 'Platform.Cocoa/GameViewController.mm')
    RenameSourceContent(project_root, identifier, 'Platform.Cocoa/Info.plist')
    RenameSourceContent(project_root, identifier, 'Platform.Cocoa/Base.lproj/MainMenu.xib')
    RenameSourceContent(project_root, identifier, 'Platform.Win32/main.cpp')
    RenameSourceContent(project_root, identifier, 'Platform.X11/main.cpp')
    RenameFilename(project_root, identifier, 'QuickStart.gyp')
    RenameFilename(project_root, identifier, 'Source/QuickStartGame.cpp')
    RenameFilename(project_root, identifier, 'Source/QuickStartGame.hpp')

    print(ConsoleColorText(bcolors.OKGREEN, 'Create a new project at \'')
        + ConsoleColorText(bcolors.BOLD, project_root)
        + ConsoleColorText(bcolors.OKGREEN, '\'.'))


def ReadInput(prompt):
    #if six.PY3:
    #    return input(prompt)
    return raw_input(prompt)


def Ask(question, default=None):
    result = ''
    question = ConsoleColorText(bcolors.OKGREEN, question)
    while True:
        if default:
            result = ReadInput('> {0} [{1}] '.format(question, default))
        else:
            result = ReadInput('> {0} '.format(question))

        result = result.strip()

        if len(result) <= 0:
            if default:
                result = default
                break
            else:
                print(ConsoleColorText(bcolors.WARNING, 'You must enter something'))
        else:
            break
    return result


def main():
    config = {
        'path': '.',
        'name': 'QuickStart',
        'url': 'com.example.QuickStart',
        'author': '',
    }

    print('Configure your new project\n')

    config['path'] = Ask('Where do you want to create your new gamedev project?',
        config['path'])

    config['name'] = Ask('What is your project name? (e.g. MyGame)')

    config['name'] = os.path.basename(config['name'])
    config['url'] = config['url'].replace('QuickStart', config['name'])

    config['url'] = Ask('What is your project URL?', config['url'])

    CreateNewProject(config)
    print(ConsoleColorText(bcolors.OKGREEN, 'Done.'))


if __name__ == '__main__':
    main()
