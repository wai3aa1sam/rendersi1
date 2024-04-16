#pragma once

#include "rds_shader_compiler/common/rds_shader_compiler_common.h"
#include "rds_render_api_layer/shader/rdsShaderInfo.h"
#include "../rdsShaderCompileRequest.h"

/*
references:
- src/render/shader_compiler/ShaderParser.h in https://github.com/SimpleTalkCpp/SimpleGameEngine
*/

namespace rds
{

#if 0
#pragma mark --- rdsShaderParser-Decl ---
#endif // 0
#if 1

class ShaderParser : public Lexer
{
public:
	void parse(ShaderCompileRequest* oCReq, ShaderInfo* outInfo, StrView filename);
	void parse(ShaderCompileRequest* oCReq, ShaderInfo* outInfo, ByteSpan data, StrView filename);

	void dump(StrView filename);

protected:
	void _parseShader();
	void _parseProperties();
	void _parseProperty();
	void _parsePass();

	void _parsePermutation();
	void _parsePermutation_Value();

	void _parseInclude();

protected:
	void readBlendFunc(RenderState::BlendFunc& v);

protected:
	ShaderInfo&				info();
	ShaderCompileRequest&	compileRequest();

protected:
	MemMapFile				_mmfile;
	ShaderInfo*				_outInfo = nullptr;
	ShaderCompileRequest*	_oCReq	 = nullptr;

private:

};

inline ShaderInfo&				ShaderParser::info()			{ return *_outInfo; }
inline ShaderCompileRequest&	ShaderParser::compileRequest()	{ return *_oCReq; }

#endif

}