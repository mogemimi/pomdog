# Copyright (c) 2013-2015 mogemimi.
# Distributed under the MIT license. See LICENSE.md file for details.

import sys
import os
import argparse
import subprocess
import uuid
import shutil


from string import Template


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


def CopyTemplates(template_directory, project_root):
    templates_list = [
        "Content",
        "Platform.Cocoa",
        "Platform.Win32",
        "Source",
        ".gitignore",
        "common.gypi",
        "QuickStart.gyp",
        "README.md",
    ]

    for path in templates_list:
        source_path = os.path.join(template_directory, path)
        dest_path = os.path.join(project_root, path)

        if os.path.isdir(source_path):
            shutil.copytree(source_path, dest_path)
        elif os.path.exists(source_path):
            shutil.copy(source_path, dest_path)
        else:
            print("Error: Cannot find file {0}".format(source_path))


def CopyFrameworkFiles(framework_root, project_root):
    framework_list = [
        "build",
        "include",
        "src",
        "third-party/glew",
        "third-party/zlib",
        "third-party/libpng",
        "LICENSE.md",
        "README.md",
        ".gitignore",
    ]

    for path in framework_list:
        source_path = os.path.join(framework_root, path)
        dest_path = os.path.join(project_root, "Pomdog/" + path)

        if os.path.isdir(source_path):
            shutil.copytree(source_path, dest_path)
        elif os.path.exists(source_path):
            shutil.copy(source_path, dest_path)
        else:
            print("Error: Cannot find file {0}".format(source_path))


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


def CreateNewProject(config):
    identifier = os.path.basename(config['name'])
    project_root = os.path.join(config['path'], identifier)
    CreateProjectDirectory(project_root)
    project_url = config['url']

    framework_root = os.path.join(os.path.dirname(__file__), "..")
    templates_directory = os.path.join(framework_root,
      'examples/QuickStart')

    CopyTemplates(templates_directory, project_root)
    CopyFrameworkFiles(framework_root, project_root)
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
    RenameFilename(project_root, identifier, 'QuickStart.gyp')
    RenameFilename(project_root, identifier, 'Source/QuickStartGame.cpp')
    RenameFilename(project_root, identifier, 'Source/QuickStartGame.hpp')


def ReadInput(prompt):
    #if six.PY3:
    #    return input(prompt)
    return raw_input(prompt)


def Ask(question, default=None):
    result = ''
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
                print('You must enter something')
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

    config['name'] = Ask('What is your project name? (ex. MyGame)')

    config['name'] = os.path.basename(config['name'])
    config['url'] = config['url'].replace('QuickStart', config['name'])

    config['url'] = Ask('What is your project URL?', config['url'])

    CreateNewProject(config)
    print('Done.')


if __name__ == '__main__':
    main()
