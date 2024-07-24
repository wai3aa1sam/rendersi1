#include "rds_render_api_layer-pch.h"
#include "rdsShaderCompileRequest.h"
#include "rdsShader.h"

#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

class CmdLineArgs
{
public:
	static constexpr char s_delimiter = ' ';

public:
	template<class... ARGS>
	void addArg(const char* fmt, ARGS&&... args)
	{
		fmtTo(_buf, fmt, rds::forward<ARGS>(args)...);
		_buf += s_delimiter;
	}

	void addStr(StrView v)
	{
		_buf.append(v.data());
		_buf += s_delimiter;
	}


	void splitArgsTo(Vector<String> oArgs)
	{
		oArgs.reserve(64);

		StrView textData = _buf.view();
		auto pair = StrUtil::splitByChar(textData, s_delimiter);
		while (pair.first.size() != 0)
		{
			oArgs.emplace_back(pair.first);
			pair = StrUtil::splitByChar(pair.second, s_delimiter);
		}
	}

	const char* stream() { return _buf.c_str(); }

private:
	TempString _buf;
};

#if 0
#pragma mark --- rdsShaderCompileRequest-Impl ---
#endif // 0
#if 1

bool 
ShaderCompileRequest::hotReload(Renderer* renderer, JobSystem* jobSystem, const ProjectSetting* projectSetting)
{
	const auto& ps = *projectSetting;

	if (!Path::isFile(ps.shaderRecompileListPath()))
		return false;

	auto* rdDev = renderer->rdDev();
	rdDev->waitIdle();
	//jobSystem->waitIdle();
	RDS_TODO("jobSystem->waitIdle()");

	Vector<u8> data;
	{
		FileStream fs;
		fs.openWrite(ps.shaderRecompileListPath(), false);
		auto lock = fs.scopedLock();
		data.resize(fs.fileSize());
		fs.readBytes(data);

		fs.setFileSize(0);
	}

	RDS_TODO("make a class for read and write");
	auto textData = makeStrView(data.byteSpan());
	auto pair = StrUtil::splitByChar(textData, "\n");
	while (pair.first.size() != 0)
	{
		#if 0
		auto filenameFullpath = pair.first;
		TempString filename;
		Path::relativeTo(filename, filenameFullpath, ps.projectRoot());
		#else
		auto filename = pair.first;
		#endif // 0

		_hotReloadShader(filename, rdDev, ps);
		pair = StrUtil::splitByChar(pair.second, "\n");
	}

	processPermutationRequests(rdDev, ps);

	//throwIf(true, "");

	bool hasCompileShaders = textData.size() > 0;
	return hasCompileShaders;
}

bool 
ShaderCompileRequest::generateCompileShaderScript(StrView rdsRoot, StrView projectRoot, StrView buildConfig, ProjectSetting* projectSetting)
{
	RDS_TODO("class FileSerializer");

	bool hasChanges = false;
	#if RDS_OS_WINDOWS
	{
		TempString outpath;
		fmtTo(outpath, "{}/{}", projectRoot, projectSetting->compileShaderScriptPath());

		String data;
		data.reserve(4096);

		auto appendLine = [](String& out, StrView data) { out += data; out += "\n"; };

		appendLine(data, "@REM #change current directory to this file");
		appendLine(data, "@%~d0");
		appendLine(data, "@cd %~dp0");

		appendLine(data, "");

		fmtTo(data,			"set BUILD_CONFIG={}\n",			buildConfig);
		fmtTo(data,			"set PROJECT_ROOT={}\n",			projectRoot);
		fmtTo(data,			"set RDS_ROOT={}\n",				rdsRoot);
		fmtTo(data,			"set SHADER_COMPILER_PATH={}\n",	projectSetting->shaderCompilerPath());
		appendLine(data,	"set COMPILE_SHADER_PATH=%RDS_ROOT%\\built-in\\script\\compile_shader");

		appendLine(data, "");

		appendLine(data, "pushd \"%COMPILE_SHADER_PATH%\"");
		fmtTo(data,	"python rds_compile_shaders.py \"%BUILD_CONFIG%\" \"%RDS_ROOT%\" \"%PROJECT_ROOT%\" \"%SHADER_COMPILER_PATH%\" \n");
		appendLine(data, "set exitCode=%ERRORLEVEL%");
		appendLine(data, "popd");

		appendLine(data, "");

		appendLine(data, "EXIT exitCode");
		appendLine(data, "@REM pause");

		char ret = File::writeFileIfChanged(outpath, data, true);
		hasChanges |= ret == 'U';
	}

	#endif

	return hasChanges;
}

