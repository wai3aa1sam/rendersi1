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

using Vk_FramedDescSets = Vector<Vk_DescriptorSet, RenderApiLayerTraits::s_kFrameInFlightCount>;

struct MaterialStage_Helper;


#if 0
#pragma mark --- rdsVk_MaterialPassStage-Decl ---
#endif // 0
#if 1

struct Vk_MaterialPass_VertexStage : public MaterialPass_VertexStage
{
	friend struct	Vk_PipelineLayoutCDesc;
	friend struct	MaterialStage_Helper;
	friend class	MaterialPass_Vk;
public:
	using Base		= MaterialPass_VertexStage;
	using Util		= Vk_RenderApiUtil;
	using Helper	= MaterialStage_Helper;

public:
	static constexpr SizeType s_kVtxInputAttrLocalSize = 32;
	using Vk_VertexInputAttrs = Vector<VkVertexInputAttributeDescription, s_kVtxInputAttrLocalSize>;

public:
	Vk_MaterialPass_VertexStage();
	~Vk_MaterialPass_VertexStage();

	void create	(MaterialPass_Vk* pass, ShaderStage* shaderStage);
	void destroy(MaterialPass_Vk* pass);

	void bind(RenderContext_Vk* ctx, Vk_CommandBuffer* vkCmdBuf, const VertexLayout* vtxLayout, MaterialPass_Vk* pass);

	const Vk_VertexInputAttrs& cacheVtxInputAttrCDesc(const VertexLayout* vtxLayout);

	Vk_VertexShaderStage*	vkShaderStage();

protected:

protected:
	Vk_DescriptorSetLayout	_vkDescriptorSetLayout;
	Vk_FramedDescSets		_vkFramedDescSets;

	VectorMap<const VertexLayout*, Vk_VertexInputAttrs>	_vtxInputAttrsMap;
};

inline Vk_VertexShaderStage* Vk_MaterialPass_VertexStage::vkShaderStage() { return sCast<Vk_VertexShaderStage*>(shaderStage()); }

struct Vk_MaterialPass_PixelStage : public MaterialPass_PixelStage
{
	friend struct	Vk_PipelineLayoutCDesc;
	friend struct	MaterialStage_Helper;
	friend class	MaterialPass_Vk;
public:
	using Base		= MaterialPass_PixelStage;
	using Util		= Vk_RenderApiUtil;
	using Helper	= MaterialStage_Helper;

public:
	void create	(MaterialPass_Vk* pass, ShaderStage* shaderStage);
	void destroy(MaterialPass_Vk* pass);

	void bind(RenderContext_Vk* ctx, Vk_CommandBuffer* vkCmdBuf, const VertexLayout* vtxLayout, MaterialPass_Vk* pass);

	Vk_PixelShaderStage*	vkShaderStage();

protected:
	Vk_DescriptorSetLayout	_vkDescriptorSetLayout;
	Vk_FramedDescSets		_vkFramedDescSets;
};

inline Vk_PixelShaderStage* Vk_MaterialPass_PixelStage::vkShaderStage() { return sCast<Vk_PixelShaderStage*>(shaderStage()); }

struct Vk_MaterialPass_ComputeStage : public MaterialPass_ComputeStage
{
	friend struct	Vk_PipelineLayoutCDesc;
	friend struct	MaterialStage_Helper;
	friend class	MaterialPass_Vk;
public:
	using Base		= MaterialPass_ComputeStage;
	using Util		= Vk_RenderApiUtil;
	using Helper	= MaterialStage_Helper;

public:
	void create	(MaterialPass_Vk* pass, ShaderStage* shaderStage);
	void destroy(MaterialPass_Vk* pass);

	void bind(RenderContext_Vk* ctx, Vk_CommandBuffer* vkCmdBuf, MaterialPass_Vk* pass);

	Vk_ComputeShaderStage*	vkShaderStage();

protected:
	Vk_DescriptorSetLayout	_vkDescriptorSetLayout;
	Vk_FramedDescSets		_vkFramedDescSets;
};

inline Vk_ComputeShaderStage* Vk_MaterialPass_ComputeStage::vkShaderStage() { return sCast<Vk_ComputeShaderStage*>(shaderStage()); }


#endif

#if 0
#pragma mark --- rdsMaterialPass_Vk-Decl ---
#endif // 0
#if 1

