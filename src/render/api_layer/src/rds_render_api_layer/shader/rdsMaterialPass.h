#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsShaderInfo.h"
#include "rdsShaderPass.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rdsShaderResource.h"

namespace rds
{

class ShaderPass;
struct VertexShaderStage;
struct PixelShaderStage;

class Material;
class MaterialPass;

class RenderContext;


#if 0
#pragma mark --- rdsMaterialPass_Stage-Decl ---
#endif // 0
#if 1

// TODO: add a template here
struct MaterialPass_Stage
{
	friend class Material;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Info			= ShaderStageInfo;
	using ConstBuffer	= ShaderResources::ConstBuffer;
	using TexParam		= ShaderResources::TexParam;
	using SamplerParam	= ShaderResources::SamplerParam;
	using BufferParam	= ShaderResources::BufferParam;
	using ImageParam	= ShaderResources::ImageParam;

public:
	static constexpr SizeType s_kLocalConstBufSize = 2;

public:
	MaterialPass_Stage();
	~MaterialPass_Stage();

	void create(MaterialPass* pass, ShaderStage* shaderStage);
	void destroy();

	template<class T>	void setParam(			Material* mtl, StrView name, const T&				v);
	template<class T>	void setArray(			Material* mtl, StrView name, const Span<T>&			v);
	template<class TEX> void setTexParam(		Material* mtl, StrView name, TEX*					v);
						void setSamplerParam(	Material* mtl, StrView name, const SamplerState&	v);
						void setBufferParam(	Material* mtl, StrView name, RenderGpuBuffer*		v);

