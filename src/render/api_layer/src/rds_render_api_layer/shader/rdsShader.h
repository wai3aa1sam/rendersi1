#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsShaderPass.h"
#include "rds_render_api_layer/transfer/command/rdsTransferCommand.h"

namespace rds
{

class RenderDevice;
class ShaderCompileRequest;

struct Shader_CreateDesc : public RenderResource_CreateDesc
{
	TempString					filename;
	const ShaderPermutations*	permuts = nullptr;
};

#if 0
#pragma mark --- rdsShader-Decl ---
#endif // 0
#if 1

class Shader : public RenderResource
{
	friend class RenderDevice;
	friend class ShaderCompileRequest;
public:
	using Base			= RenderResource;
	using This			= Shader;
	using CreateDesc	= Shader_CreateDesc;
	using CmdCreate		= TransferCommand_CreateShader;
	using CmdDestroy	= TransferCommand_DestroyShader;

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
	static SPtr<Shader>		make(RDS_DebugLabel_PARAM, const CreateDesc& cDesc);

public:
	Shader();
	virtual ~Shader();

protected:
	void create(const CreateDesc& cDesc);
	void create(StrView filename);

	virtual void onDestroy() override;

public:
	ShaderPropId makePropNameId(StrView name) const;

	int getPropIndexBy(	const ShaderPropId& nameId) const;
	int getPassIndexBy(	const ShaderPropId& nameId) const;

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

	virtual void onReset();

	virtual UPtr<ShaderPass> onMakePass(Shader* shader, const ShaderPass::Info& info) = 0;

protected:
	String		_filename;
	String		_shadername;
	ShaderInfo	_info;
	
	Passes		_passes;
	Permuts		_permuts;

	VectorMap<ShaderPropId, int> _passNameIdMap;
};

inline Span<UPtr<Shader::Pass> >	Shader::passes()			{ return _passes.span(); }
inline const String&				Shader::filename()	const	{ return _filename; }
inline const Shader::Info&			Shader::info()		const	{ return _info; }

inline const	String&				Shader::shadername()	const	{ return _shadername; }
inline const	Shader::Permuts&	Shader::permutations()	const	{ return _permuts; }
inline 			Shader::Permuts&	Shader::permutations() 			{ return _permuts; }


#endif

}