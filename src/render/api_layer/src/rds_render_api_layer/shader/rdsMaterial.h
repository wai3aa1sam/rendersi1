#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
//#include "rds_render_api_layer/rdsRenderDevice.h"
#include "rdsShaderInfo.h"
#include "rdsMaterialPass.h"

namespace rds
{

class Shader;
class RenderContext;

class RenderGpuBuffer;

class Texture;
class RenderGpuBuffer;

#if 0
#pragma mark --- rdsMaterial-Decl ---
#endif // 0
#if 1

struct Material_CreateDesc : public RenderResource_CreateDesc
{
	Shader* shader = nullptr;
};

class Material : public RenderResource
{
public:
	using Base			= RenderResource;
	using This			= Material;

	using CreateDesc	= Material_CreateDesc;

	using Pass			= MaterialPass;
	using Stage			= MaterialPass_Stage;
	using VertexStage	= MaterialPass_VertexStage;
	using PixelStage	= MaterialPass_PixelStage;

public:
	static constexpr SizeType s_kShaderStageCount	= enumInt(ShaderStageFlag::_kCount);
	static constexpr SizeType s_kLocalPassSize		= 2;

public:
	static CreateDesc		makeCDesc();
	static SPtr<Material>	make(const CreateDesc& cDesc);
	static SPtr<Material>	make(Shader* shader);

public:
	Material();
	virtual ~Material();

	void create	(const CreateDesc& cDesc);
	void destroy();

	void setShader(Shader* shader);

	void setParam(StrView name,			Texture2D*		v, const SamplerState& samplerState = {});
	void setParam(StrView name,			TextureCube*	v, const SamplerState& samplerState = {});
	void setParam(StrView name, const	SamplerState&	v);

	void setParam(StrView name, const i32&		v);
	void setParam(StrView name, const u32&		v);
	void setParam(StrView name, const f32&		v);
	void setParam(StrView name, const Color4b&	v);
	void setParam(StrView name, const Tuple2f&	v);
	void setParam(StrView name, const Tuple3f&	v);
	void setParam(StrView name, const Tuple4f&	v);
	void setParam(StrView name, const Mat4f&	v);

	void setParam(StrView name, RenderGpuBuffer* v);

	Pass*			getPass		(SizeType i);
	ShaderParamId	getParamId	(StrView name) const;

	Span<UPtr<Pass> >	passes();
	Shader*				shader();
	const ShaderInfo&	info() const;

	u32 frameIdx() const;

protected:
	virtual void onCreate		(const CreateDesc& cDesc);
	virtual void onPostCreate	(const CreateDesc& cDesc);
	virtual void onDestroy		();

	virtual UPtr<MaterialPass> onMakePass() = 0;

protected:
	template<class T>	void _setParam			(StrView name, const	T&					v);
	template<class TEX> void _setTexParam		(StrView name,			TEX*				v, const SamplerState& samplerState);
						void _setSamplerParam	(StrView name, const	SamplerState&		v);
						void _setBufferParam	(StrView name,			RenderGpuBuffer*	v);

protected:
	SPtr<Shader> _shader;
	Vector<UPtr<MaterialPass>, s_kLocalPassSize> _passes;

	u32 _frameIdx = 0;
};

inline void Material::setParam(StrView name,		Texture2D*		v, const SamplerState& samplerState)	{ return _setTexParam(name, v, samplerState); }
inline void Material::setParam(StrView name,		TextureCube*	v, const SamplerState& samplerState)	{ return _setTexParam(name, v, samplerState); }
inline void Material::setParam(StrView name, const	SamplerState&	v)										{ return _setSamplerParam(name, v); }

inline void Material::setParam(StrView name, const i32&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const u32&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const f32&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Color4b&	v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple2f&	v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple3f&	v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple4f&	v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Mat4f&	v)		{ return _setParam(name, v); }

inline void Material::setParam(StrView name, RenderGpuBuffer* v)	{ return _setBufferParam(name, v); }

inline
Material::Pass* 
Material::getPass(SizeType i)
{
	if (i >= _passes.size())
	{
		RDS_CORE_ASSERT(false, "invalid pass index");
		return nullptr;
	}
	return _passes[i].ptr();
}

inline Shader*						Material::shader() { return _shader; }
inline Span<UPtr<Material::Pass>>	Material::passes() { return _passes; }

inline u32 Material::frameIdx() const { return _frameIdx; }

template<class T> inline
void 
Material::_setParam(StrView name, const T& v)
{
	for (auto& pass : _passes)
	{
		pass->setParam(name, v);
	}
}

template<class TEX> inline 
void 
Material::_setTexParam(StrView name, TEX* v, const SamplerState& samplerState)
{
	for (auto& pass : _passes)
	{
		pass->setTexParam(name, v);
	}

	setParam(name, v->bindlessHandle().getResourceIndex());

	TempString samplerName;
	ShaderResources::getSamplerNameTo(samplerName, name);
	setParam(samplerName, samplerState);
}

inline
void 
Material::_setBufferParam(StrView name, RenderGpuBuffer* v)
{
	for (auto& pass : _passes)
	{
		pass->setBufferParam(name, v);
	}

	setParam(name, v->bindlessHandle().getResourceIndex());
}

#endif

}