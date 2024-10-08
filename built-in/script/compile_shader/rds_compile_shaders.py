import math
import os, sys, platform, pathlib
import subprocess
import time
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
    Program             = 0
    BuildConfig         = 1
    RdsRoot             = 2
    ProjectRoot         = 3
    ShaderCompilerPath  = 4

class BColors:
    Header      = '\033[95m'
    OkBlue      = '\033[94m'
    OkCyan      = '\033[96m'
    OkGreen     = '\033[92m'
    Warning     = '\033[93m'
    Fail        = '\033[91m'
    EndC        = '\033[0m'
    Bold        = '\033[1m'
    Underline   = '\033[4m'

    Red         = '\033[31m'

def printRedText(str):
    print("{}{}{}".format(BColors.Red, str, BColors.EndC))

class ShaderCompileDesc:
    #rds_make            = ''
    gnu_make_root               = ''
    mk_build_config             = ''
    mk_gnu_make                 = ''
    mk_rds_shader_compiler_path = ''
    mk_rds_root                 = ''
    mk_project_root             = ''
    mk_shader_file_path         = ''

class CompileShaders:
    
    @staticmethod
    def printProcOutput(errorList, proc, shaderPath, timeoutSec):
        stdout, stderr = proc.communicate(timeout = timeoutSec)
        rtCode = proc.returncode
        hasError = rtCode != 0
        if (hasError):
            errorList.append(shaderPath)
            printRedText("\n\n\n***************** error: {}".format(shaderPath))
            print(stdout.decode())
            printRedText("--- error end --- {} --- --- ---".format(shaderPath))

    @staticmethod
    def compileShaderBatch(errorList, shaderCompileDesc, shader_paths, offset, batch, timeoutSec):
        procs = []
        for i in range(batch):
            iShaderPath = offset + i
            if iShaderPath >= len(shader_paths):
                break
            
            path = shader_paths[offset + i]
            #print("compiling {}".format(path))

            # if (i > 1):
            #     break
            
            mk_shader_file_path = shaderCompileDesc.mk_shader_file_path + path
            
            # print("mk_rds_root:         {}".format(mk_rds_root))
            # print("mk_project_root:     {}".format(mk_project_root))
            # print("shader_file_path:    {}".format(shader_file_path))
            # print("shader path:         {}".format(path))
            
            proc = subprocess.Popen(
                args = [shaderCompileDesc.gnu_make_root
                        , shaderCompileDesc.mk_build_config
                        , shaderCompileDesc.mk_rds_root
                        , shaderCompileDesc.mk_project_root, mk_shader_file_path
                        , shaderCompileDesc.mk_gnu_make
                        , shaderCompileDesc.mk_rds_shader_compiler_path
                        ]
                #, shell = True, stdout = subprocess.PIPE, stderr = subprocess.PIPE
                , stdout = subprocess.PIPE, stderr = subprocess.STDOUT
            )

            procs.append(proc)
            #proc.wait()

        for i, proc in enumerate(procs):
            iShaderPath = offset + i
            shaderPath = shader_paths[iShaderPath]
            #print("\t === Begin Wait {} ===".format(shaderPath))
            stdout = {}
            try:
                # proc.wait(timeout = timeoutSec) # this will deadlock when using subprocess.PIPE ...., see doc.......
                CompileShaders.printProcOutput(errorList, proc, shaderPath, timeoutSec)
                pass
            except:
                proc.kill()
                errorList.append(shaderPath)
                #CompileShaders.printProcOutput(errorList, proc, shaderPath, timeoutSec)
            #print("\t === End ===")
        #print("=== Compile Shaders End ===")

    def main(args):
        #print("Project_root:")
        #print(args[CmdLineArg.Project_root.value])
        #print("args[CmdLineArg.RdsRoot.value]: {}".format(args[CmdLineArg.RdsRoot.value]))

        rds_root        = os.path.abspath(args[CmdLineArg.RdsRoot.value])
        project_root    = os.path.abspath(args[CmdLineArg.ProjectRoot.value])
        gnu_make_root   = ""

        is_rds_root_exist = os.path.exists(rds_root + "/build") and os.path.exists(rds_root + "/CMakeLists.txt") # TODO: need proper check
        if not (is_rds_root_exist):
            str = "in correct rds_root: [{}]".format(rds_root)
            raise Exception(str)

        shader_src_paths   = MyPathLib.getListOfFiles_Ext(project_root, ".shader")
        shader_paths       = MyPathLib.getRelativePath(shader_src_paths, project_root)

        #print(os.path.abspath(project_root))
        #print(shaders_src_path)
        #print(shaders_path)
        
        if (platform.system() == 'Windows'):
            gnu_make_root = rds_root + "/external/gnu_make/windows/make.exe"
        else:
            str = "currentlt platform: {}, only support windows currently".format(platform.system())
            raise Exception(str)
        
        if (not os.path.exists(gnu_make_root)):
            str = "gnu_make_root: [{}] does not exist".format(gnu_make_root)
            raise Exception(str)
        
        # === TODO: no hardcode, load variable name
        imported_path       = "local_temp/imported"
        cmpDesc = ShaderCompileDesc
        cmpDesc.rds_make                    = "make"
        cmpDesc.gnu_make_root               = gnu_make_root
        cmpDesc.mk_build_config             = "BUILD_CONFIG"                + "=" + args[CmdLineArg.BuildConfig.value]
        cmpDesc.mk_gnu_make                 = "GNU_MAKE"                    + "=" + gnu_make_root
        cmpDesc.mk_rds_shader_compiler_path = "RDS_SHADER_COMPILER_PATH"    + "=" + args[CmdLineArg.ShaderCompilerPath.value]
        cmpDesc.mk_rds_root                 = "RDS_ROOT"                    + "=" + rds_root
        cmpDesc.mk_project_root             = "PROJECT_ROOT"                + "=" + project_root
        cmpDesc.mk_shader_file_path         = "SHADER_FILE_PATH"            + "=" + ""
        # ===

        timeoutSec      = 4 # sec
        errorList       = []

        #shader_paths    = ["asset/shader/demo/hello_triangle/hello_triangle.shader"]

        #print("=== Compile Shaders Begin ===")
        #bin_path_base = project_root + "/" + imported_path + "/"
        batchSize = 8
        batchCount = math.ceil(len(shader_paths) / batchSize)
        for i in range(batchCount):
            CompileShaders.compileShaderBatch(errorList, cmpDesc, shader_paths, i * batchSize, batchSize, timeoutSec)

        hasError = len(errorList)
        if (hasError):
            printRedText("\n***\n total {} shaders has error".format(len(errorList)))
            for i, err in enumerate(errorList):
                print(" - {}".format(errorList[i]))

        return hasError
    
returnCode = CompileShaders.main(sys.argv)
sys.exit(returnCode)
