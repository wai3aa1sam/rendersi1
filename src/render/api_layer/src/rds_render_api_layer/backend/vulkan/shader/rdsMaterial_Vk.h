#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/shader/rdsMaterial.h"
#include "rdsShader_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

class Shader_Vk;
class ShaderPass_Vk;
class Material_Vk;
class MaterialPass_Vk;

struct Vk_VertexShaderStage;
struct Vk_PixelShaderStage;

class RenderContext_Vk;

class Vk_RenderPass;
class Vk_Framebuffer;

using Vk_FramedDescrSets = Vector<Vk_DescriptorSet, RenderApiLayerTraits::s_kFrameInFlightCount>;

struct MaterialStage_Helper;


#if 0
#pragma mark --- rdsVk_MaterialPassStage-Decl ---
#endif // 0
#if 1

template<class MATERIAL_PASS_STAGE, class VK_SHADER_PASS_STAGE>
struct Vk_MaterialPassStage : public MATERIAL_PASS_STAGE
{
	friend struct	Vk_PipelineLayoutCDesc;
	friend struct	MaterialStage_Helper;
	friend class	MaterialPass_Vk;
public:
	using Base = MATERIAL_PASS_STAGE;
	using Util = Vk_RenderApiUtil;
	using Vk_ShaderPassStage_T	 = VK_SHADER_PASS_STAGE;

public:
	Vk_MaterialPassStage()
	{

	}

	~Vk_MaterialPassStage()
	{
		destroy();
	}

	void create(MaterialPass* pass, ShaderStage* shaderStage)
	{
		Base::create(pass, shaderStage);
	}

	void destroy()
	{
		Base::destroy();
	}

public:
	RenderDevice_Vk*		renderDeviceVk()	{ return pass->shader()->renderDeviceVk(); }
	Vk_ShaderPassStage_T*	vkShaderStage()		{ return sCast<Vk_ShaderPassStage_T*>(shaderStage()); }

protected:
};

struct Vk_MaterialPass_VertexStage : public Vk_MaterialPassStage<MaterialPass_VertexStage, Vk_VertexShaderStage>
{
	friend struct	Vk_PipelineLayoutCDesc;
	friend struct	MaterialStage_Helper;
	friend class	MaterialPass_Vk;
public:
	using Base		= Vk_MaterialPassStage<MaterialPass_VertexStage, Vk_VertexShaderStage>;

public:
	static constexpr SizeType s_kVtxInputAttrLocalSize = 32;
	using Vk_VertexInputAttrs = Vector<VkVertexInputAttributeDescription, s_kVtxInputAttrLocalSize>;

public:
	const Vk_VertexInputAttrs& cacheVtxInputAttrCDesc(const VertexLayout* vtxLayout);

protected:
	VectorMap<const VertexLayout*, Vk_VertexInputAttrs>	_vtxInputAttrsMap;
};

struct Vk_MaterialPass_TessellationControlStage : public Vk_MaterialPassStage<MaterialPass_TessellationControlStage, Vk_TessellationControlShaderStage>
{
public:
	using Base = Vk_MaterialPassStage<MaterialPass_TessellationControlStage, Vk_TessellationControlShaderStage>;
};

struct Vk_MaterialPass_TessellationEvaluationStage : public Vk_MaterialPassStage<MaterialPass_TessellationEvaluationStage, Vk_TessellationEvaluationShaderStage>
{
public:
	using Base = Vk_MaterialPassStage<MaterialPass_TessellationEvaluationStage, Vk_TessellationEvaluationShaderStage>;
};

struct Vk_MaterialPass_GeometryStage : public Vk_MaterialPassStage<MaterialPass_GeometryStage, Vk_GeometryShaderStage>
{
public:
	using Base = Vk_MaterialPassStage<MaterialPass_GeometryStage, Vk_GeometryShaderStage>;
};