void 
ShaderCompileRequest::processPermutationRequests(RenderDevice* renderDevice, const ProjectSetting& projectSetting)
{
	const auto& ps			= projectSetting;
	auto&		rdDev		= *renderDevice;
	auto&		shaderStock	= rdDev.shaderStock();

	auto& permutReq = shaderStock.permutationRequest();
	for (auto& pair : permutReq)
	{
		auto&		mtl			= *pair.first;
		auto*		prevShader	= mtl.shader();
		const auto& filename	= prevShader->filename();

		TempString impShaderDir;
		ShaderCompileRequest::getImportedShaderDirTo(impShaderDir, filename, ps);

		SPtr<Shader> shader = compilePermutationShader(filename, impShaderDir, mtl.permutations(), rdDev, ps);
		mtl.setShader(shader);
	}
	
	shaderStock.clearPermutationRequest();
}

void 
ShaderCompileRequest::_hotReloadShader(StrView filename, RenderDevice* renderDevice, const ProjectSetting& projectSetting)
{
	auto& ps			= projectSetting;
	auto& rdDev			= *renderDevice;
	auto& shaderStock	= rdDev.shaderStock();

	auto* shaders	= shaderStock.getShaders(filename);

	if (!shaders)
		return;

	// remove all permutation first
	SPtr<Shader> shader = shaderStock.findShader(filename);
	shaders->clear();
	shaders->emplace_back(shader);
	shader->onReset();

	auto* mtlTable = shaderStock.getMaterials(shader);
	if (!mtlTable)
		return;

	TempString impShaderDir;
	TempString shaderPermutDir;
	ShaderCompileRequest::getImportedShaderDirTo(impShaderDir, filename, ps);
	ShaderCompileRequest::getShaderPermutationDirTo(shaderPermutDir, impShaderDir, "", ps);
	Path::removeAll(shaderPermutDir);

	auto& info = shader->info();

	RDS_TODO("TODO: Material::setShader() will remove itself from mtlTable, however we are using it to looping");
	RDS_TODO(", so it has to copy here, rewrite another function for hotReload to solve this problem (probably pass a isHotReloading bool to it, or a delete request????)");
	ShaderStock::Materials mtls = *mtlTable;
	for (auto& mtl : mtls)
	{
		// if pervious has permutation, compare its value to new shader info, if no change
		if (info.permuts.is_empty())
		{
			mtl->setShader(shader);
		}
		else
		{
			mtl->resetPermutation(info.permuts);
			
			const auto&  permuts		= mtl->permutations();
			SPtr<Shader> permutShader	= compilePermutationShader(filename, impShaderDir, permuts, rdDev, ps);

			mtl->setShader(permutShader);
		}
	}
}

SPtr<Shader> 
ShaderCompileRequest::compilePermutationShader(StrView filename, StrView impShaderDir, const Permutations& permuts, RenderDevice& renderDevice, const ProjectSetting& projectSetting)
{
	auto& ps	= projectSetting;
	auto& rdDev = renderDevice;

	bool isPermutatedShader = !permuts.isEmpty();

	if (!isPermutatedShader)
		return rdDev.createShader(filename);

	TempString shaderPermutDir;
	TempString permutName;

	permuts.appendNameTo(permutName);
	ShaderCompileRequest::getShaderPermutationDirTo(shaderPermutDir, impShaderDir, permutName, ps);

	bool isExist = Directory::isExist(shaderPermutDir);
	if (isExist)
		return rdDev.createShader(filename, permuts);

	RDS_CORE_LOG("--- Compile shader permutation: {}", filename);
	{
		CmdLineArgs args;

		args.addStr(ps.shaderCompilerRoot());
		args.addArg("-cwd={}",	ps.projectRoot());
		args.addArg("-file={}", filename);

		for (size_t i = 0; i < permuts.size(); i++)
		{
			auto& permut = permuts[i];
			args.addArg("-D{}={}", permut.name(), permut.value());
		}

		Process proc;
		proc.execute(ps.shaderCompilerRoot(), args.stream());
	}

	return rdDev.createShader(filename, permuts);
}


#endif

}