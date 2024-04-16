import os, pathlib
import importlib

class PathUtil:
    @staticmethod 
    def basename(path): return PathUtil_Impl.basename(path)

    @staticmethod
    def getListOfFiles(dirName): return PathUtil_Impl.getListOfFiles(dirName)

    @staticmethod
    def getListOfFiles_Ext(dirName, extension): return PathUtil_Impl.getListOfFiles_Ext(dirName, extension)
    
    @staticmethod
    def getRelativePath(src, relativeTo): return PathUtil_Impl.getRelativePath(src, relativeTo)

    @staticmethod
    def fastScandir(dirname): return PathUtil_Impl.fastScandir(dirname)

class PathUtil_Impl:

    @staticmethod
    def basename(path):
        return pathlib.Path(path).suffix

    # from https://thispointer.com/python-how-to-get-list-of-files-in-directory-and-sub-directories/
    @staticmethod
    def getListOfFiles(dirName):
        # create a list of file and sub directories 
        # names in the given directory 
        listOfFile = os.listdir(dirName)
        allFiles = list()
        # Iterate over all the entries
        for entry in listOfFile:
            # Create full path
            fullPath = os.path.join(dirName, entry)
            # If entry is a directory then get the list of files in this directory 
            if os.path.isdir(fullPath):
                allFiles = allFiles + PathUtil_Impl.getListOfFiles(fullPath)
            else:
                allFiles.append(fullPath)
                    
        return allFiles

    @staticmethod
    def getListOfFiles_Ext(dirName, extension):
        # create a list of file and sub directories 
        # names in the given directory 
        listOfFile = os.listdir(dirName)
        allFiles = list()
        # Iterate over all the entries
        for entry in listOfFile:
            # Create full path
            fullPath = os.path.join(dirName, entry)
            # If entry is a directory then get the list of files in this directory 
            if os.path.isdir(fullPath):
                allFiles = allFiles + PathUtil_Impl.getListOfFiles_Ext(fullPath, extension)
            else:
                if (PathUtil_Impl.basename(fullPath) != extension):
                    continue
                allFiles.append(fullPath)
                    
        return allFiles

    @staticmethod
    def getRelativePath(src, relativeTo):
        res = []
        for dirname in list(src):
            if os.path.isdir(dirname): 
                continue
            dir = os.path.relpath(dirname, relativeTo)
            res.append(dir)
            
        return res

    #from https://stackoverflow.com/questions/18394147/how-to-do-a-recursive-sub-folder-search-and-return-files-in-a-list/59803793#59803793
    @staticmethod
    def fastScandir(dirname):
        subfolders= [f.path for f in os.scandir(dirname) if f.is_dir()]
        for dirname in list(subfolders):
            subfolders.extend(PathUtil_Impl.fastScandir(dirname))
        return subfolders


    @staticmethod
    def basename(path):
        return pathlib.Path(path).suffix

    # from https://thispointer.com/python-how-to-get-list-of-files-in-directory-and-sub-directories/
    @staticmethod
    def getListOfFiles(dirName):
        # create a list of file and sub directories 
        # names in the given directory 
        listOfFile = os.listdir(dirName)
        allFiles = list()
        # Iterate over all the entries
        for entry in listOfFile:
            # Create full path
            fullPath = os.path.join(dirName, entry)
            # If entry is a directory then get the list of files in this directory 
            if os.path.isdir(fullPath):
                allFiles = allFiles + PathUtil_Impl.getListOfFiles(fullPath)
            else:
                allFiles.append(fullPath)
                    
        return allFiles

    @staticmethod
    def getListOfFiles_Ext(dirName, extension):
        # create a list of file and sub directories 
        # names in the given directory 
        listOfFile = os.listdir(dirName)
        allFiles = list()
        # Iterate over all the entries
        for entry in listOfFile:
            # Create full path
            fullPath = os.path.join(dirName, entry)
            # If entry is a directory then get the list of files in this directory 
            if os.path.isdir(fullPath):
                allFiles = allFiles + PathUtil_Impl.getListOfFiles_Ext(fullPath, extension)
            else:
                if (PathUtil_Impl.basename(fullPath) != extension):
                    continue
                allFiles.append(fullPath)
                    
        return allFiles

    @staticmethod
    def getRelativePath(src, relativeTo):
        res = []
        for dirname in list(src):
            if os.path.isdir(dirname): 
                continue
            dir = os.path.relpath(dirname, relativeTo)
            res.append(dir)
            
        return res

    #from https://stackoverflow.com/questions/18394147/how-to-do-a-recursive-sub-folder-search-and-return-files-in-a-list/59803793#59803793
    @staticmethod
    def fastScandir(dirname):
        subfolders= [f.path for f in os.scandir(dirname) if f.is_dir()]
        for dirname in list(subfolders):
            subfolders.extend(PathUtil_Impl.fastScandir(dirname))
        return subfolders

# moduleName = input('rdsPath')
# importlib.import_module(moduleName)