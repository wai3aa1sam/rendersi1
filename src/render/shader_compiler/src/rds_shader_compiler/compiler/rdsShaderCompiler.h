#pragma once

#include "rds_shader_compiler/common/rds_shader_compiler_common.h"
#include "rds_render_api_layer/shader/rdsShaderInfo.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderCompiler-Decl ---
#endif // 0
#if 1

struct ShaderCompileOption
{
public:
	ShaderCompileOption()
		: isDebug(false), isToSpirv(false), enableLog(false)
	{

	}

	bool isDebug	: 1;
	bool enableLog	: 1;
	bool isToSpirv	: 1;
};

class ShaderCompiler : public NonCopyable
{
public:
	using Option = ShaderCompileOption;

public:
	virtual ~ShaderCompiler() = default;

protected:
	template<class... ARGS> void log(const char* fmt, ARGS&&... args);
	template<class... ARGS> void _log(const char* fmt, ARGS&&... args);

protected:
	const Option& opt() const;

protected:
	const Option* _opt = nullptr;
};

inline const ShaderCompiler::Option& ShaderCompiler::opt() const { return *_opt; }

template<class... ARGS> inline
void 
ShaderCompiler::log(const char* fmt, ARGS&&... args)
{
	if (!opt().enableLog)
		return;

	_log(fmt, rds::forward<ARGS>(args)...);
}

template<class... ARGS> inline
void 
ShaderCompiler::_log(const char* fmt, ARGS&&... args)
{
	RDS_CORE_LOG(fmt, rds::forward<ARGS>(args)...);
}

#endif

}