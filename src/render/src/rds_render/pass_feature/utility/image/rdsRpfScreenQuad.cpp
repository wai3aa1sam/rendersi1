#include "rds_render-pch.h"
#include "rdsRpfScreenQuad.h"

namespace rds
{
SPtr<Shader> RpfScreenQuad::_shaderScreenQuad;

#if 0
#pragma mark --- rdsRpfScreenQuad-Impl ---
#endif // 0
#if 1

RpfScreenQuad::RpfScreenQuad()
{

}

RpfScreenQuad::~RpfScreenQuad()
{
	destroy();
}

void 
RpfScreenQuad::create()
{
	RenderUtil::createShader(&_shaderScreenQuad, "asset/shader/pass_feature/utility/image/rdsScreenQuad.shader");
}

void 
RpfScreenQuad::destroy()
{
	RenderUtil::destroyShader(_shaderScreenQuad);
}

RdgPass* 
RpfScreenQuad::addDrawScreenQuadPass(SPtr<Material>& mtl_, RdgTextureHnd rtColor, RdgTextureHnd texColor)
{
	RenderUtil::createMaterial(_shaderScreenQuad, &mtl_);

	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passScreenQuad	= nullptr;

	Material* mtl = mtl_;
	{
		auto passName = fmtAs_T<TempString>("screenQuad-{}", texColor.name());
		auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(rtColor, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				mtl->setParam("tex_color", texColor.texture2D());
				drawData->setupMaterial(mtl);
				rdReq.drawSceneQuad(RDS_SRCLOC, mtl);
			}
		);
		passScreenQuad = &pass;
	}

	return passScreenQuad;
}

#endif

}