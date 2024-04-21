#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsShaderPass.h"

namespace rds
{

class ShaderCompileRequest;

struct Shader_CreateDesc : public RenderResource_CreateDesc
{
	StrView filename;
	const ShaderPermutations* permuts = nullptr;
};

#if 0
#pragma mark --- rdsShader-Decl ---
#endif // 0
#if 1

class Shader : public RenderResource
{
	friend class ShaderCompileRequest;
public:
	using Base			= RenderResource;
	using This			= Shader;
	using CreateDesc	= Shader_CreateDesc;

	using Pass	= ShaderPass;
	using Stage = ShaderStage;

	using Info		= ShaderInfo;
	using Permuts	= ShaderPermutations;

public:
	static constexpr SizeType s_kLocalPassSize = 2;

public:
	using Passes = Vector<UPtr<Pass>, s_kLocalPassSize>;

public:
	static CreateDesc		makeCDesc();
	static SPtr<Shader>		make(const CreateDesc& cDesc);

public:
	Shader();
	virtual ~Shader();

	void create(const CreateDesc& cDesc);
	void create(StrView filename);
	void destroy();

public:
	ShaderParamId getParamId() const;
	
	Span<UPtr<Pass> > passes();

	const String&	filename()	const;
	const Info&		info()		const;
	
	const	String&  shadername()	const;
	const	Permuts& permutations() const;
			Permuts& permutations();
	bool			 isPermutatedShader() const;

protected:
	virtual void onCreate		(const CreateDesc& cDesc);
	virtual void onPostCreate	(const CreateDesc& cDesc);
	virtual void onDestroy		();

	virtual void onReset();

	virtual UPtr<ShaderPass> onMakePass(Shader* shader, const ShaderPass::Info& info) = 0;

protected:
	String		_filename;
	String		_shadername;
	ShaderInfo	_info;
	
	Passes		_passes;
	Permuts		_permuts;
};

inline Span<UPtr<Shader::Pass> >	Shader::passes()			{ return _passes.span(); }
inline const String&				Shader::filename()	const	{ return _filename; }
inline const Shader::Info&			Shader::info()		const	{ return _info; }

inline const	String&				Shader::shadername()	const	{ return _shadername; }
inline const	Shader::Permuts&	Shader::permutations()	const	{ return _permuts; }
inline 			Shader::Permuts&	Shader::permutations() 			{ return _permuts; }


#endif

}