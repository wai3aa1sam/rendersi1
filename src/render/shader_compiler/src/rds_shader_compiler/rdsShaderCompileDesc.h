#pragma once

#include "rds_shader_compiler/common/rds_shader_compiler_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderCompileInfo-Decl ---
#endif // 0
#if 1

struct ShaderIncludes
{
public:
	static bool s_resolve(ShaderIncludes& include, StrView shader_path, StrView inc_path)
	{
		auto& files = include._files;
		auto& back = files.emplace_back();
		auto is_valid = s_resolve(back, include, shader_path, inc_path);
		if (!is_valid)
		{
			files.pop_back();
		}
		return is_valid;
	}

	template<class STRING>
	static bool s_resolve(STRING& out, const ShaderIncludes& include, StrView shader_path, StrView inc_path)
	{
		out.clear();

		auto& dirs  = include._dirs;

		TempString tmp;
		fmtTo(tmp, "{}/{}", shader_path, inc_path);

		bool is_path_valid = Path::isFile(tmp);
		if (!is_path_valid)
		{
			for (auto& dir : dirs)
			{
				tmp.clear();
				fmtTo(tmp, "{}/{}", dir, inc_path);
				is_path_valid = Path::isFile(tmp);
				if (is_path_valid)
					break;
			}
		}

		if (is_path_valid)
			out.assign(tmp.c_str());

		return is_path_valid;
	}

public:
	void reserve()				{ _files.reserve(fileCount()); }
	void addFileCount()			{ _fileCount++; };

	String& emplaceBackDir()	{ return _dirs.emplace_back(); }

	String& emplaceBackFile()	{ return _files.emplace_back(); }
	void	popBackFile()		{ return _files.pop_back(); }

	void clearFiles()			{ _files.clear(); }

	bool resolve(StrView inc_path, StrView inputFilename)
	{
		auto& include = *this;
		return s_resolve(include, inputFilename, inc_path);
	}

public:
	int  fileCount()	const	{ return _fileCount; }

			String& lastFile()			{ return _files.back(); }
	const	String& lastFile()	const	{ return _files.back(); }


	Span<String>		files()			{ return _files; }
	Span<const String>	files() const	{ return _files; }

	Span<String>		dirs()			{ return _dirs; }
	Span<const String>	dirs() const	{ return _dirs; }

private:
	int					_fileCount = 0;
	Vector<String>		_files;
	Vector<String, 4>	_dirs;
};

struct ShaderCompileInfo
{
	String cwd;

	String rdsRoot;
	String compilerPath;

	String shaderCompiler;

	String compilerMakePath;
	String projectMakePath;

	String outputPath;

	String builtInPath;

	String buildConfig;
};

#endif

#if 0
#pragma mark --- rdsShaderCompileDesc-Decl ---
#endif // 0
#if 1

class ShaderCompileDesc : public NonCopyable
{
public:
	static constexpr int s_kInvalid = -1;

public:
	String language;
	String inputFilename;
	String outputFilename;
	String permutName;

	String profile;
	String entry;

	ShaderStageFlag stage;

	ShaderCompileInfo	compileInfo;
	ShaderInfo			shaderInfo;

	Vector<ShaderMarco, 12>	marcos;
	ShaderIncludes			includes;

	ShaderCompileOption compileOption;

	bool   isGNUMakeCompile : 1;
	bool   isGenerateMake	: 1;
	
	int globalBinding	= s_kInvalid;
	int globalSet		= s_kInvalid;

public:
	bool isValidGlobalBinding() const { return globalBinding != s_kInvalid && globalSet != s_kInvalid; }

public:
	template<class STR>
	void getOutputPathTo(STR& o) const
	{
		auto& ps = *ProjectSetting::instance();
		fmtTo(o, "{}/{}/makeFile", ps.importedShaderPath(), inputFilename);
	}

	template<class STR>
	void getBinFilepath(STR& o, StrView dstDir, ShaderStageFlag stage, RenderApiType apiType) const
	{
		auto& binFilepath	= o;
		auto& output		= outputFilename;

		if (!dstDir.is_empty())
		{
			ShaderCompileUtil::getBinFilepathTo(binFilepath, dstDir, stage, apiType);
		}
		else if (!output.is_empty())
		{
			binFilepath = output;
		}
		throwIf(binFilepath.is_empty(), "invalid output name / dstDir");
	}

private:

};


#endif

}