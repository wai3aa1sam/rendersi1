#include "rds_render-pch.h"
#include "rdsRenderPassFeature.h"
#include "rdsRenderPassPipeline.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderPassFeature-Impl ---
#endif // 0
#if 1

void 
RenderPassFeature::createShader(SPtr<Shader>* oShader, StrView filename)
{
	auto& shader	= *oShader;
	if (shader)
		return;

	shader	= Renderer::rdDev()->createShader(filename);
}

void 
RenderPassFeature::createMaterial(SPtr<Shader>* oShader, SPtr<Material>* oMtl, StrView filename, const Function<void(Material*)>& fnSetParam)
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
RenderPassFeature::createMaterial(Shader* shader, SPtr<Material>* oMtl)
{
	//auto& shader	= *oShader;
	auto& mtl		= *oMtl;

	if (mtl)
		return;

	mtl		= Renderer::rdDev()->createMaterial();
	mtl->setShader(shader);
}


RenderPassFeature::RenderPassFeature()
{

}

RenderPassFeature::~RenderPassFeature()
{

}

void 
RenderPassFeature::setRenderPassPipeline(RenderPassPipeline* v)
{
	_rdPassPipeline = v;
}

RenderGraph*	RenderPassFeature::renderGraph()	{ return _rdPassPipeline->renderGraph(); }
DrawData_Base*	RenderPassFeature::drawDataBase()	{ return _rdPassPipeline->drawDataBase(); }
RenderDevice*	RenderPassFeature::renderDevice()	{ return renderGraph()->renderContext()->renderDevice(); }

#endif

}