struct Vk_MaterialPass_PixelStage : public Vk_MaterialPassStage<MaterialPass_PixelStage, Vk_PixelShaderStage>
{
public:
	using Base = Vk_MaterialPassStage<MaterialPass_PixelStage, Vk_PixelShaderStage>;
};

struct Vk_MaterialPass_ComputeStage : public MaterialPass_ComputeStage
{
public:
	using Base = Vk_MaterialPassStage<MaterialPass_PixelStage, Vk_PixelShaderStage>;
};

#endif

#if 0
#pragma mark --- rdsMaterialPass_Vk-Decl ---
#endif // 0
#if 1

class MaterialPass_Vk : public MaterialPass
{
	friend struct Vk_MaterialPass_VertexStage;
	friend struct Vk_MaterialPass_TessellationControlStage;
	friend struct Vk_MaterialPass_TessellationEvaluationStage;
	friend struct Vk_MaterialPass_GeometryStage;
	friend struct Vk_MaterialPass_PixelStage;
	friend struct Vk_MaterialPass_ComputeStage;
	friend struct Vk_PipelineLayoutCDesc;
public:
	using Base = MaterialPass;
	using Util = Vk_RenderApiUtil;

	using VertexStage					= Vk_MaterialPass_VertexStage;
	using TessellationControlStage		= Vk_MaterialPass_TessellationControlStage;
	using TessellationEvaluationStage	= Vk_MaterialPass_TessellationEvaluationStage;
	using GeometryStage					= Vk_MaterialPass_GeometryStage;
	using PixelStage					= Vk_MaterialPass_PixelStage;
	using ComputeStage					= Vk_MaterialPass_ComputeStage;

public:
	MaterialPass_Vk();
	virtual ~MaterialPass_Vk();

	virtual void onBind(RenderContext* ctx, const VertexLayout* vtxLayout, RenderPrimitiveType primitive, Vk_CommandBuffer* vkCmdBuf, u32 iFrame);
	virtual void onBind(RenderContext* ctx, Vk_CommandBuffer* vkCmdBuf, u32 iFrame);

protected:
	void bindDescriptorSet(VkPipelineBindPoint vkBindPt, RenderContext* ctx, Vk_CommandBuffer* vkCmdBuf, u32 iFrame);

public:
	Material_Vk*	material	();
	Shader_Vk*		shader		();
	ShaderPass_Vk*	shaderPass	();

	VertexStage*					vkVertexStage();
	TessellationControlStage*		vkTessellationControlStage();
	TessellationEvaluationStage*	vkTessellationEvaluationStage();
	GeometryStage*					vkGeometryStage();
	PixelStage*						vkPixelStage();
	ComputeStage*					vkComputeStage();

	VertexStage&					vkVertexStage_noCheck();
	TessellationControlStage&		vkTessellationControlStage_noCheck();
	TessellationEvaluationStage&	vkTessellationEvaluationStage_noCheck();
	GeometryStage&					vkGeometryStage_noCheck();
	PixelStage&						vkPixelStage_noCheck();
	ComputeStage&					vkComputeStage_noCheck();

	Vk_PipelineLayout& vkPipelineLayout();
	RenderDevice_Vk* renderDeviceVk();

	Vk_DescriptorSetLayout& vkDescriptorSetLayout();
	Vk_DescriptorSet&		vkDescriptorSet(u32 iFrame);

protected:
	virtual void onCreate(Material* material, ShaderPass* shaderPass) override;
	virtual void onDestroy() override;

	void bindPipeline(Vk_CommandBuffer* vkCmdBuf, Vk_RenderPass* vkRdPass, const VertexLayout* vtxLayout, RenderPrimitiveType primitive);

protected:
	VertexStage						_vkVertexStage;
	TessellationControlStage		_vkTescStage;
	TessellationEvaluationStage		_vkTeseStage;
	GeometryStage					_vkGeometryStage;
	PixelStage						_vkPixelStage;
	ComputeStage					_vkComputeStage;

	// TODO: use a union reflection, no, use bindless then only the Material_Vk has it
	Vk_FramedDescrSets		_vkFramedDescrSets;
	
