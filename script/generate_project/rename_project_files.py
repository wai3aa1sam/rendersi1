import glob
import os

# from https://thispointer.com/python-how-to-get-list-of-files-in-directory-and-sub-directories/
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
            allFiles = allFiles + getListOfFiles(fullPath)
        else:
            allFiles.append(fullPath)
                
    return allFiles

#from https://stackoverflow.com/questions/18394147/how-to-do-a-recursive-sub-folder-search-and-return-files-in-a-list/59803793#59803793
def fast_scandir(dirname):
    subfolders= [f.path for f in os.scandir(dirname) if f.is_dir()]
    for dirname in list(subfolders):
        subfolders.extend(fast_scandir(dirname))
    return subfolders

class FilePath:
    @staticmethod
    def getDirname(path):
        return os.path.dirname(path)

    @staticmethod
    def getBasename(path):
        return os.path.basename(path)

    @staticmethod
    def getExtension(path):
        return os.path.splitext(path)[1]

def changeProjectFileNames(target_name, src_files, form_name):
    for file in src_files:
        ext = FilePath.getExtension(file)
        if(ext != '.h' and ext != '.cpp'):
            continue
        parent_dir = FilePath.getDirname(file)
        basename = FilePath.getBasename(file)
        new_basename = basename.replace(form_name, target_name)
        new_name = parent_dir + '\\' + new_basename
        os.rename(file, new_name)

def changeProjectDirnames(target_name, src_dirs, form_name):
    for dir in src_dirs:
        basename = FilePath.getBasename(dir)
        if(basename.find(form_name) < 0):
            continue
        parent_dir = FilePath.getDirname(dir)
        new_basename = basename.replace(form_name, target_name)
        new_name = parent_dir + '\\' + new_basename
        os.rename(dir, new_name)

def isProjectName(name, target_name):
    return name.find(target_name) >= 0

def getPossibleProjectRoot(dirname):
    root = FilePath.getDirname(dirname)
    root = FilePath.getDirname(root)
    return root

def findProjectRoot(dirname, target_name):
    listOfFile = os.listdir(dirname)
    #print("target_name: " + target_name)
    for entry in listOfFile:
        fullPath = os.path.join(dirname, entry)
        if os.path.isdir(fullPath):
            if(isProjectName(entry, target_name)):
                return fullPath

    return None
        
def rename_project_files():
    current_dir = os.path.dirname(os.path.realpath(__file__))

    print("current_dir: " + current_dir)
    
    target_name = input("Enter project namespace: ")
    from_name = "projNamespace"
    src_name = "src"

    root = findProjectRoot(current_dir, src_name)
    if(root == None):
        root = getPossibleProjectRoot(current_dir)
        root = findProjectRoot(root, src_name)

    if(root == None):
        print("Error: cannot find project src root!")
        print("==========End============")
        return

    print("project src root: ", root)

    isRename = input("sure to rename the project files? [y/n]")
    if (isRename == 'y'):
        print("Renaming")
        src_files = getListOfFiles(root)
        src_dirs = fast_scandir(root)
        changeProjectFileNames(target_name, src_files, from_name)
        changeProjectDirnames(target_name, src_dirs, from_name)
    print("==========End============")

rename_project_files()
