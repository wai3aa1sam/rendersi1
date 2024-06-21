#include "rds_render-pch.h"
#include "rdsRpfDeferred.h"

namespace rds
{

#if 0
#pragma mark --- rdsRpfDeferred-Impl ---
#endif // 0
#if 1

RpfDeferred::RpfDeferred()
{

}

RpfDeferred::~RpfDeferred()
{
	destroy();
}

void 
RpfDeferred::create()
{
	//createMaterial(&_shaderDeferred, &_mtlDeferred, "asset/shader/demo/geometry_buffer/rdsDeferred.shader");
}

void 
RpfDeferred::destroy()
{
	_mtlDeferred.reset(nullptr);
	_shaderDeferred.reset(nullptr);
}

RdgPass* 
RpfDeferred::addGeometryPass(const DrawSettings& drawSettings, RdgTextureHnd dsBuf)
{
	auto*	rdGraph		= renderGraph();
	auto*	drawData	= drawDataBase();

	auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	RdgTextureHnd texNormal		= rdGraph->createTexture("gBuf_normal",			Texture2D_CreateDesc{ screenSize, ColorType::RGh,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });
	RdgTextureHnd texAlbedo		= rdGraph->createTexture("gBuf_albedo",			Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });

	RdgPass* passGeom = nullptr;
	{
		auto& pass = rdGraph->addPass("geometry_pass", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(texNormal,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setRenderTarget(texAlbedo,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf, RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = _mtlDeferred;
				rdReq.reset(rdGraph->renderContext(), drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				drawData->drawScene(rdReq, mtl, drawSettings);
			}
		);
		passGeom = &pass;
	}

	//result.normal	= texNormal;
	//result.albedo	= texAlbedo;

	return passGeom;
}


#endif

}