	// per Renderer Pipeline state, created if not find 
	// after submit add to _vkPipelineMap
};

inline Material_Vk*						MaterialPass_Vk::material()				{ return sCast<Material_Vk*>(_material); }
inline Shader_Vk*						MaterialPass_Vk::shader()				{ return reinCast<Shader_Vk*>(_material->shader()); }
inline ShaderPass_Vk*					MaterialPass_Vk::shaderPass()			{ return reinCast<ShaderPass_Vk*>(_shaderPass); }

inline MaterialPass_Vk::VertexStage*					MaterialPass_Vk::vkVertexStage()					{ return vertexStage()					? &_vkVertexStage	: nullptr; }
inline MaterialPass_Vk::TessellationControlStage*		MaterialPass_Vk::vkTessellationControlStage()		{ return tessellationControlStage()		? &_vkTescStage		: nullptr; }
inline MaterialPass_Vk::TessellationEvaluationStage*	MaterialPass_Vk::vkTessellationEvaluationStage()	{ return tessellationEvaluationStage()	? &_vkTeseStage		: nullptr; }
inline MaterialPass_Vk::GeometryStage*					MaterialPass_Vk::vkGeometryStage()					{ return geometryStage()				? &_vkGeometryStage	: nullptr; }
inline MaterialPass_Vk::PixelStage*						MaterialPass_Vk::vkPixelStage()						{ return pixelStage()					? &_vkPixelStage	: nullptr; }
inline MaterialPass_Vk::ComputeStage*					MaterialPass_Vk::vkComputeStage()					{ return computeStage()					? &_vkComputeStage	: nullptr; }

inline MaterialPass_Vk::VertexStage&					MaterialPass_Vk::vkVertexStage_noCheck()					{ return _vkVertexStage; }
inline MaterialPass_Vk::TessellationControlStage&		MaterialPass_Vk::vkTessellationControlStage_noCheck()		{ return _vkTescStage; }
inline MaterialPass_Vk::TessellationEvaluationStage&	MaterialPass_Vk::vkTessellationEvaluationStage_noCheck()	{ return _vkTeseStage; }
inline MaterialPass_Vk::GeometryStage&					MaterialPass_Vk::vkGeometryStage_noCheck()					{ return _vkGeometryStage; }
inline MaterialPass_Vk::PixelStage&						MaterialPass_Vk::vkPixelStage_noCheck()						{ return _vkPixelStage; }
inline MaterialPass_Vk::ComputeStage&					MaterialPass_Vk::vkComputeStage_noCheck()					{ return _vkComputeStage; }

inline Vk_DescriptorSetLayout&			MaterialPass_Vk::vkDescriptorSetLayout()		{ return shaderPass()->vkDescriptorSetLayout(); }
inline Vk_DescriptorSet&				MaterialPass_Vk::vkDescriptorSet(u32 iFrame)	{ return _vkFramedDescrSets[iFrame]; }


#endif


#if 0
#pragma mark --- rdsMaterial_Vk-Decl ---
#endif // 0
#if 1

class Material_Vk : public RenderResource_Vk<Material>
{
public:
	using Base = RenderResource_Vk<Material>;

	using Pass			= MaterialPass_Vk;
	using VertexStage	= Vk_MaterialPass_VertexStage;
	using PixelStage	= Vk_MaterialPass_PixelStage;

public:
	Material_Vk();
	~Material_Vk();

protected:
	virtual void onCreate		(const CreateDesc& cDesc) override;
	virtual void onPostCreate	(const CreateDesc& cDesc) override;
	virtual void onDestroy		();

	virtual UPtr<MaterialPass> onMakePass();

protected:


};

inline
UPtr<MaterialPass> 
Material_Vk::onMakePass()
{
	return makeUPtr<Pass>();
}


#endif

inline RenderDevice_Vk*	MaterialPass_Vk::renderDeviceVk() { return material()->renderDeviceVk(); }

}
#endif