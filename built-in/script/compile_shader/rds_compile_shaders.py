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

class CompileShaders:
    
    @staticmethod
    def printProcOutput(proc, timeoutSec):
        stdout, stderr = proc.communicate(timeout = timeoutSec)
        print(stdout.decode())
        #print(stderr.decode())

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

        shader_src_paths   = MyPathLib.getListOfFiles_Ext(project_root, ".shader")
        shader_paths       = MyPathLib.getRelativePath(shader_src_paths, project_root)

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

        procs           = []
        timeoutSec      = 10 # sec

        #shader_paths    = ["asset/shader/demo/hello_triangle/hello_triangle.shader"]

        print("=== Compile Shaders Begin ===")
        #bin_path_base = project_root + "/" + imported_path + "/"
        for i, path in enumerate(shader_paths):

            # if (i > 1):
            #     break
            
            shader_file_path = mk_shader_file_path + path
            
            # print("mk_rds_root:         {}".format(mk_rds_root))
            # print("mk_project_root:     {}".format(mk_project_root))
            # print("shader_file_path:    {}".format(shader_file_path))
            # print("shader path:         {}".format(path))
            
            proc = subprocess.Popen(
                args = [rds_make, mk_rds_root, mk_project_root, shader_file_path, ]
                #, shell = True, stdout = subprocess.PIPE, stderr = subprocess.PIPE
                , stdout = subprocess.PIPE, stderr = subprocess.STDOUT
            )

            procs.append(proc)
            #proc.wait()

        for i, proc in enumerate(procs):
            print("\t === Begin Wait {} ===".format(shader_paths[i]))
            try:
                # proc.wait(timeout = timeoutSec) # this will deadlock when using subprocess.PIPE ...., see doc.......
                CompileShaders.printProcOutput(proc, timeoutSec)
                pass
            except:
                proc.kill()
                CompileShaders.printProcOutput(proc, timeoutSec)
            print("\t === End ===")

        print("=== Compile Shaders End ===")
            
        return
    
        

CompileShaders.main(sys.argv)