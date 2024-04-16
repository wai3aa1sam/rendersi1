import glob
import os
import sys

def relativePath(src, relativeTo):
    res = []
    for dirname in list(src):
        if os.path.isdir(dirname): 
            continue
        dir = os.path.relpath(dirname, relativeTo)
        res.append(dir)
        
    return res

class CmdLineInterface:
    @staticmethod
    def retToMakeFile(src):
        for idx, elem in enumerate(src):
            print(elem)

    def main(args):
        if (len(args) == 1):
            return

        funcIdx = 1
        startIdx = funcIdx + 1
        func = args[funcIdx]
        argCount = len(args) - 2

        if (func == "_relat_dirs"):
            res = relativePath(sys.argv[startIdx + 1: argCount], args[startIdx])
            CmdLineInterface.retToMakeFile(res)

CmdLineInterface.main(sys.argv)
