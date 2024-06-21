#include "rds_render-pch.h"
#include "rdsRpfSkybox.h"

namespace rds
{

#if 0
#pragma mark --- rdsRpfSkybox-Impl ---
#endif // 0
#if 1

RpfSkybox::RpfSkybox()
{

}

RpfSkybox::~RpfSkybox()
{
	destroy();
}

void 
RpfSkybox::create()
{
	createMaterial(&_shaderSkybox, &_mtlSkybox, "asset/shader/demo/geometry_buffer/rdsSkybox.shader");
}

void 
RpfSkybox::destroy()
{
	_mtlSkybox.reset(nullptr);
	_shaderSkybox.reset(nullptr);
}

RdgPass* 
RpfSkybox::addSkybox(RdgTextureHnd rtColor)
{
	//auto*		rdGraph		= renderGraph();
	//auto*		drawData	= drawDataBase();
	RdgPass*	passGeom	= nullptr;

	/*auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	RdgTextureHnd texNormal		= rdGraph->createTexture("gBuf_normal",			Texture2D_CreateDesc{ screenSize, ColorType::RGh,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });
	RdgTextureHnd texAlbedo		= rdGraph->createTexture("gBuf_albedo",			Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });
	result.debugPosition		= rdGraph->createTexture("gBuf_debugPosition",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAf,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });

	{
		auto& pass = rdGraph->addPass("geometry_pass", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(texNormal,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setRenderTarget(texAlbedo,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setRenderTarget(result.debugPosition,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf, RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = _mtlSkybox;
				rdReq.reset(rdGraph->renderContext(), drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				drawData->drawScene(rdReq, mtl, drawSettings);
			}
		);
		passGeom = &pass;
	}

	result.normal	= texNormal;
	result.albedo	= texAlbedo;*/

	return passGeom;
}


#endif

}