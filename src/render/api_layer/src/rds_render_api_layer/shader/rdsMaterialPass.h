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

	template<class TEX> void setTexParam	(Material* mtl, StrView name, TEX* v);
	template<class T>	void setParam		(Material* mtl, StrView name, const T& v);
						void setSamplerParam(Material* mtl, StrView name, const SamplerState&	v);
						void setBufferParam	(Material* mtl, StrView name, RenderGpuBuffer*		v);

	const ShaderStageInfo&	info() const;
	ShaderResources&		shaderResources(Material* mtl);

protected:
	ShaderStage*			_shaderStage = nullptr;
	FramedShaderResources	_framedShaderResources;
};

#if 0
#pragma mark --- rdsMaterialPass_Stage-Impl ---
#endif // 0
#if 1

template<class T> inline 
void 
MaterialPass_Stage::setParam(Material* mtl, StrView name, const T& v)
{
	/*if (!_shaderStage)
		return;*/
	shaderResources(mtl).setParam(name, v);
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

inline const ShaderStageInfo& MaterialPass_Stage::info() const { return _shaderStage->info(); }

#endif // 1

struct MaterialPass_VertexStage : public MaterialPass_Stage
{
public:
			VertexShaderStage* shaderStage();
	const	VertexShaderStage* shaderStage() const;

protected:
};

inline			VertexShaderStage* MaterialPass_VertexStage::shaderStage()			{ return sCast<VertexShaderStage*>(_shaderStage); }
inline const	VertexShaderStage* MaterialPass_VertexStage::shaderStage() const	{ return sCast<VertexShaderStage*>(_shaderStage); }

struct MaterialPass_PixelStage : public MaterialPass_Stage
{
public:
			PixelShaderStage* shaderStage();
	const	PixelShaderStage* shaderStage() const;

protected:
};

inline			PixelShaderStage* MaterialPass_PixelStage::shaderStage()		{ return sCast<PixelShaderStage*>(_shaderStage); }
inline const	PixelShaderStage* MaterialPass_PixelStage::shaderStage() const	{ return sCast<PixelShaderStage*>(_shaderStage); }

struct MaterialPass_ComputeStage : public MaterialPass_Stage
{
public:
			ComputeShaderStage* shaderStage();
	const	ComputeShaderStage* shaderStage() const;

protected:
};

inline			ComputeShaderStage* MaterialPass_ComputeStage::shaderStage()		{ return sCast<ComputeShaderStage*>(_shaderStage); }
inline const	ComputeShaderStage* MaterialPass_ComputeStage::shaderStage() const	{ return sCast<ComputeShaderStage*>(_shaderStage); }

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
	using Stage			= MaterialPass_Stage;
	using VertexStage	= MaterialPass_VertexStage;
	using PixelStage	= MaterialPass_PixelStage;
	using ComputeStage	= MaterialPass_ComputeStage;

	using Info = ShaderInfo::Pass;

public:
	MaterialPass();
	virtual ~MaterialPass();

	void create(Material* material, ShaderPass* shaderPass);
	void destroy();

	//void bind(RenderContext* ctx, const VertexLayout* vtxLayout);

	template<class TEX> void setTexParam	(StrView name, TEX* v);
	template<class T>	void setParam		(StrView name, const T& v);
						void setSamplerParam(StrView name, const SamplerState& samplerState);
						void setBufferParam	(StrView name, RenderGpuBuffer* v);

	const Info& info() const;

	VertexStage*	vertexStage();
	PixelStage*		pixelStage();
	ComputeStage*	computeStage();

	Material&		material();
	ShaderPass&		shaderPass();

protected:
	virtual void onCreate(Material* material, ShaderPass* shaderPass);
	virtual void onDestroy();

	//virtual void onBind(RenderContext* ctx, const VertexLayout* vtxLayout) = 0;

protected:
	Material*		_material		= nullptr;
	ShaderPass*		_shaderPass		= nullptr;
	VertexStage*	_vertexStage	= nullptr;
	PixelStage*		_pixelStage		= nullptr;
	ComputeStage*	_computeStage	= nullptr;
};

template<class TEX> inline 
void 
MaterialPass::setTexParam(StrView name, TEX* v)
{
	auto* mtl = _material;
	if (_vertexStage)	_vertexStage ->setTexParam(mtl, name, v);
	if (_pixelStage)	_pixelStage  ->setTexParam(mtl, name, v);
	if (_computeStage)	_computeStage->setTexParam(mtl, name, v);
}

template<class T> inline 
void 
MaterialPass::setParam(StrView name, const T& v)
{
	auto* mtl = _material;
	if (_vertexStage)	_vertexStage ->setParam(mtl, name, v);
	if (_pixelStage)	_pixelStage	 ->setParam(mtl, name, v);
	if (_computeStage)	_computeStage->setParam(mtl, name, v);
}

inline
void 
MaterialPass::setSamplerParam(StrView name, const SamplerState& samplerState)
{
	auto* mtl = _material;
	if (_vertexStage)	_vertexStage ->setSamplerParam(mtl, name, samplerState);
	if (_pixelStage)	_pixelStage  ->setSamplerParam(mtl, name, samplerState);
	if (_computeStage)	_computeStage->setSamplerParam(mtl, name, samplerState);
}

inline
void 
MaterialPass::setBufferParam(StrView name, RenderGpuBuffer* v)
{
	auto* mtl = _material;
	if (_computeStage)	_computeStage->setBufferParam(mtl, name, v);
}


inline const MaterialPass::Info& MaterialPass::info() const { return _shaderPass->info(); }

inline MaterialPass::VertexStage*	MaterialPass::vertexStage()		{ return _vertexStage; }
inline MaterialPass::PixelStage*	MaterialPass::pixelStage()		{ return _pixelStage; }
inline MaterialPass::ComputeStage*	MaterialPass::computeStage()	{ return _computeStage; }

inline Material&					MaterialPass::material()	{ return *_material; }
inline ShaderPass&					MaterialPass::shaderPass()	{ return *_shaderPass; }

#endif

}