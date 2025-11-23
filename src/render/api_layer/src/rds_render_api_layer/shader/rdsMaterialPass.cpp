#include "rds_render_api_layer-pch.h"
#include "rdsMaterial.h"
#include "rdsMaterialPass.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

#if 0
#pragma mark --- rdsMaterialPassStage-Impl ---
#endif // 0
#if 1

MaterialPass_Stage::MaterialPass_Stage()
{

}

MaterialPass_Stage::~MaterialPass_Stage()
{
	destroy();
}

void 
MaterialPass_Stage::create(MaterialPass* pass, ShaderStage* shaderStage)
{
	if (!shaderStage)
		return;

	destroy();
	_shaderStage = shaderStage;
}

void 
MaterialPass_Stage::destroy()
{
	
	this->_shaderStage = nullptr;
}

#endif

#if 0
#pragma mark --- rdsMaterialPass-Impl ---
#endif // 0
#if 1

MaterialPass::MaterialPass()
{

}

MaterialPass::~MaterialPass()
{

}

void 
MaterialPass::create(Material* material, ShaderPass* shaderPass) 
{ 
	onCreate(material, shaderPass); 
}

void 
MaterialPass::destroy()
{
	onDestroy();
}

int 
MaterialPass::uploadToGpu()
{
	return _shaderRscs.uploadToGpu();
}

//void 
//MaterialPass::bind(RenderContext* ctx, const VertexLayout* vtxLayout) 
//{ 
//	onBind(ctx, vtxLayout); 
//}

void 
MaterialPass::onCreate(Material* material, ShaderPass* shaderPass)
{
	_material	= material;
	_shaderPass = shaderPass;

	_shaderRscs.create(info().allStageUnionInfo, shaderPass);
}

void 
MaterialPass::onDestroy()
{
	_shaderRscs.destroy();
}

#endif

}