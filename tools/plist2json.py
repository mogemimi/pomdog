#
#  Copyright (C) 2013-2014 mogemimi.
#
#  Distributed under the MIT License.
#  See accompanying file LICENSE.md or copy at
#  http://enginetrouble.net/pomdog/LICENSE.md for details.
#


import sys
import os
import argparse
import subprocess
import shutil

def ParsingCommandLineAraguments():
    parser = argparse.ArgumentParser(prog='plist2json',
                                     description='Convert plist to json.')
    parser.add_argument('name', default='def')
    args = parser.parse_args()
    return args

def CopyPlistFile(name):
    root, ext = os.path.splitext(name)
    destination = root + ".json"
    shutil.copyfile(name, destination)
    return destination

def ConvertPlistToJson(name):
    process = subprocess.Popen(["plutil", "-convert", "json", name])

def Run():
    args = ParsingCommandLineAraguments()

    #print("args = {0}".format(vars(args))) # for Debug
    destination = CopyPlistFile(args.name)
    ConvertPlistToJson(destination)

Run()
