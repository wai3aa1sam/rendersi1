#include "rds_render-pch.h"
#include "rdsRenderPassFeature.h"
#include "rdsRenderPassPipeline.h"

#include "rds_render/pass_feature/utility/image/rdsRpfClearImage2D.h"
#include "rds_render/pass_feature/utility/image/rdsRpfClearImage3D.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderPassFeature-Impl ---
#endif // 0
#if 1

RenderPassFeature::RenderPassFeature()
{

}

RenderPassFeature::~RenderPassFeature()
{

}

RdgPass* RenderPassFeature::_addClearImage2DPass(SPtr<Material>& material, RdgTextureHnd image) { return _rdPassPipeline->_rpfClearImage2D->addClearImage2DPass(material, image); }
RdgPass* RenderPassFeature::_addClearImage3DPass(SPtr<Material>& material, RdgTextureHnd image) { return _rdPassPipeline->_rpfClearImage3D->addClearImage3DPass(material, image); }

void 
RenderPassFeature::setRenderPassPipeline(RenderPassPipeline* v)
{
	_rdPassPipeline = v;
}

RenderGraph*	RenderPassFeature::renderGraph()	{ return _rdPassPipeline->renderGraph(); }
DrawData_Base*	RenderPassFeature::drawDataBase()	{ return _rdPassPipeline->drawDataBase(); }
RenderDevice*	RenderPassFeature::renderDevice()	{ return renderGraph()->renderContext()->renderDevice(); }

#endif

#if 0
#pragma mark --- rdsRenderUtil-Impl ---
#endif // 0
#if 1

void 
RenderUtil::createShader(SPtr<Shader>* oShader, StrView filename)
{
	auto& shader	= *oShader;
	if (shader)
		return;

	shader	= Renderer::rdDev()->createShader(filename);
}

void 
RenderUtil::createMaterial(SPtr<Shader>* oShader, SPtr<Material>* oMtl, StrView filename, const Function<void(Material*)>& fnSetParam)
{
	auto& shader	= *oShader;
	auto& mtl		= *oMtl;

	if (mtl)
		return;

	createShader(&shader, filename);
	mtl		= Renderer::rdDev()->createMaterial();
	mtl->setShader(shader);

	if (fnSetParam)
		fnSetParam(mtl);
}

void 
RenderUtil::createMaterials(SPtr<Shader>* oShader,  Span<SPtr<Material> > oMtls, StrView filename)
{
	createShader(oShader, filename);
	for (auto& e : oMtls)
	{
		createMaterial(*oShader, &e);
	}
}

void 
RenderUtil::createMaterial(Shader* shader, SPtr<Material>* oMtl)
{
	//auto& shader	= *oShader;
	auto& mtl		= *oMtl;

	if (mtl)
		return;

	mtl		= Renderer::rdDev()->createMaterial();
	mtl->setShader(shader);
}

void
RenderUtil::destroyShader(SPtr<Shader>& shader)
{
	shader.reset(nullptr);
}

void
RenderUtil::destroyShaderMaterial(SPtr<Shader>& shader, SPtr<Material>& material)
{
	destroyMaterial(material);
	destroyShader(shader);
}

void 
RenderUtil::destroyShaderMaterials(SPtr<Shader>& shader, Span<SPtr<Material> >	materials)
{
	destroyMaterials(materials);
	destroyShader(shader);
}

void
RenderUtil::destroyMaterial(SPtr<Material>& material)
{
	material.reset(nullptr);
}

void
RenderUtil::destroyMaterials(Span<SPtr<Material>> materials)
{
	for (auto& e : materials)
	{
		destroyMaterial(e);
	}
}



#endif


}