#include "rds_shader_compiler-pch.h"
#include "rdsShaderParser.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderParser-Impl ---
#endif // 0
#if 1
void 
ShaderParser::parse(ShaderInfo& outInfo, StrView filename)
{
	_mmfile.open(filename);
	parse(outInfo, _mmfile.span(), filename);
}

void ShaderParser::parse(ShaderInfo& outInfo, ByteSpan data, StrView filename)
{
	_outInfo = &outInfo;

	outInfo.clear();
	reset(data, filename);

	skipNewline();

	while (!token().isNone() && !token().isIdentifier("Shader"))
	{
		nextToken();
	}

	if (token().isIdentifier("Shader"))
	{
		_parseShader();
	}
	else 
	{
		error("missing Shader tag");
	}
}

void ShaderParser::dump(StrView filename)
{
	if (!_outInfo)
		return;
	JsonUtil::writeFileIfChanged(filename, *_outInfo, true);
}

void ShaderParser::_parseShader()
{
	//using Lexer::Token

	const Token& token = this->token();

	nextToken();
	expected(TokenType::Operator, "{");

	for (;;) 
	{
		if (token.isOperator("}"))		{ nextToken(); break; }
		if (token.isNewline())			{ nextToken(); continue; }

		if (token.isIdentifier("Properties"))	{ _parseProperties();	continue; }
		if (token.isIdentifier("Pass"))			{ _parsePass();			continue; }
		if (token.isIdentifier("Permutation"))	{ _parsePermutation();	continue; }

		return errorUnexpectedToken();
	}
}

void ShaderParser::_parseProperties()
{
	nextToken();
	expected(TokenType::Operator, "{");

	const Token& token = this->token();

	for (;;) 
	{
		skipNewline();
		if (token.isOperator("}")) 
		{ 
			nextToken(); break; 
		}
		_parseProperty();
	}
}

void ShaderParser::_parseProperty()
{
	using Prop = ShaderInfo::Prop;
	Prop& prop = _outInfo->props.emplace_back();
	const Token& token = this->token();

	if (token.isOperator("["))
	{
		nextToken();
		while (!token.isNone())
		{
			skipNewline();

			if (token.isIdentifier("DisplayName"))
			{
				nextToken();
				expected(TokenType::Operator, "=");
				readString(prop.displayName);
			}
			if (token.isOperator("]")) { nextToken(); break; }

			expected(TokenType::Operator, ",");
		}
	}

	skipNewline();

	{
		// prop type
		_parseEnum(prop.type);

		// prop name
		readIdentifier(prop.name);
	}

	// optional defaultValue
	if (token.isOperator("=")) 
	{
		nextToken();
		while (!token.isNone()) 
		{
			if (token.isNewline()) 
			{ 
				break; 
			}
			prop.defaultValue += token.value();
			nextToken();
		}
	}

	if (!token.isNewline()) 
	{
		errorUnexpectedToken();
	}
	nextToken();
}

void ShaderParser::_parsePass()
{
	const Token& token = this->token();

	nextToken();
	auto& o = _outInfo->passes.emplace_back();

	if (token.isString()) 
	{
		readString(o.name);
	}
	expected(TokenType::Operator, "{");

	for (;;) 
	{
		if (token.isOperator("}"))	{ nextToken(); break; }
		if (token.isNewline())		{ nextToken(); continue; }

		if (token.isIdentifier("VsFunc"))		{ nextToken(); readIdentifier(o.vsFunc); continue; }
		if (token.isIdentifier("PsFunc"))		{ nextToken(); readIdentifier(o.psFunc); continue; }
		if (token.isIdentifier("CsFunc"))		{ nextToken(); readIdentifier(o.csFunc); continue; }

		if (token.isIdentifier("Cull"))			{ nextToken(); readEnum(o.renderState.cull); continue; }

		if (token.isIdentifier("DepthTest"))	{ nextToken(); readEnum(o.renderState.depthTest.op); continue; }
		if (token.isIdentifier("DepthWrite"))	{ nextToken(); readBool(o.renderState.depthTest.writeMask); continue; }

		if (token.isIdentifier("Wireframe"))	{ nextToken(); readBool(o.renderState.wireframe); continue; }

		if (token.isIdentifier("BlendRGB")   )	{ nextToken(); _readBlendFunc(o.renderState.blend.rgb); continue; }
		if (token.isIdentifier("BlendAlpha") )	{ nextToken(); _readBlendFunc(o.renderState.blend.alpha); continue; }

		return errorUnexpectedToken();
	}
}

void 
ShaderParser::_parsePermutation()
{
}

void 
ShaderParser::_parsePermutation_Value()
{
}

void 
ShaderParser::_readBlendFunc(RenderState::BlendFunc& v)
{
	readEnum(v.op);
	readEnum(v.srcFactor);
	readEnum(v.dstFactor);
}


#endif

}