	const ShaderStageInfo&	info() const;
	//ShaderResources&		shaderResources(Material* mtl);

protected:
	ShaderStage*				_shaderStage = nullptr;
	//FramedT<ShaderResources>	_framedShaderResources;
};

#if 0
#pragma mark --- rdsMaterialPass_Stage-Impl ---
#endif // 0
#if 1

#if !RDS_SHADER_USE_BINDLESS
template<class T> inline 
void 
MaterialPass_Stage::setParam(Material* mtl, StrView name, const T& v)
{
	/*if (!_shaderStage)
	return;*/
	shaderResources(mtl).setParam(name, v);
}

template<class T> inline
void 
MaterialPass_Stage::setArray(Material* mtl, StrView name, const Span<T>& v)
{
	shaderResources(mtl).setArray(name, v);
}

template<class TEX> inline
void 
MaterialPass_Stage::setTexParam(Material* mtl, StrView name, TEX* v)
{
	/*if (!_shaderStage)
	return;*/
	shaderResources(mtl).setTexParam(name, v);
}

inline
void 
MaterialPass_Stage::setSamplerParam(Material* mtl, StrView name, const SamplerState& samplerState)
{
	/*if (!_shaderStage)
	return;*/
	shaderResources(mtl).setSamplerParam(name, samplerState);
}

inline
void 
MaterialPass_Stage::setBufferParam(Material* mtl, StrView name, RenderGpuBuffer* v)
{
	/*if (!_shaderStage)
	return;*/
	shaderResources(mtl).setBufferParam(name, v);
}
#endif // 0

inline const ShaderStageInfo& MaterialPass_Stage::info() const { return _shaderStage->info(); }

#endif // 1

struct MaterialPass_VertexStage : public MaterialPass_Stage
{
public:
			VertexShaderStage* shaderStage();
	const	VertexShaderStage* shaderStage() const;

protected:
};

struct MaterialPass_TessellationControlStage : public MaterialPass_Stage
{
public:
			TessellationControlShaderStage* shaderStage();
	const	TessellationControlShaderStage* shaderStage() const;

protected:
};

struct MaterialPass_TessellationEvaluationStage : public MaterialPass_Stage
{
public:
			TessellationEvaluationShaderStage* shaderStage();
	const	TessellationEvaluationShaderStage* shaderStage() const;

protected:
};

struct MaterialPass_GeometryStage : public MaterialPass_Stage
{
public:
			GeometryShaderStage* shaderStage();
	const	GeometryShaderStage* shaderStage() const;

protected:
};

struct MaterialPass_PixelStage : public MaterialPass_Stage
{
public:
			PixelShaderStage* shaderStage();
	const	PixelShaderStage* shaderStage() const;

protected:
};

struct MaterialPass_ComputeStage : public MaterialPass_Stage
{
public:
			ComputeShaderStage* shaderStage();
	const	ComputeShaderStage* shaderStage() const;

protected:
};

inline			VertexShaderStage*					MaterialPass_VertexStage::shaderStage()							{ return sCast<VertexShaderStage*>(_shaderStage); }
inline const	VertexShaderStage*					MaterialPass_VertexStage::shaderStage() const					{ return sCast<VertexShaderStage*>(_shaderStage); }

inline			TessellationControlShaderStage*		MaterialPass_TessellationControlStage::shaderStage()			{ return sCast<TessellationControlShaderStage*>(_shaderStage); }
inline const	TessellationControlShaderStage*		MaterialPass_TessellationControlStage::shaderStage() const		{ return sCast<TessellationControlShaderStage*>(_shaderStage); }

inline			TessellationEvaluationShaderStage* MaterialPass_TessellationEvaluationStage::shaderStage()			{ return sCast<TessellationEvaluationShaderStage*>(_shaderStage); }
inline const	TessellationEvaluationShaderStage* MaterialPass_TessellationEvaluationStage::shaderStage() const	{ return sCast<TessellationEvaluationShaderStage*>(_shaderStage); }

inline			GeometryShaderStage*				MaterialPass_GeometryStage::shaderStage()						{ return sCast<GeometryShaderStage*>(_shaderStage); }
inline const	GeometryShaderStage*				MaterialPass_GeometryStage::shaderStage() const					{ return sCast<GeometryShaderStage*>(_shaderStage); }

inline			PixelShaderStage*					MaterialPass_PixelStage::shaderStage()							{ return sCast<PixelShaderStage*>(_shaderStage); }
inline const	PixelShaderStage*					MaterialPass_PixelStage::shaderStage() const					{ return sCast<PixelShaderStage*>(_shaderStage); }

inline			ComputeShaderStage*					MaterialPass_ComputeStage::shaderStage()						{ return sCast<ComputeShaderStage*>(_shaderStage); }
inline const	ComputeShaderStage*					MaterialPass_ComputeStage::shaderStage() const					{ return sCast<ComputeShaderStage*>(_shaderStage); }

#endif

#if 0
#pragma mark --- rdsMaterialPass-Decl ---
#endif // 0
#if 1

class MaterialPass
{
	friend class Material;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Stage							= MaterialPass_Stage;
	using VertexStage					= MaterialPass_VertexStage;
	using TessellationControlStage		= MaterialPass_TessellationControlStage;
	using TessellationEvaluationStage	= MaterialPass_TessellationEvaluationStage;
	using GeometryStage					= MaterialPass_GeometryStage;
	using PixelStage					= MaterialPass_PixelStage;
	using ComputeStage					= MaterialPass_ComputeStage;

	using Info = ShaderInfo::Pass;

public:
	MaterialPass();
	virtual ~MaterialPass();

	void create(Material* material, ShaderPass* shaderPass);
	void destroy();

public:
	void uploadToGpu();

public:
	//void bind(RenderContext* ctx, const VertexLayout* vtxLayout);

	template<class TEX> void setTexParam	(	StrView name, TEX*					v);
	template<class T>	void setArray(			StrView name, const Span<T>&		v);
	template<class T>	void setParam(			StrView name, const T&				v);
						void setSamplerParam(	StrView name, u32					samplerIndex);
						void setBufferParam(	StrView name, RenderGpuBuffer*		v);
	template<class TEX> void setImageParam(		StrView name, TEX*					v, u32 mipLevel);

public:
	const Info& info() const;

	VertexStage*					vertexStage();
	TessellationControlStage*		tessellationControlStage();
	TessellationEvaluationStage*	tessellationEvaluationStage();
	GeometryStage*					geometryStage();
	PixelStage*						pixelStage();
	ComputeStage*					computeStage();

	Material&			material();
	ShaderPass&			shaderPass();
	ShaderResources&	shaderResources();