class MaterialPass_Vk : public MaterialPass
{
	friend struct Vk_MaterialPass_VertexStage;
	friend struct Vk_MaterialPass_PixelStage;
	friend struct Vk_MaterialPass_ComputeStage;
public:
	using Base = MaterialPass;
	using Util = Vk_RenderApiUtil;

	using VertexStage	= Vk_MaterialPass_VertexStage;
	using PixelStage	= Vk_MaterialPass_PixelStage;
	using ComputeStage	= Vk_MaterialPass_ComputeStage;

	using Vk_PipelineMap = VectorMap<Vk_RenderPass*, Vk_Pipeline>;

public:
	MaterialPass_Vk();
	virtual ~MaterialPass_Vk();

	virtual void onBind(RenderContext* ctx, const VertexLayout* vtxLayout, Vk_CommandBuffer* vkCmdBuf);
	virtual void onBind(RenderContext* ctx, Vk_CommandBuffer* vkCmdBuf);

	Material_Vk*	material	();
	Shader_Vk*		shader		();
	ShaderPass_Vk*	shaderPass	();

	VertexStage*	vkVertexStage();
	PixelStage*		vkPixelStage();
	ComputeStage*	vkComputeStage();

	VertexStage&	vkVertexStage_NoCheck();
	PixelStage&		vkPixelStage_NoCheck();
	ComputeStage&	vkComputeStage_NoCheck();

	Vk_PipelineLayout& vkPipelineLayout();
	Vk_PipelineLayout& computeVkPipelineLayout();

	RenderDevice_Vk* renderDeviceVk();

protected:
	virtual void onCreate(Material* material, ShaderPass* shaderPass) override;
	virtual void onDestroy() override;

	void bindPipeline(Vk_CommandBuffer* vkCmdBuf, Vk_RenderPass* vkRdPass, const VertexLayout* vtxLayout);

protected:
	void createVkPipeline		(Vk_Pipeline& out, Vk_RenderPass* vkRdPass, const VertexLayout* vtxLayout);
	void createComputeVkPipeline(Vk_Pipeline& out);

protected:
	VertexStage		_vkVertexStage;
	PixelStage		_vkPixelStage;
	ComputeStage	_vkComputeStage;

	Vk_PipelineLayout	_vkPipelineLayout;		// maybe should in shader, since it does not need to recreate if no push_constant
	Vk_PipelineLayout	_computeVkPipelineLayout;		// maybe should in shader, since it does not need to recreate if no push_constant
	Vk_PipelineMap		_vkPipelineMap;
	Vk_Pipeline			_computeVkPipeline;

	// TODO: use a union reflection, no, use bindless then only the Material_Vk has it
	Vk_DescriptorSetLayout	_vkDescriptorSetLayout;
	Vk_FramedDescSets		_vkFramedDescSets;
	
	// per Renderer Pipeline state, created if not find 
	// after submit add to _vkPipelineMap
};

inline Vk_PipelineLayout&				MaterialPass_Vk::vkPipelineLayout()				{ return _vkPipelineLayout; }
inline Vk_PipelineLayout&				MaterialPass_Vk::computeVkPipelineLayout()		{ return _computeVkPipelineLayout; }

inline Material_Vk*						MaterialPass_Vk::material()				{ return sCast<Material_Vk*>(_material); }
inline Shader_Vk*						MaterialPass_Vk::shader()				{ return reinCast<Shader_Vk*>(_material->shader()); }
inline ShaderPass_Vk*					MaterialPass_Vk::shaderPass()			{ return reinCast<ShaderPass_Vk*>(_shaderPass); }

inline MaterialPass_Vk::VertexStage*	MaterialPass_Vk::vkVertexStage()		{ return vertexStage()	? &_vkVertexStage	: nullptr; }
inline MaterialPass_Vk::PixelStage*		MaterialPass_Vk::vkPixelStage()			{ return pixelStage()	? &_vkPixelStage	: nullptr; }
inline MaterialPass_Vk::ComputeStage*	MaterialPass_Vk::vkComputeStage()		{ return computeStage()	? &_vkComputeStage	: nullptr; }

inline MaterialPass_Vk::VertexStage&	MaterialPass_Vk::vkVertexStage_NoCheck()	{ return _vkVertexStage; }
inline MaterialPass_Vk::PixelStage&		MaterialPass_Vk::vkPixelStage_NoCheck()		{ return _vkPixelStage; }
inline MaterialPass_Vk::ComputeStage&	MaterialPass_Vk::vkComputeStage_NoCheck()	{ return _vkComputeStage; }

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