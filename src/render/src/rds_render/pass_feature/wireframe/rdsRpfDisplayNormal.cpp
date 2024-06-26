#include "rds_render-pch.h"
#include "rdsRpfDisplayNormal.h"

namespace rds
{

#if 0
#pragma mark --- rdsRpfDisplayNormal-Impl ---
#endif // 0
#if 1

RpfDisplayNormal::RpfDisplayNormal()
{

}

RpfDisplayNormal::~RpfDisplayNormal()
{
	destroy();
}

void 
RpfDisplayNormal::create()
{
	RenderUtil::createMaterial(&_shaderDisplayNormal, &_mtlDisplayNormal, "asset/shader/util/rdsDisplayNormals.shader");
}

void 
RpfDisplayNormal::destroy()
{
	RenderUtil::destroyShaderMaterial(_shaderDisplayNormal, _mtlDisplayNormal);
}

RdgPass* 
RpfDisplayNormal::addDisplayNormalPass(const DrawSettings& drawSettings, RdgTextureHnd rtColor)
{
	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passDisplayNormal	= nullptr;

	Material* mtl = _mtlDisplayNormal;
	{
		auto& pass = rdGraph->addPass("display_normal", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(rtColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				drawData->setupMaterial(mtl);
				drawData->drawScene(rdReq, mtl);
			});
		passDisplayNormal = &pass;
	}
	
	return passDisplayNormal;
}


#endif

}