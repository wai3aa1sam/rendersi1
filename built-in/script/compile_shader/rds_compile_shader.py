import os, sys, pathlib
import subprocess
from enum import Enum

class MyPathLib:
    @staticmethod 
    def basename(path): return MyPathLib_Impl.basename(path)

    @staticmethod
    def getListOfFiles(dirName): return MyPathLib_Impl.getListOfFiles(dirName)

    @staticmethod
    def getListOfFiles_Ext(dirName, extension): return MyPathLib_Impl.getListOfFiles_Ext(dirName, extension)
    
    @staticmethod
    def getRelativePath(src, relativeTo): return MyPathLib_Impl.getRelativePath(src, relativeTo)

    @staticmethod
    def fastScandir(dirname): return MyPathLib_Impl.fastScandir(dirname)

class MyPathLib_Impl:

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
                allFiles = allFiles + MyPathLib_Impl.getListOfFiles(fullPath)
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
                allFiles = allFiles + MyPathLib_Impl.getListOfFiles_Ext(fullPath, extension)
            else:
                if (MyPathLib_Impl.basename(fullPath) != extension):
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
            subfolders.extend(MyPathLib_Impl.fastScandir(dirname))
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
                allFiles = allFiles + MyPathLib_Impl.getListOfFiles(fullPath)
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
                allFiles = allFiles + MyPathLib_Impl.getListOfFiles_Ext(fullPath, extension)
            else:
                if (MyPathLib_Impl.basename(fullPath) != extension):
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
            subfolders.extend(MyPathLib_Impl.fastScandir(dirname))
        return subfolders

class CmdLineArg(Enum):
    Program = 0
    Project_root = 1

class CompileShader:
    def main(args):
        cur_dir = os.getcwd()

        #print("Project_root:")
        #print(args[CmdLineArg.Project_root.value])

        rds_root        = os.path.abspath(cur_dir + "/../../..")
        project_root    = os.path.dirname(args[CmdLineArg.Project_root.value])

        is_rds_root_exist = os.path.exists(rds_root + "/build") and os.path.exists(rds_root + "/CMakeLists.txt") # TODO: need proper check
        if not (is_rds_root_exist):
            raise Exception("in correct rds_root")
            return

        shaders_src_path    = MyPathLib.getListOfFiles_Ext(project_root, ".shader")
        shaders_path        = MyPathLib.getRelativePath(shaders_src_path, project_root)

        #print(os.path.abspath(project_root))
        #print(shaders_src_path)
        #print(shaders_path)

        # === TODO: no hardcode, load variable name
        rds_make            = "make"
        imported_path       = "local_temp/imported"
        mk_rds_root         = "RDS_ROOT"            + "=" + rds_root
        mk_project_root     = "PROJECT_ROOT"        + "=" + project_root
        mk_shader_file_path = "SHADER_FILE_PATH"    + "=" + ""
        # ===
        
        print("====")
        #bin_path_base = project_root + "/" + imported_path + "/"
        for idx, path in enumerate(shaders_path):
            
            #bin_path = bin_path_base + path
            #print("bin path:", bin_path)
            #print("shader path:", path)

            shader_file_path = mk_shader_file_path + path

            if True:    # faster, since no wait, but need to handle the output for atomic stdout
                res = subprocess.Popen(
                [rds_make, mk_rds_root, mk_project_root, shader_file_path, ],)
            else:
                res = subprocess.run(
                [rds_make, mk_rds_root, mk_project_root, shader_file_path, ], 
                cwd = cur_dir, capture_output = True, shell = True)

                print(res.returncode)
                print(res.stdout.decode())
                print(res.stderr.decode())

        return
        

CompileShader.main(sys.argv)
