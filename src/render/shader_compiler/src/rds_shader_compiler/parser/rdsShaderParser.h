#pragma once

#include "rds_shader_compiler/common/rds_shader_compiler_common.h"
#include "rds_render_api_layer/shader/rdsShaderInfo.h"

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
	void parse(ShaderInfo& outInfo, StrView filename);
	void parse(ShaderInfo& outInfo, ByteSpan data, StrView filename);

	void dump(StrView filename);

protected:
	void _parseShader();
	void _parseProperties();
	void _parseProperty();
	void _parsePass();

	void _parsePermutation();
	void _parsePermutation_Value();

	//void _parseBlendFunc(RenderState::BlendFunc& v);

	template<class E> void _parseEnum(E& v_);

protected:
	MemMapFile	_mmfile;
	ShaderInfo* _outInfo = nullptr;

private:

};

template<class E> inline
void 
ShaderParser::_parseEnum(E& v)
{
	errorIf(!enumTryParse(v, token().value()), "ShaderParse parse enum failed");
	nextToken();
}


#endif

}