	u32 lastEngineFrameIndex() const;

protected:
	virtual void onCreate(Material* material, ShaderPass* shaderPass);
	virtual void onDestroy();

	//virtual void onBind(RenderContext* ctx, const VertexLayout* vtxLayout) = 0;

protected:
	Material*						_material		= nullptr;
	ShaderPass*						_shaderPass		= nullptr;
	VertexStage*					_vertexStage	= nullptr;
	TessellationControlStage*		_tescStage		= nullptr;
	TessellationEvaluationStage*	_teseStage		= nullptr;
	GeometryStage*					_geometryStage	= nullptr;
	PixelStage*						_pixelStage		= nullptr;
	ComputeStage*					_computeStage	= nullptr;

	FramedShaderResources _framedShaderRscs;
};

template<class T> inline 
void 
MaterialPass::setParam(StrView name, const T& v)
{
	auto* mtl = _material; RDS_UNUSED(mtl);

	#if RDS_SHADER_USE_BINDLESS
	_framedShaderRscs.setParam(name, v);
	#else
	if (_vertexStage)	_vertexStage ->setParam(mtl, name, v);
	if (_pixelStage)	_pixelStage	 ->setParam(mtl, name, v);
	if (_computeStage)	_computeStage->setParam(mtl, name, v);
	#endif
}

template<class T> inline
void 
MaterialPass::setArray(StrView name, const Span<T>& v)
{
	auto* mtl = _material; RDS_UNUSED(mtl);
	_framedShaderRscs.setArray(name, v);
}

template<class TEX> inline 
void 
MaterialPass::setTexParam(StrView name, TEX* v)
{
	auto* mtl = _material; RDS_UNUSED(mtl);

	#if RDS_SHADER_USE_BINDLESS
	_framedShaderRscs.setParam(name, v);
	#else
	if (_vertexStage)	_vertexStage ->setTexParam(mtl, name, v);
	if (_pixelStage)	_pixelStage  ->setTexParam(mtl, name, v);
	if (_computeStage)	_computeStage->setTexParam(mtl, name, v);
	#endif
}

inline
void 
MaterialPass::setSamplerParam(StrView name, u32 samplerIndex)
{
	auto* mtl = _material; RDS_UNUSED(mtl);

	#if RDS_SHADER_USE_BINDLESS
	_framedShaderRscs.setParam(name, samplerIndex);
	#else
	if (_vertexStage)	_vertexStage ->setSamplerParam(mtl, name, v);
	if (_pixelStage)	_pixelStage  ->setSamplerParam(mtl, name, v);
	if (_computeStage)	_computeStage->setSamplerParam(mtl, name, v);
	#endif
}

inline
void 
MaterialPass::setBufferParam(StrView name, RenderGpuBuffer* v)
{
	auto* mtl = _material; RDS_UNUSED(mtl);

	#if RDS_SHADER_USE_BINDLESS
	_framedShaderRscs.setParam(name, v);
	#else
	if (_computeStage)	_computeStage->setBufferParam(mtl, name, v);
	#endif
}

inline const MaterialPass::Info& MaterialPass::info() const { return _shaderPass->info(); }

inline MaterialPass::VertexStage*					MaterialPass::vertexStage()					{ return _vertexStage; }
inline MaterialPass::TessellationControlStage*		MaterialPass::tessellationControlStage()	{ return _tescStage; }
inline MaterialPass::TessellationEvaluationStage*	MaterialPass::tessellationEvaluationStage() { return _teseStage; }
inline MaterialPass::GeometryStage*					MaterialPass::geometryStage()				{ return _geometryStage; }
inline MaterialPass::PixelStage*					MaterialPass::pixelStage()					{ return _pixelStage; }
inline MaterialPass::ComputeStage*					MaterialPass::computeStage()				{ return _computeStage; }

inline Material&					MaterialPass::material()					{ return *_material; }
inline ShaderPass&					MaterialPass::shaderPass()					{ return *_shaderPass; }
inline ShaderResources&				MaterialPass::shaderResources()				{ return _framedShaderRscs.shaderResource(); }

inline u32							MaterialPass::lastEngineFrameIndex() const	{ return _framedShaderRscs.lastEngineFrameIndex(); }

#endif

}