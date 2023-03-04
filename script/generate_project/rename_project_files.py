import glob
import os

# from https://thispointer.com/python-how-to-get-list-of-files-in-directory-and-sub-directories/
def getListOfFiles(dirName):
    # create a list of file and sub directories 
    # names in the given directory 
    if not (os.path.exists(dirName)):
        print("path: {} is not exist".format(dirName))
        return None

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

def isProjectMarker(name, target_name):
    return name == target_name
    #return name.find(target_name) >= 0

def getPossibleProjectRoot(dirname):
    # just like /.. twice
    root = FilePath.getDirname(dirname)
    root = FilePath.getDirname(root)
    return root

def findProjectRoot(dirname, target_name):
    listOfFile = os.listdir(dirname)
    #print("target_name: " + target_name)
    for entry in listOfFile:
        fullPath = os.path.join(dirname, entry)
        if os.path.isdir(fullPath):
            if(isProjectMarker(entry, target_name)):
                return fullPath

    return ""

def findAndCheckProjectRoot(dirname, src_name):
    root = dirname
    if(root):
        root = getPossibleProjectRoot(dirname)
        root = findProjectRoot(root, src_name)

    if (root == ""):
        print("Error: cannot find project src root!")
        return
    
    print("project src root: ", root)
    return root

def getInputName(msg, default = ""):
    defaultMsg = ""
    if (default):
        defaultMsg = ", no input for default: {} :".format(default)
    name = input(msg + defaultMsg)
    if (name == ""):
        return default
    return name

class ProjectRename:
    def __init__(self):
        current_dir = ""
        src_name    = "src"

    def rename_project_files_to_new_namespace(self):
        print("function 1: rename_project_files_to_new_namespace")
        print(self.current_dir)

        root = findAndCheckProjectRoot(self.current_dir, self.src_name)
        if(root == ""):
            return

        from_name   = getInputName("Enter name to change from:", "projNamespace")
        to_name     = getInputName("Enter name to change: ")

        print("From {} to {}".format(from_name, to_name))
        isRename = input("sure to rename the project files? [y/n]")
        if (isRename == 'y'):
            print("processing...")
            src_files = getListOfFiles(root)
            if (src_files == None):
                return
            src_dirs = fast_scandir(root)
            changeProjectFileNames(to_name, src_files, from_name)
            changeProjectDirnames(to_name, src_dirs, from_name)

    def rename_project_sub_module_files(self):
        print("function 2: rename_project_sub_module_files")
        
        root = findAndCheckProjectRoot(self.current_dir, self.src_name)
        if(root == ""):
            return

        projNamespace           = getInputName("Enter project namespace: ", "projNamespace")
        sub_module_dir          = getInputName("Enter sub-module dir: (relative to {})".format(root))
        from_sub_module_name    = getInputName("Enter sub-module name to change from: ")
        to_sub_module_name      = getInputName("Enter sub-module name to change: ")

        from_name       = projNamespace + "_" + from_sub_module_name
        to_name         = projNamespace + "_" + to_sub_module_name
        sub_module_root = "{}/{}".format(root, sub_module_dir)

        print("sub-module path: {}".format(sub_module_root))
        print("From {} to {}".format(from_name, to_name))
        isRename = input("sure to rename the project files? [y/n]")
        if (isRename == 'y'):
            print("processing...")
            src_files = getListOfFiles(sub_module_root)
            if (src_files == None):
                return
            src_dirs = fast_scandir(sub_module_root)
            changeProjectFileNames(to_name, src_files, from_name)
            changeProjectDirnames(to_name, src_dirs, from_name)

def my_main():
    current_dir = os.path.dirname(os.path.realpath(__file__))
    projRename = ProjectRename()

    projRename.current_dir  = current_dir
    projRename.src_name     = "src"
    print("current_dir: " + current_dir)

    print("function 0: exit")
    print("function 1: rename_project_files_to_new_namespace")
    print("function 2: rename_project_sub_module_files")
    
    function = input("Please enter function [n]: ")
    function = int(function)

    match function:
        case 0: return
        case 1: projRename.rename_project_files_to_new_namespace()
        case 2: projRename.rename_project_sub_module_files()

    print("==========End============")

my_main()
