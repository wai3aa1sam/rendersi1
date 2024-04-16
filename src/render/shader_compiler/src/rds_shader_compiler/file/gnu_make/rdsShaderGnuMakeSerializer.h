#pragma once

#include "rds_shader_compiler/common/rds_shader_compiler_common.h"
#include "rdsGnuMakeSerializer.h"
#include "../../rdsShaderCompileRequest.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderGnuMakeSerializer-Decl ---
#endif // 0
#if 1

class ShaderGnuMakeSerializer : public GnuMakeSerializer
{
public:
	using Base = GnuMakeSerializer;
	//using ShaderCompileRequest = ShaderCompileRequest;
	struct Request;
	
public:
	using ApiType = RenderApiType;

public:
	static void dump(StrView filename, const ShaderCompileRequest& cReq);

public:
	ShaderGnuMakeSerializer(const ShaderCompileRequest& cReq);

private:
	struct BuildRequest
	{
		BuildRequest() = default;
		BuildRequest(const String& entry, ShaderStageFlag stage, size_t passIndex, ApiType apiType)
			: _entry(&entry), _stage(stage), _passIndex(passIndex), _apiType(apiType)
		{
		}

		void reset(const String& entry, ShaderStageFlag stage)
		{
			this->_entry	= &entry;
			this->_stage	= stage;
		}

		bool build(ShaderGnuMakeSerializer* gnu, Request& request, const String& entry, ShaderStageFlag stage, size_t passIndex, ApiType apiType)
		{
			_entry		= &entry;
			_stage		= stage;
			_passIndex	= passIndex;
			_apiType	= apiType;

			return gnu->_init_bin(request, *this);
		}

		const String*	_entry = nullptr;
		ShaderStageFlag _stage;
		size_t			_passIndex;
		ApiType			_apiType;
	};

	static void generateDepdency(Request& request, BuildRequest& buildReq);
	void _init_variables(				Request& request);
	void _init_includes(				Request& request);
	void _init_build_target(			Request& request, ApiType apiType);
	bool _init_bin(						Request& request, BuildRequest& buildReq);
	bool _init_cli(						Request& request, BuildRequest& buildReq);
	bool _init_cli_rdsShaderCompiler(	Request& request, BuildRequest& buildReq);
	bool _init_cli_glslc(				Request& request, BuildRequest& buildReq);
	void _init_cli_include(				Request& request, BuildRequest& buildReq, StrView syntax);
	void _init_cli_marco(				Request& request, BuildRequest& buildReq, StrView syntax);

public:
	const ShaderCompileRequest& compileRequest() const;

public:
	struct Request : public RequestBase
	{
		const ShaderCompileRequest* compileRequest = nullptr;
	};

protected:
	Request _request;
};

inline const ShaderCompileRequest& ShaderGnuMakeSerializer::compileRequest() const { return *_request.compileRequest; }

#endif

}