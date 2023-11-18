#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsShaderPass.h"

namespace rds
{

struct Shader_CreateDesc : public RenderResource_CreateDesc
{
	StrView filename;
};

#if 0
#pragma mark --- rdsShader-Decl ---
#endif // 0
#if 1

class Shader : public RenderResource
{
public:
	using Base			= RenderResource;
	using This			= Shader;
	using CreateDesc	= Shader_CreateDesc;

	using Pass	= ShaderPass;
	using Stage = ShaderStage;

	using Info = ShaderInfo;

public:
	static constexpr SizeType s_kLocalPassSize = 2;

public:
	static CreateDesc		makeCDesc();
	static SPtr<Shader>		make(const CreateDesc& cDesc);

public:
	Shader();
	virtual ~Shader();

	void create(const CreateDesc& cDesc);
	void create(StrView filename);
	void destroy();

	ShaderParamId getParamId() const;
	
	Span<UPtr<Pass> > passes();

	const String&	filename()	const;
	const Info&		info()		const;

	virtual void onReset();

protected:
	virtual void onCreate		(const CreateDesc& cDesc);
	virtual void onPostCreate	(const CreateDesc& cDesc);
	virtual void onDestroy		();

	//virtual UPtr<ShaderPass> onMakePass(Shader* shader, ShaderPass::Info* info) = 0;

protected:
	String		_filename;
	ShaderInfo	_info;
	
	Vector<UPtr<Pass>, s_kLocalPassSize> _passes;
};

inline Span<UPtr<Shader::Pass> >	Shader::passes()			{ return _passes.span(); }
inline const String&				Shader::filename()	const	{ return _filename; }
inline const Shader::Info&			Shader::info()		const	{ return _info; }


#endif

}