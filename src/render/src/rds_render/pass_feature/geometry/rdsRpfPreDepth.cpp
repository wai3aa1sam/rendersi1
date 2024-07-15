#include "rds_render-pch.h"
#include "rdsRpfPreDepth.h"

namespace rds
{

#if 0
#pragma mark --- rdsRpfPreDepth-Impl ---
#endif // 0
#if 1

RpfPreDepth::RpfPreDepth()
{

}

RpfPreDepth::~RpfPreDepth()
{
	destroy();
}

void 
RpfPreDepth::create()
{
	RenderUtil::createMaterial(&_shaderPreDepth, &_mtlPreDepth, "asset/shader/pass_feature/geometry/rdsPreDepth.shader");
}

void 
RpfPreDepth::destroy()
{
	RenderUtil::destroyShaderMaterial(_shaderPreDepth, _mtlPreDepth);
}

RdgPass* 
RpfPreDepth::addPreDepthPass(const DrawSettings& drawSettings, RdgTextureHnd dsBuf, RdgTextureHnd* oTexDepth, Color4f clearColor)
{
	auto*		rdGraph			= renderGraph();
	auto*		drawData		= drawDataBase();
	RdgPass*	passPreDepth	= nullptr;

	//Material* mtl = _mtlPreDepth;
	{
		auto& pass = rdGraph->addPass("pre_depth", RdgPassTypeFlags::Graphics);
		if (oTexDepth)
			pass.setRenderTarget(*oTexDepth, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf, RenderAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);
		pass.setExecuteFunc(
			[=, drawSettings = drawSettings](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				if (oTexDepth)
					clearValue->setClearColor(clearColor);
				clearValue->setClearDepth();

				constCast(drawSettings).overrideShader = _shaderPreDepth;
				drawData->drawScene(rdReq, drawSettings);
			});
		passPreDepth = &pass;
	}

	return passPreDepth;
}


#endif

}